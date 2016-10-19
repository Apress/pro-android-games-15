/*
 *    io.c
 *
 *	  This file is part of WiiC, written by:
 *		Gabriele Randelli
 *		Email: randelli@dis.uniroma1.it
 *
 *    Copyright 2010
 *		
 *	  This file is based on Wiiuse, written By:
 *		Michael Laforest	< para >
 *		Email: < thepara (--AT--) g m a i l [--DOT--] com >
 *
 *	  Copyright 2006-2007
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	  $Header$
 */

/**
 *	@file
 *	@brief Handles device I/O (non-OS specific).
 */

#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"
#include "wiic_internal.h"
#include "io.h"


 /**
 *	@brief Get initialization data from the wiimote.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param data		unused
 *	@param len		unused
 *
 *	When first called for a wiimote_t structure, a request
 *	is sent to the wiimote for initialization information.
 *	This includes factory set accelerometer data.
 *	The handshake will be concluded when the wiimote responds
 *	with this data.
 */
void wiic_handshake(struct wiimote_t* wm, byte* data, unsigned short len) {
	if (!wm)	return;

	switch (wm->handshake_state) {
		case 0:
		{
			/* send request to wiimote for accelerometer calibration */
			byte* buf;

			WIIMOTE_ENABLE_STATE(wm, WIIMOTE_STATE_HANDSHAKE);
			wiic_set_leds(wm, WIIMOTE_LED_NONE);

			buf = (byte*)malloc(sizeof(byte) * 8);
			wiic_read_data_cb(wm, wiic_handshake, buf, WM_MEM_OFFSET_CALIBRATION, 7);
			wm->handshake_state++;

			wiic_set_leds(wm, WIIMOTE_LED_NONE);

			break;
		}
		case 1:
		{
			struct read_req_t* req = wm->read_req;
			struct accel_t* accel = &wm->accel_calib;

			/* received read data */
			accel->cal_zero.x = req->buf[0];
			accel->cal_zero.y = req->buf[1];
			accel->cal_zero.z = req->buf[2];

			accel->cal_g.x = req->buf[4] - accel->cal_zero.x;
			accel->cal_g.y = req->buf[5] - accel->cal_zero.y;
			accel->cal_g.z = req->buf[6] - accel->cal_zero.z;

			/* done with the buffer */
			free(req->buf);

			/* handshake is done */
			WIIC_DEBUG("Handshake finished. Calibration: Idle: X=%x Y=%x Z=%x\t+1g: X=%x Y=%x Z=%x",
					accel->cal_zero.x, accel->cal_zero.y, accel->cal_zero.z,
					accel->cal_g.x, accel->cal_g.y, accel->cal_g.z);


			/* request the status of the wiimote to see if there is an expansion */
			wiic_status(wm);

			WIIMOTE_DISABLE_STATE(wm, WIIMOTE_STATE_HANDSHAKE);
			WIIMOTE_ENABLE_STATE(wm, WIIMOTE_STATE_HANDSHAKE_COMPLETE);
			wm->handshake_state++;

			/* now enable IR if it was set before the handshake completed */
			if (WIIMOTE_IS_SET(wm, WIIMOTE_STATE_IR)) {
				WIIC_DEBUG("Handshake finished, enabling IR.");
				WIIMOTE_DISABLE_STATE(wm, WIIMOTE_STATE_IR);
				wiic_set_ir(wm, 1);
			}

			break;
		}
		default:
		{
			break;
		}
	}
}
