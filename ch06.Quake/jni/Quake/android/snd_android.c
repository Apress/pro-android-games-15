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
#include "quakedef.h"
#include "sound.h"

#define LOG_TAG "QSound"
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static int  snd_inited;

int paint_audio (void *unused, void * stream, int len)
{
	if (!snd_inited) return 0;
	if (shm) {
		shm->buffer = (unsigned char *)stream;
		shm->samplepos += len / (shm->samplebits / 4);
		// Check for samplepos overflow?
		S_PaintChannels (shm->samplepos);
		return len;
	}
	return 0;
}

qboolean SNDDMA_Init(void)
{

	 // most of the wav files are 16 bits, 22050 Hz, mono
	/* Fill the audio DMA information block */
	shm = &sn;

	shm->samplebits = 16;

	//malloc max : 19 MB
	/*shm->speed = 44100;
	shm->channels = 2;
	*/

	// malloc max : 7 MB  => -12 MB !!
	shm->speed = 22050;
	shm->channels = 2;

	LOGD("SNDDMA_Init Speed %d channels %d", shm->speed, shm->channels);
	

	// 2048 (= 100 ms) better for multithreading ?

	shm->samples = 2048 * shm->channels;
	shm->samplepos = 0;
	shm->submission_chunk = 1;
	shm->buffer = NULL;

	snd_inited = 1;

	return true;
}

int SNDDMA_GetDMAPos(void)
{

	return shm->samplepos;

}

void SNDDMA_Shutdown(void)
{
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

