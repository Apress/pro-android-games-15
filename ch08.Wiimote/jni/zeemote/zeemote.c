
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>

//#include <linux/uinput.h>

#ifdef ANDROID
	#include <android/log.h>

	#define LOG_TAG "Zee"
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
	#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
	#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
	#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__)
	#define printf LOGD
#endif

#define ZEEMOTE_AXIS_UNKNOWN 0
#define ZEEMOTE_AXIS_X       1
#define ZEEMOTE_AXIS_Y       2

#define ZEEMOTE_BUTTONS 7
#define ZEEMOTE_STICK   8
#define ZEEMOTE_BATTERY 17

#define ZEEMOTE_BUTTON_NONE  (0xfe)

#define ZEEMOTE_BUTTON_A 0
#define ZEEMOTE_BUTTON_B 1
#define ZEEMOTE_BUTTON_C 2
#define ZEEMOTE_BUTTON_D 3

#define ZEEMOTE_CLASS  0x000584   // Peripheral, Pointing device/Joystick

/* Byte order conversions */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htozs(d)  bswap_16(d)
#define ztohs(d)  bswap_16(d)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define htozs(d)  (d)
#define ztohs(d)  (d)
#else
#error "Unknown byte order"
#endif

#define KEY_UP                  103
#define KEY_LEFT                105
#define KEY_RIGHT               106
#define KEY_DOWN                108
#define KEY_A                   30
#define KEY_B                   48
#define KEY_C                   46
#define KEY_D                   32

static int keys[] = {
  KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, 
  KEY_A, KEY_B, KEY_C, KEY_D, 
  0 };

static char* keys_lbls[] = {
  "KEY_LEFT", "KEY_RIGHT", "KEY_UP", "KEY_DOWN", 
  "KEY_A", "KEY_B", "KEY_C", "KEY_D", 
  0 };

extern void zee_send_event(int idx, char *format, ...);

/**
 * connect
 */ 
int bluez_connect(bdaddr_t *bdaddr, int channel) {
  struct sockaddr_rc rem_addr;
  int bt;

  // bluez connects to BlueClient
  if( (bt = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0 ) {
    fprintf(stderr, "Can't create socket. %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  /* connect on rfcomm */
  memset(&rem_addr, 0, sizeof(rem_addr));
  rem_addr.rc_family = AF_BLUETOOTH;
  rem_addr.rc_bdaddr = *bdaddr;
  rem_addr.rc_channel = channel;
  
  if( connect(bt, (struct sockaddr *)&rem_addr, sizeof(rem_addr)) < 0 ){
    printf("Can't connect. %s(%d)\n", strerror(errno), errno);
    close(bt);
    return -1;
  }

  return bt;
}

#define MAGIC  0xa1

typedef struct {
  unsigned char length;
  unsigned char magic;
  unsigned char type;
} zeemote_hdr_t;

ssize_t read_num(int fd, void *data, size_t count) {
  ssize_t total = 0;

  while(count) {
    ssize_t rd = read(fd, data, count);
    if(rd < 0) {
      perror("read");
      return rd;
    } else {
      count -= rd;
      data += rd;
      total += rd;
    }
  }
  return total;
}

bdaddr_t *inquiry() {
  inquiry_info *info = NULL;
  uint8_t lap[3] = { 0x33, 0x8b, 0x9e };
  int num_rsp, i, mote;
  bdaddr_t *result = NULL;

  printf("Inquiry: Scaning for devices...\n");
  
  num_rsp = hci_inquiry(-1, 8, 0, lap, &info, 0);
  
  if (num_rsp < 0) {
    printf("Inquiry failed: %s", strerror(errno));
    return NULL;
  }

  for (mote=0, i = 0; i < num_rsp; i++) 
    if((info+i)->dev_class[0] == ((ZEEMOTE_CLASS >> 0) & 0xff) &&
       (info+i)->dev_class[1] == ((ZEEMOTE_CLASS >> 8) & 0xff) &&
       (info+i)->dev_class[2] == ((ZEEMOTE_CLASS >> 16) & 0xff)) 
      mote++;

  if(!mote) {
    printf("No zeemotes found\n");
    return result;
  }

  result = malloc(sizeof(bdaddr_t) * (mote + 1));

  for (mote=0, i = 0; i < num_rsp; i++) {
    if((info+i)->dev_class[0] == ((ZEEMOTE_CLASS >> 0) & 0xff) &&
       (info+i)->dev_class[1] == ((ZEEMOTE_CLASS >> 8) & 0xff) &&
       (info+i)->dev_class[2] == ((ZEEMOTE_CLASS >> 16) & 0xff)) {
      result[mote++] = (info+i)->bdaddr; 
    }
  }
  
  bacpy(&result[mote++], BDADDR_ANY);
  return result;
}

typedef union {
	char axis[3];
	unsigned char buttons[6];
	unsigned short voltage;
	char dummy[48];
} zeeData;

/**
 * Status helper
 */
static void SendStatusEvent(int error, char *format, ...) 
{
	va_list         argptr;
	char     string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	zee_send_event(0, "event=EV_STATUS|error=%d|message=%s", error, string);
}

/**
 * Stick reader
 */ 
static void ProcessStick(int bt, zeemote_hdr_t hdr) //, zeeData data)
{
	zeeData data;
	if(hdr.length-2 == sizeof(data.axis)) {
		if(read_num(bt, data.axis, sizeof(data.axis))) {
			if(data.axis[ZEEMOTE_AXIS_UNKNOWN]) {
				printf("WARNING: ZEEMOTE_STICK axis UNKNOWN != 0!\n");
				return;
			}
			int X = data.axis[ZEEMOTE_AXIS_X];
			int Y = data.axis[ZEEMOTE_AXIS_Y];
			int scaleX = (X <= 127) ? (X/127.0f) * 100 : ((X-255)/127.0f) * 100;
			int scaleY = (Y <= 127) ? (Y/127.0f) * 100 : ((255-Y)/127.0f) * -100;
			
			//printf("Axis X=%d Y=%d SX=%d SY=%d\n", X, Y, scaleX, scaleY );
			zee_send_event(0, "event=EV_JOYSTICK|X=%d|Y=%d|SX=%d|SY=%d", X, Y, scaleX, scaleY );
		} 
		/*
		else 
			printf("ERROR: reading ZEEMOTE_STICK payload failed\n");
		*/
	} 
	else {
		//printf("ERROR: unexpected length %d in ZEEMOTE_STICK\n", hdr.length);
		read_num(bt, data.dummy, hdr.length - 2);
	}
}

/**
 * Buttons
 */
static void ProcessButtons(int bt, zeemote_hdr_t hdr) 
{
	zeeData data;
	if(hdr.length-2 == sizeof(data.buttons)) {
		if(read_num(bt, data.buttons, sizeof(data.buttons))) {
			//int i, buttons = 0;
			int pressed = data.buttons[0] != 254;
			static btn = 0;
			
			if ( data.buttons[0] !=  ZEEMOTE_BUTTON_NONE )
				btn = data.buttons[0];
				
			//printf("Buttons %d lbl=%s pressed=%d\n", btn , keys_lbls[4 + btn], pressed );
			
			zee_send_event(0, "event=EV_KEY|button=%d|label=%s|pressed=%d", btn , keys_lbls[4 + btn], pressed );
		}
		else { 
			//printf("ERROR: reading ZEEMOTE_BUTTONS payload failed\n");
		}
	} 
	else {
	  printf("ERROR: unexpected length %d in ZEEMOTE_BUTTONS\n", hdr.length);
	  read_num(bt, data.dummy, hdr.length - 2);
	}
}

/**
 * Battery
 */
static void ProcessBattery(int bt, zeemote_hdr_t hdr) 
{
	zeeData data;
	if(hdr.length-2 == sizeof(data.voltage)) {
		if(read_num(bt, &data.voltage, sizeof(data.voltage))) {
			//printf("ZEEMOTE_BATTERY: %d.%03u volts\n", ztohs(data.voltage)/1000, ztohs(data.voltage)%1000);
			zee_send_event(0, "event=EV_BATTERY|value=%d.%03u|unit=volts", ztohs(data.voltage)/1000, ztohs(data.voltage)%1000);
		} 
		else { 
			//printf("ERROR: reading ZEEMOTE_BATTERY payload failed\n");
		}
	} 
	else {
		printf("ERROR: unexpected length %d in ZEEMOTE_BATTERY\n",  hdr.length);
		read_num(bt, data.dummy, hdr.length - 2);
	}
}


/**
 * 1 zeemote only :(
 */
static int done = 0;
 
void ProcessZeemotes(bdaddr_t *bdaddr)
{
	int bt;
	// for each zeemote
	/* 1st zeemote only
	while(bacmp(bdaddr, BDADDR_ANY)) { */
		char addr[18];
		ba2str(bdaddr, addr);

		// open bluetooth rfcomm
		if((bt = bluez_connect(bdaddr, 1)) >= 0) {
			//printf("Connect: Connected to Zeemote %s", addr);
			SendStatusEvent(0, "Connected to Zeemote %s", addr);
			done = 0;
			
			while( !done) {
				zeemote_hdr_t hdr;
				zeeData data;
				
				int rd = read_num(bt, &hdr, sizeof(hdr));
				
				if(rd == sizeof(hdr)) {
					if(hdr.magic == MAGIC && hdr.length >= 2) {
						switch(hdr.type) {
							case ZEEMOTE_STICK:
								ProcessStick(bt, hdr); //, data);
								break;
								
							case ZEEMOTE_BATTERY:
								ProcessBattery(bt, hdr);
								break;
							
							case ZEEMOTE_BUTTONS:
								ProcessButtons(bt, hdr);
								break;
							
							default:
								if(hdr.length - 2 > sizeof(data.dummy)) {
									printf("%d bytes of unknown command %d exceeding limits\n", hdr.length-2, hdr.type);
								}
								read_num(bt, data.dummy, hdr.length - 2);
								break;
						}
					}
				}
			}
		}
		else {
			//printf("Connect: Connection to %s failed\n", addr);
			SendStatusEvent(1, "Connection to %s failed", addr);
			
			close(bt);
			return;
		}
		close(bt);
		
		//printf("Connect: Disconnected fromm Zeemote %s", addr);
		SendStatusEvent(0, "Disconnected fromm Zeemote %s", addr);
	/*	
	bdaddr++;
	} 
	*/
}  

void DiscoverAndConnect () 
{
	bdaddr_t *bdaddr = NULL;
	
	zee_send_event(0, "event=EV_DISCOVER|status=Scanning");
	
    bdaddr = inquiry();
	
    if(!bdaddr) {
		//printf("Discover: No devices found\n");
		SendStatusEvent(1, "No devices found");
		return;
    }
	ProcessZeemotes (bdaddr);
}

void Disconnect ()
{
	done = 1;
}
