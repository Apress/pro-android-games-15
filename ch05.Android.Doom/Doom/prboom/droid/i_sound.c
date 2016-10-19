/* $Id: i_sndsdl.c 512 2009-06-04 15:00:34Z sezero $
 *
 *  Ripped && Adapted from the PrBoom project:
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301  USA.
 *
 * DESCRIPTION:
 *  System interface for sound, using SDL.
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_LIBSDL_MIXER
#define HAVE_MIXER
#endif
#include <math.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_audio.h"
#include "SDL_mutex.h"
#include "SDL_byteorder.h"
#include "SDL_version.h"
#ifdef HAVE_MIXER
#include "SDL_mixer.h"
#endif

#include "z_zone.h"

#include "m_swap.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "lprintf.h"
#include "s_sound.h"

#include "doomdef.h"
#include "doomstat.h"
#include "doomtype.h"

#include "d_main.h"


#define SAMPLE_FORMAT	AUDIO_S16SYS
#define SAMPLE_ZERO	0
#define SAMPLE_RATE	11025	/* Hz */
#define SAMPLE_CHANNELS	2

#if 0
#define SAMPLE_TYPE	char
#else
#define SAMPLE_TYPE	short
#endif

// Vladimir
// Variables used by Boom from Allegro
// created here to avoid changes to core Boom files
int snd_card = 1;
int mus_card = 1;
int detect_voices = 0; // God knows


/*
 *	SOUND HEADER & DATA
 */

int snd_Channels;
int snd_MaxVolume,		/* maximum volume for sound */
	snd_MusicVolume;	/* maximum volume for music */
boolean snd_MusicAvail,		/* whether music is available */
	snd_SfxAvail;		/* whether sfx are available */

/*
 *	SOUND FX API
 */

typedef struct
{
	unsigned char	*begin;		/* pointers into Sample.firstSample */
	unsigned char	*end;

	SAMPLE_TYPE	*lvol_table;	/* point into vol_lookup */
	SAMPLE_TYPE	*rvol_table;

	unsigned int	pitch_step;
	unsigned int	step_remainder;	/* 0.16 bit remainder of last step. */

	int		pri;
	unsigned int	time;
} Channel;

typedef struct
{
/* Sample data is a lump from a wad: byteswap the a, freq
 * and the length fields before using them		*/
	short		a;		/* always 3	*/
	short		freq;		/* always 11025	*/
	int32_t		length;		/* sample length */
	unsigned char	firstSample;
} Sample;
//COMPILE_TIME_ASSERT(Sample, offsetof(Sample,firstSample) == 8);


#define CHAN_COUNT	8
static Channel	channel[CHAN_COUNT];

#define MAX_VOL		64	/* 64 keeps our table down to 16Kb */
static SAMPLE_TYPE	vol_lookup[MAX_VOL * 256];

static int	steptable[256];		/* Pitch to stepping lookup */

static boolean	snd_initialized;
static int	SAMPLECOUNT = 512;
int	snd_samplerate = SAMPLE_RATE;


void audio_loop (void *unused, Uint8 *stream, int len)
{
	Channel* chan;
	Channel* cend;
	SAMPLE_TYPE *begin;
	SAMPLE_TYPE *end;
	unsigned int sample;
	register int dl;
	register int dr;

	if (!snd_initialized)
		return;
	
	end = (SAMPLE_TYPE *) (stream + len);
	cend = channel + CHAN_COUNT;

	begin = (SAMPLE_TYPE *) stream;
	while (begin < end)
	{
	// Mix all the channels together.
		dl = SAMPLE_ZERO;
		dr = SAMPLE_ZERO;

		chan = channel;
		for ( ; chan < cend; chan++)
		{
			// Check channel, if active.
			if (chan->begin)
			{
				// Get the sample from the channel.
				sample = *chan->begin;

				// Adjust volume accordingly.
				dl += chan->lvol_table[sample];
				dr += chan->rvol_table[sample];

				// Increment sample pointer with pitch adjustment.
				chan->step_remainder += chan->pitch_step;
				chan->begin += chan->step_remainder >> 16;
				chan->step_remainder &= 65535;

				// Check whether we are done.
				if (chan->begin >= chan->end)
				{
					chan->begin = NULL;
				//	printf ("  channel done %d\n", chan);
				}
			}
		}

#if 0	/* SAMPLE_FORMAT */
		if (dl > 127)
			dl = 127;
		else if (dl < -128)
			dl = -128;
		if (dr > 127)
			dr = 127;
		else if (dr < -128)
			dr = -128;
#else
		if (dl > 0x7fff)
			dl = 0x7fff;
		else if (dl < -0x8000)
			dl = -0x8000;
		if (dr > 0x7fff)
			dr = 0x7fff;
		else if (dr < -0x8000)
			dr = -0x8000;
#endif

		*begin++ = dl;
		*begin++ = dr;
	}
}


void I_SetSfxVolume(int volume)
{
}

// Gets lump nums of the named sound.  Returns pointer which will be
// passed to I_StartSound() when you want to start an SFX.  Must be
// sure to pass this to UngetSoundEffect() so that they can be
// freed!

int I_GetSfxLumpNum(sfxinfo_t *sound)
{
	//return W_GetNumForName(sound->lumpname);
    char namebuf[9];
    sprintf(namebuf, "ds%s", sound->name);
    return W_GetNumForName(namebuf);
}


// Id is unused.
// Data is a pointer to a Sample structure.
// Volume ranges from 0 to 127.
// Separation (orientation/stereo) ranges from 0 to 255.  128 is balanced.
// Pitch ranges from 0 to 255.  Normal is 128.
// Priority looks to be unused (always 0).

//int I_StartSound(int id, void *data, int vol, int sep, int pitch, int priority)
int I_StartSound(int id, int _channel, int vol, int sep, int pitch, int priority)
{
	// Relative time order to find oldest sound.
	static unsigned int soundTime = 0;
	int chanId;
	Sample *sample;
	Channel *chan;
	int oldest;
	int i;

  const unsigned char* data;
  int lump;
  size_t len;

    lump = S_sfx[id].lumpnum;

  // We will handle the new SFX.
  // Set pointer to raw data.
  len = W_LumpLength(lump);

  // e6y: Crash with zero-length sounds.
  // Example wad: dakills (http://www.doomworld.com/idgames/index.php?id=2803)
  // The entries DSBSPWLK, DSBSPACT, DSSWTCHN and DSSWTCHX are all zero-length sounds
  if (len<=8) return -1;

  /* Find padded length */
  len -= 8;
  // do the lump caching outside the SDL_LockAudio/SDL_UnlockAudio pair
  // use locking which makes sure the sound data is in a malloced area and
  // not in a memory mapped one
  data = W_LockLumpNum(lump);

	vol += 150 ; // Vladimir: vol boost?

	// Find an empty channel, the oldest playing channel, or default to 0.
	// Currently ignoring priority.

	chanId = 0;
	oldest = soundTime;
	for (i = 0; i < CHAN_COUNT; i++)
	{
		if (! channel[ i ].begin)
		{
			chanId = i;
			break;
		}
		if (channel[ i ].time < oldest)
		{
			chanId = i;
			oldest = channel[ i ].time;
		}
	}

	sample = (Sample *) data;
	chan = &channel[chanId];

	
	I_UpdateSoundParams(chanId + 1, vol, sep, pitch);

	// begin must be set last because the audio thread will access the channel
	// once it is non-zero.  Perhaps this should be protected by a mutex.
	chan->pri = priority;
	chan->time = soundTime;
	chan->end = &sample->firstSample + LONG(sample->length);
	chan->begin = &sample->firstSample;

	soundTime++;

#if 0
	jni_printf ("I_StartSound %d: v:%d s:%d p:%d pri:%d | %d %d %d %d\n",
		id, vol, sep, pitch, priority,
		chanId, chan->pitch_step, SHORT(sample->a), SHORT(sample->freq));
#endif

	return chanId + 1;
}

void I_StopSound(int handle)
{
	handle--;
	handle &= 7;
	channel[handle].begin = NULL;
}

//int I_SoundIsPlaying(int handle)
boolean I_SoundIsPlaying(int handle)
{
	handle--;
	handle &= 7;
	return (channel[ handle ].begin != NULL);
}

boolean I_AnySoundStillPlaying(void)
{
  boolean result = false;
  int i;

  for (i=0; i<CHAN_COUNT; i++)
    result |= channel[i].begin != NULL;

  return result;
}

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch)
{
	int lvol, rvol;
	Channel *chan;

	if (!snd_initialized)
		return;
		
//	SDL_LockAudio();
	
	// Set left/right channel volume based on seperation.
	sep += 1;	// range 1 - 256
	lvol = vol - ((vol * sep * sep) >> 16);	// (256*256);
	sep = sep - 257;
	rvol = vol - ((vol * sep * sep) >> 16);

	// Sanity check, clamp volume.
	if (rvol < 0)
	{
	//	printf ("rvol out of bounds %d, id %d\n", rvol, handle);
		rvol = 0;
	}
	else if (rvol > 127)
	{
	//	printf ("rvol out of bounds %d, id %d\n", rvol, handle);
		rvol = 127;
	}

	if (lvol < 0)
	{
	//	printf ("lvol out of bounds %d, id %d\n", lvol, handle);
		lvol = 0;
	}
	else if (lvol > 127)
	{
	//	printf ("lvol out of bounds %d, id %d\n", lvol, handle);
		lvol = 127;
	}

	// Limit to MAX_VOL (64)
	lvol >>= 1;
	rvol >>= 1;

	handle--;
	handle &= 7;
	chan = &channel[handle];
	chan->pitch_step = steptable[pitch];
	chan->step_remainder = 0;
	chan->lvol_table = &vol_lookup[lvol * 256];
	chan->rvol_table = &vol_lookup[rvol * 256];

	//SDL_UnlockAudio();
}


/*
 *	SOUND STARTUP STUFF
 */

// inits all sound stuff
void I_StartupSound (void)
{
	//SDL_AudioSpec desired, obtained;

	if (snd_initialized)
		return;

	if (M_CheckParm("--nosound") || M_CheckParm("-s") || M_CheckParm("-nosound"))
	{
		jni_printf ( "I_StartupSound: Sound Disabled.\n");
		return;
	}

	jni_printf("I_StartupSound (SDL):\n");

	/* Initialize variables */
	snd_SfxAvail = snd_MusicAvail = false;
	/*
	desired.freq = snd_samplerate;
	desired.format = SAMPLE_FORMAT;
	desired.channels = SAMPLE_CHANNELS;
	*/
	SAMPLECOUNT = 512;
	
	//desired.samples = SAMPLECOUNT*snd_samplerate/11025;
	//desired.callback = audio_loop;

	/* Vladimir
	if (SDL_OpenAudio(&desired, &obtained) < 0)
	{
		jni_printf ("Couldn't open audio with desired format");
		return;
	}*/
	
	snd_initialized = true;
	//SAMPLECOUNT = obtained.samples;
	
	jni_printf("Configured audio device with %d samples/slice\n", SAMPLECOUNT);
	snd_SfxAvail = true;
	
	//SDL_PauseAudio(0);
}

// shuts down all sound stuff
void I_ShutdownSound (void)
{
	if (snd_initialized)
	{
		snd_initialized = false;
		snd_SfxAvail = false;
		snd_MusicAvail = false;
		//SDL_CloseAudio();
	}
}

//void I_SetChannels(int channels)
void I_SetChannels(void)
{
	int v, j;
	int *steptablemid;

	// We always have CHAN_COUNT channels.

	for (j = 0; j < CHAN_COUNT; j++)
	{
		channel[j].begin = NULL;
		channel[j].end   = NULL;
		channel[j].time = 0;
	}

	// This table provides step widths for pitch parameters.
	steptablemid = steptable + 128;
	for (j = -128; j < 128; j++)
	{
		steptablemid[j] = (int) (pow(2.0, (j/64.0)) * 65536.0);
	}

	// Generate the volume lookup tables.
	for (v = 0; v < MAX_VOL; v++)
	{
		for (j = 0; j < 256; j++)
		{
		//	vol_lookup[v*256+j] = 128 + ((v * (j-128)) / (MAX_VOL-1));

		// Turn the unsigned samples into signed samples.

#if 0	/* SAMPLE_FORMAT */
			vol_lookup[v*256+j] = (v * (j-128)) / (MAX_VOL-1);
#else
			vol_lookup[v*256+j] = (v * (j-128) * 256) / (MAX_VOL-1);
#endif
		//	printf ("vol_lookup[%d*256+%d] = %d\n", v, j, vol_lookup[v*256+j]);
		}
	}
}


/*
 *	SONG API
 */
#include "mmus2mid.h"

char* music_tmp = NULL; /* cph - name of music temporary file */

//int I_RegisterSong(void *data)
int I_RegisterSong(const void *data, size_t len)
{
  MIDI *mididata;
  FILE *midfile;

  if ( len < 32 )
    return 0; // the data should at least as big as the MUS header
	
  if ( music_tmp == NULL )
    return 0;
	
  //jni_printf("I_RegisterSong mus data=%p size: %d file:%s", data, len, music_tmp);
	
  midfile = fopen(music_tmp, "wb");
  
  if ( midfile == NULL ) {
    lprintf(LO_ERROR,"Couldn't write MIDI to %s\n", music_tmp);
    return 0;
  }
  
  jni_printf("I_RegisterSong Converting mus to midi...");
  
  /* Convert MUS chunk to MIDI? */
  if ( memcmp(data, "MUS", 3) == 0 )
  {
    UBYTE *mid;
    int midlen;

    mididata = malloc(sizeof(MIDI));
    mmus2mid(data, mididata, 89, 0);
    MIDIToMidi(mididata,&mid,&midlen);
    M_WriteFile(music_tmp,mid,midlen);
    free(mid);
    free_mididata(mididata);
    free(mididata);
  } else {
    fwrite(data, len, 1, midfile);
  }
  fclose(midfile);

  /*
  music[0] = Mix_LoadMUS(music_tmp);
  if ( music[0] == NULL ) {
    lprintf(LO_ERROR,"Couldn't load MIDI from %s: %s\n", music_tmp, Mix_GetError());
  }
*/

	return 0;
}

int I_RegisterExternalSong(const char *name)
{
	return 0;
}

void I_UnRegisterSong(int handle)
{
}

void I_PauseSong(int handle)
{
}

void I_ResumeSong(int handle)
{
}

void I_SetMusicVolume(int volume)
{
}

int I_QrySongPlaying(int handle)
{
	return 0;
}

// Stops a song.  MUST be called before I_UnregisterSong().
void I_StopSong(int handle)
{
	if ( music_tmp != NULL ) {
		jni_printf("I_StopSong");
		jni_stop_music();
	}
}

//void I_PlaySong(int handle, boolean looping)
void I_PlaySong(int handle, int looping)
{
	if ( music_tmp != NULL ) {
		jni_printf("I_PlaySong handle=%d, loop=%d, file=%s", handle, looping, music_tmp);
		jni_start_music(music_tmp, 0);
	}
	else {
		jni_printf("I_PlaySong invalid music file %p", music_tmp);
	}
}

void I_ShutdownMusic(void)
{
  if (music_tmp) {
    unlink(music_tmp);
    lprintf(LO_DEBUG, "I_ShutdownMusic: removing %s", music_tmp);
    free(music_tmp);
	music_tmp = NULL;
  }
}

// cournia - try to load a music file into SDL_Mixer
//           returns true if could not load the file
int I_RegisterMusic( const char* filename, musicinfo_t *song )
{
	jni_printf("I_RegisterMusic No external music for %s", filename);
	song->data = 0;
	song->handle = 0;
	song->lumpnum = 0;
	return 0;

}

void I_InitMusic(void)
{
  if (!music_tmp) {
    music_tmp = strdup(DOOMWADDIR "/tmpmusic.mid");
    {
	  jni_printf("Creating tmp music file %s", music_tmp);
	  
      int fd = mkstemp(music_tmp);
	  
      if (fd<0) {
        lprintf(LO_ERROR, "I_InitMusic: failed to create music temp file %s", music_tmp);
        free(music_tmp); 
		music_tmp = NULL;
		return;
      } 
	  else { 
        close(fd);
	  }
    }
    atexit(I_ShutdownMusic);
  }
}

void I_InitSound(void)
{
	jni_printf("** I_InitSound sample rate %d nomusicparm=%d", snd_samplerate, nomusicparm);
	
	I_StartupSound();
	
	if (!nomusicparm) {
		I_InitMusic();
	}
	else {
		jni_printf("No music inited");
	}
}
