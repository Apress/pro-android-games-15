package game.engine.controller;


import java.io.ByteArrayInputStream;
import java.util.Properties;

import android.app.Activity;
import android.os.Handler;
import android.util.Log;

public class Wiimote implements IController
{
	static final String TAG =  "Wiimote";

	static final int[] LEDS = { 0x00, 0x10, 0x20, 0x40, 0x80};
	
	// JNI error codes
	static final int RC_NOERROR = 0;
	static final int RC_ERROR_INIT = 100;
	static final int RC_ERROR_DISCOVERY = 101;
	static final int RC_ERROR_DISCOVERY_NOWIIMOTES = 102;
	
	
	//static final int RC_ERROR_OPEN = 300;
	static final int RC_ERROR_CONNECT = 110;
	static final int RC_ERROR_DISCONNECTED = 120;
	static final int RC_ERROR_ALL_DISCONNECTED = 121;
	
	static {
		System.loadLibrary("wiimote");
	}

	// Native subs
	static private native void Initialize();
	
	// this subs blocks execution
	static private native void DiscoverAndConnect();
	static private native void DisconnectAll();
	static private native void ToggleRumble();
	static private native void Status();
	static private native void SetLed(int led);

	static private native void Connect(String address);
	
	// event listener
	private static IController.IControllerListener mListener;

	// Natice sub to send an array of bytes to the Wiimote
	// used for testing.
	static private native void SendBytes(byte[] data); 
	
	// parent
	private Activity mActivity;
	private Handler mHandler;
	
	// Coontroller connected?
	private static boolean connected = false;
	
	/**
	 * Constructor 
	 */
	public Wiimote(Activity ctx) { //, eTargetApp target) {
		mActivity = ctx;
		mHandler = new Handler(mActivity.getMainLooper());
		//mTarget = target;
		
		Initialize();
	}
	
	public void connect() {
		Log.d(TAG, "Starting connection thread");
		
		new Thread(new Runnable() {
			public void run() {
				DiscoverAndConnect();
			}
		}).start();
	}

	public void connect(final String address) {
		Log.d(TAG, "Starting connection thread");
		
		new Thread(new Runnable() {
			public void run() {
				Connect(address);
			}
		}).start();
	}

	public void sendBytes(byte[] data) {
		SendBytes(data); 
	}
	
	public void disconnect() {
		Log.d(TAG, "Disconnecting...");
		DisconnectAll();
	}

	public void rumble() {
		ToggleRumble();
	}
	
	/**
	 * Get status
	 */
	public void getStatus() {
		// TODO: native sub not working
		Status();
	}

	public void setLed(int num) {
		if ( num > LEDS.length) return;
		if ( num < 0) return;
		
		SetLed(LEDS[num]);
	}
	
	/**
	 * JNI callback: fires when there is a message
	 * @param message
	 */
	private static void OnMessage(int rc, String message) {
		switch (rc) {
		case RC_NOERROR:
			if ( message != null && message.contains("Connected")) {
				
				Log.d(TAG, "Connection OK");
				connected = true;
				
				if ( mListener != null) {
					mListener.connected();
				}
			}
			else {
				Log.d(TAG, message);
				if ( mListener != null) {
					mListener.onMessage(message);
				}
			}
			break;

		default:
			Log.e(TAG, "Error code " + rc + ": " + message);
			connected = false;
			if ( mListener != null) {
				mListener.disconnected("Error code " + rc + ": " + message);
			}
			break;
		}
	}
	
	/**
	 * JNI: Fires when there is a controller event: key, ....
	 * @param code
	 * @param message
	 */
	private static void OnEvent(int code, String message) {
		if ( message == null) return;
		
		// parse message as props: EVT_TYPE={BTNPRESS|BTNRELEASE}|BTN={BUTTON}
		try {
			Properties payload = new Properties();
			payload.load(new ByteArrayInputStream(message.replaceAll("\\|", "\n").getBytes()));

			String type = payload.getProperty("EVT_TYPE");
			
			if ( type.equals("NUNCHUK")) {
				handleNunchuckEvent(payload);
				return;
			}
			else if ( type.equals("EXT_INSERTED")) {
				String name = payload.getProperty("EXT_NAME"); //NUNCHUK)
				System.out.println("** Extension inserted " + name);
				return;
			}
			handleButtonEvent(payload);
		} catch (Exception e) {
			Log.e(TAG, "Wii:OnEvent: " + e);
		}
		
	}

	/**
	 * Button event handler
	 * @param payload
	 */
	private static void handleButtonEvent(Properties payload) {
		// button presses
		String type = payload.getProperty("EVT_TYPE");
		String button = payload.getProperty("BTN");
		
		Log.d(TAG, "Button Event: " + payload + " Type: " + type + " Btn:" + button);
/*		
		if ( mListener != null)
			mListener.onMessage("Event: " + payload + " Type: " + type + " Btn:" + button);
		
		
		// btn events for now...
		if ( type == null || button == null) {
			Log.e(TAG, "Invalid wii event: " + message);
			return;
		}
		
		int sc = Bindings.getWiiMapping(button);
		
		if ( sc < 0) {
			Log.e(TAG, "Invalid WII binding code for button " + button);
			return;
		}
		
		// get binding & send event
		if (type.equalsIgnoreCase("BTNPRESS")) {
			//Log.d(TAG, "Button press " + button + " code=" + sc);
			sendEvent(true, sc, sc);
		}
		else if (type.equalsIgnoreCase("BTNRELEASE")) {
			//Log.d(TAG, "Button release " + button + " code=" + sc);
			sendEvent(false, sc, sc);
		} */
	}
	
	/**
	 * Nunchuck handler
	 * @param payload
	 */
	private static void handleNunchuckEvent(Properties payload) {
		try {
			/**
			 * The angle \a ang is relative to the positive y-axis into quadrant I
			 * and ranges from 0 to 360 degrees. The magnitude is the distance from 
			 * the center to where the joystick is being held.
			 */
			float joyAngle = Float.parseFloat(payload.getProperty("JANGLE"));
			float joyMagnitude = Float.parseFloat(payload.getProperty("JMAGNITUDE"));
			
			if (joyAngle == Float.NaN || joyMagnitude < 0.4 ) {
				return;
			}

//			Log.d(TAG, "Nunchuck roll=" + roll + " pitch=" + pitch + " Yaw=" + yaw 
//					+ " JAngle=" + joyAngle + " JMag=" + joyMagnitude);
			
			if ( mListener != null) {
				//mListener.onJoysticEvent(dx, dy);
				mListener.onJoystickEvent(joyAngle, joyMagnitude);
			}
		} catch (NumberFormatException e) {
			Log.e(TAG, "Nunchuck parse error:" + e);
		}
	}
	/**
	 * END JNI CALLBACKS
	 */
	
	private static void sendEvent(boolean pressed, int ascii, int scanCode)  {
		if ( mListener != null) {
			mListener.onButtonEvent(pressed, ascii, scanCode);
		}
	}
	

	@Override
	public void setListener(IControllerListener listener) {
		mListener = listener;
	}

	@Override
	public boolean isConnected() {
		return connected;
	}

	
}
