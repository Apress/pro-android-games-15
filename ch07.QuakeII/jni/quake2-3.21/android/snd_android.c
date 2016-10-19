/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include <stdio.h>

//#include "../game/q_shared.h"
//#include "../client/snd_local.h"
#include "../client/client.h"
#include "../client/snd_loc.h"


#define LOG_TAG "QSound"
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static int  snd_inited;

/* The audio callback. All the magic happens here. */
static int dmapos = 0;
static int dmasize = 0;

/*
===============
SNDDMA_AudioCallback
===============
*/
void SNDDMA_AudioCallback(void *userdata, void *stream, int len)
{
	int pos = (dmapos * (dma.samplebits/8));
	if (pos >= dmasize)
		dmapos = pos = 0;

	if (!snd_inited)  /* shouldn't happen, but just in case... */
	{
		memset(stream, '\0', len);
		return;
	}
	else
	{
		int tobufend = dmasize - pos;  /* bytes to buffer's end. */
		int len1 = len;
		int len2 = 0;

		if (len1 > tobufend)
		{
			len1 = tobufend;
			len2 = len - len1;
		}
		memcpy(stream, dma.buffer + pos, len1);
		if (len2 <= 0)
			dmapos += (len1 / (dma.samplebits/8));
		else  /* wraparound? */
		{
			memcpy(stream+len1, dma.buffer, len2);
			dmapos = (len2 / (dma.samplebits/8));
		}
	}

	if (dmapos >= dmasize)
		dmapos = 0;
}

// SNDDMA_AudioCallback is a better implementaion
int paint_audio (void *unused, void * stream, int len)
{
	if (!snd_inited) return 0;
	//if (dma) {
		dma.buffer = (unsigned char *)stream;
		dma.samplepos += len / (dma.samplebits / 4);
		// Check for samplepos overflow?
		S_PaintChannels (dma.samplepos);
		return len;
	//}
	return 0;
}


qboolean SNDDMA_Init(void)
{

	 // most of the wav files are 16 bits, 22050 Hz, mono
	/* Fill the audio DMA information block */
	//shm = &sn;

	dma.samplebits = 16;


	// malloc max : 7 MB  => -12 MB !!
	dma.speed = 22050;
	dma.channels = 2;

	LOGD("SNDDMA_Init Speed %d channels %d", dma.speed, dma.channels);
	
	dmapos = 0;
	
	// 2048 (= 100 ms) better for multithreading ?

	dma.samples = 32768; //2048 * dma.channels;
	dma.samplepos = 0;
	dma.submission_chunk = 1;

	dmasize = (dma.samples * (dma.samplebits/8));
	dma.buffer = calloc(1, dmasize);
	
	//dma.buffer = NULL;

	snd_inited = 1;

	return 1;
}

int SNDDMA_GetDMAPos(void)
{
	//return dma.samplepos;
	return dmapos;
}

void SNDDMA_Shutdown(void)
{
	free(dma.buffer);
	dma.buffer = NULL;
	dmapos = dmasize = 0;
	snd_inited = 0;
	Com_Printf("SDL audio device shut down.\n");

	if (snd_inited)
	{
		snd_inited = 0;
	}
}

/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
===============
*/
void SNDDMA_Submit(void)
{
}

void SNDDMA_BeginPainting( void ) {
}

