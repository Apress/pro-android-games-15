/*
 *	motionplus.h
 *
 *	Written By:
 *		Gabriele Randelli	
 *		Email: < randelli (--AT--) dis [--DOT--] uniroma1 [--DOT--] it >
 *
 *	Copyright 2010
 *
 *	This file is part of wiiC.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	$Header$
 *
 */

/**
 *	@file
 *	@brief Motion Plus expansion device.
 */
#ifndef MOTIONPLUS_H_INCLUDED
#define MOTIONPLUS_H_INCLUDED

#include "wiic_internal.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

int motion_plus_handshake(struct wiimote_t* wm, byte* data, unsigned short len);

void motion_plus_disconnected(struct motion_plus_t* mp);

void motion_plus_event(struct motion_plus_t* mp, byte* msg);

void wiiuse_set_mp_threshold(struct wiimote_t* wm, int threshold);

#ifdef __cplusplus
}
#endif

#endif // MOTIONPLUS_H_INCLUDED
