#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiic.h>

#include <android/log.h>
#include "wiijni.h"

#define LOG_TAG "WIIHandler"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__)

#define printf LOGD

extern int sig_exit;

static void jni_send_btn_press(int id, const char * btn)
{
	jni_send_event(id, "EVT_TYPE=BTNPRESS|BTN=%s", btn);
}

static void jni_send_btn_release(int id, const char * btn)
{
	jni_send_event(id, "EVT_TYPE=BTNRELEASE|BTN=%s", btn);
}

/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *
 *	This function is called automatically by the wiic library when an
 *	event occurs on the specified wiimote.
 */
void handle_event(struct wiimote_t* wm) {
	int id = wm->unid;
	//printf("\n\n--- EVENT [id %i] ---\n", wm->unid);

	/* if a button is pressed, report it */
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))		jni_send_btn_press(id,"A");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))		jni_send_btn_press(id,"B");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))		jni_send_btn_press(id,"UP");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))	jni_send_btn_press(id,"DOWN");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))	jni_send_btn_press(id,"LEFT");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))	jni_send_btn_press(id,"RIGHT");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))	jni_send_btn_press(id,"MINUS");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))	jni_send_btn_press(id,"PLUS");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))		jni_send_btn_press(id,"ONE");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO))		jni_send_btn_press(id,"TWO");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))	jni_send_btn_press(id,"HOME");

	if (IS_RELEASED(wm, WIIMOTE_BUTTON_A))		jni_send_btn_release(id,"A");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_B))		jni_send_btn_release(id,"B");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_UP))		jni_send_btn_release(id,"UP");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_DOWN))	jni_send_btn_release(id,"DOWN");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_LEFT))	jni_send_btn_release(id,"LEFT");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_RIGHT))	jni_send_btn_release(id,"RIGHT");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_MINUS))	jni_send_btn_release(id,"MINUS");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_PLUS))	jni_send_btn_release(id,"PLUS");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_ONE))	jni_send_btn_release(id,"ONE");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_TWO))	jni_send_btn_release(id,"TWO");
	if (IS_RELEASED(wm, WIIMOTE_BUTTON_HOME))	jni_send_btn_release(id,"HOME");
	
	/*
	 *	Pressing minus will tell the wiimote we are no longer interested in movement.
	 *	This is useful because it saves battery power.
	 */
/*	 
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS))
		wiic_motion_sensing(wm, 0);
*/

	/*
	 *	Pressing plus will tell the wiimote we are interested in movement.
	 */
/*	 
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS))
		wiic_motion_sensing(wm, 1);
		
	if(IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_LEFT))
		wiic_set_motion_plus(wm,0);
	if(IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))
		wiic_set_motion_plus(wm,1); */
	/*
	 *	Pressing B will toggle the rumble
	 *
	 *	if B is pressed but is not held, toggle the rumble
	 */
/*	 
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B))
		wiic_toggle_rumble(wm);

	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_UP))
		wiic_set_ir(wm, 1);
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_DOWN))
		wiic_set_ir(wm, 0);
*/

	/* if the accelerometer is turned on then print angles */
	
	if (WIIC_USING_ACC(wm)) {
		printf("wiimote roll  = %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
		printf("wiimote pitch = %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
		printf("wiimote yaw   = %f\n", wm->orient.yaw);
	}

	/* if the Motion Plus is turned on then print angle rates */
/*	
	if (WIIC_USING_MOTION_PLUS(wm)) {
		printf("motion plus roll rate = %f\n", wm->exp.mp.angle_rate_gyro.r);
		printf("motion plus pitch rate = %f\n", wm->exp.mp.angle_rate_gyro.p);
		printf("motion plus yaw rate = %f\n", wm->exp.mp.angle_rate_gyro.y);
	}
*/
	/* show events specific to supported expansions */
	
	if (wm->exp.type == EXP_NUNCHUK) {
		// nunchuk 
		struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;

		if ( nc->btns != 0 || nc->btns_released != 0 ) {
			int Z_TR01 = (nc->btns == 3) || (nc->btns_released == 3); /* Old Z=1, New(tr01) Z=(3)ALL */
			/*
			printf("nunchuk buttons pressed  = %d released = %d ZTR01 = %d\n", nc->btns, nc->btns_released, Z_TR01);
			*/
			if (IS_JUST_PRESSED(nc, NUNCHUK_BUTTON_C) && !Z_TR01)	jni_send_btn_press(id,"C");
			if (IS_JUST_PRESSED(nc, NUNCHUK_BUTTON_Z))				jni_send_btn_press(id,"Z");

			if (IS_RELEASED(nc, NUNCHUK_BUTTON_C) && !Z_TR01)	jni_send_btn_release(id,"C");
			if (IS_RELEASED(nc, NUNCHUK_BUTTON_Z))				jni_send_btn_release(id,"Z");
		}
/*
		printf("nunchuk roll  = %f\n", nc->orient.roll);
		printf("nunchuk pitch = %f\n", nc->orient.pitch);
		printf("nunchuk yaw   = %f\n", nc->orient.yaw);

		printf("nunchuk joystick angle:     %f\n", nc->js.ang);
		printf("nunchuk joystick magnitude: %f\n", nc->js.mag);
		*/
		jni_send_event(id, "EVT_TYPE=NUNCHUK|ROLL=%.2f|PITCH=%.2f|YAW=%.2f|JANGLE=%.2f|JMAGNITUDE=%.2f"
			, nc->orient.roll, nc->orient.pitch, nc->orient.yaw, nc->js.ang, nc->js.mag);
	} 
	/* else if (wm->exp.type == EXP_CLASSIC) {
		// classic controller 
		struct classic_ctrl_t* cc = (classic_ctrl_t*)&wm->exp.classic;

		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZL))			printf("Classic: ZL pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_B))			printf("Classic: B pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_Y))			printf("Classic: Y pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_A))			printf("Classic: A pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_X))			printf("Classic: X pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZR))			printf("Classic: ZR pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_LEFT))		printf("Classic: LEFT pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_UP))			printf("Classic: UP pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_RIGHT))		printf("Classic: RIGHT pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_DOWN))		printf("Classic: DOWN pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_L))		printf("Classic: FULL L pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_MINUS))		printf("Classic: MINUS pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_HOME))		printf("Classic: HOME pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_PLUS))		printf("Classic: PLUS pressed\n");
		if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_R))		printf("Classic: FULL R pressed\n");

		printf("classic L button pressed:         %f\n", cc->l_shoulder);
		printf("classic R button pressed:         %f\n", cc->r_shoulder);
		printf("classic left joystick angle:      %f\n", cc->ljs.ang);
		printf("classic left joystick magnitude:  %f\n", cc->ljs.mag);
		printf("classic right joystick angle:     %f\n", cc->rjs.ang);
		printf("classic right joystick magnitude: %f\n", cc->rjs.mag);
	} else if (wm->exp.type == EXP_GUITAR_HERO_3) {
		// guitar hero 3 guitar 
		struct guitar_hero_3_t* gh3 = (guitar_hero_3_t*)&wm->exp.gh3;

		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_STRUM_UP))		printf("Guitar: Strum Up pressed\n");
		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_STRUM_DOWN))	printf("Guitar: Strum Down pressed\n");
		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_YELLOW))		printf("Guitar: Yellow pressed\n");
		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_GREEN))		printf("Guitar: Green pressed\n");
		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_BLUE))			printf("Guitar: Blue pressed\n");
		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_RED))			printf("Guitar: Red pressed\n");
		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_ORANGE))		printf("Guitar: Orange pressed\n");
		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_PLUS))			printf("Guitar: Plus pressed\n");
		if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_MINUS))		printf("Guitar: Minus pressed\n");

		printf("Guitar whammy bar:          %f\n", gh3->whammy_bar);
		printf("Guitar joystick angle:      %f\n", gh3->js.ang);
		printf("Guitar joystick magnitude:  %f\n", gh3->js.mag);
	} else if(wm->exp.type == EXP_BALANCE_BOARD) {
		struct balance_board_t* bb = (balance_board_t*)&wm->exp.bb;
		
		printf("balance board top left weight: %f\n", bb->pressure_weight.top_left);
		printf("balance board top right weight: %f\n", bb->pressure_weight.top_right);
		printf("balance board bottom left weight: %f\n", bb->pressure_weight.bottom_left);
		printf("balance board bottom right weight: %f\n", bb->pressure_weight.bottom_right);
		printf("balance board total weight: %f\n", bb->pressure_weight.weight);
	}
	*/
}


/**
 *	@brief Callback that handles a read event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param data		Pointer to the filled data block.
 *	@param len		Length in bytes of the data block.
 *
 *	This function is called automatically by the wiic library when
 *	the wiimote has returned the full data requested by a previous
 *	call to wiic_read_data().
 *
 *	You can read data on the wiimote, such as Mii data, if
 *	you know the offset address and the length.
 *
 *	The \a data pointer was specified on the call to wiic_read_data().
 *	At the time of this function being called, it is not safe to deallocate
 *	this buffer.
 */
void handle_read(struct wiimote_t* wm, byte* data, unsigned short len) {
/*
	int i = 0;

	printf("\n\n--- DATA READ [wiimote id %i] ---\n", wm->unid);
	printf("finished read of size %i\n", len);
	for (; i < len; ++i) {
		if (!(i%16))
			printf("\n");
		printf("%x ", data[i]);
	}
	printf("\n\n"); */
}


/**
 *	@brief Callback that handles a controller status event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *	@param attachment		Is there an attachment? (1 for yes, 0 for no)
 *	@param speaker			Is the speaker enabled? (1 for yes, 0 for no)
 *	@param ir				Is the IR support enabled? (1 for yes, 0 for no)
 *	@param led				What LEDs are lit.
 *	@param battery_level	Battery level, between 0.0 (0%) and 1.0 (100%).
 *
 *	This occurs when either the controller status changed
 *	or the controller status was requested explicitly by
 *	wiic_status().
 *
 *	One reason the status can change is if the nunchuk was
 *	inserted or removed from the expansion port.
 */
void handle_ctrl_status(struct wiimote_t* wm) {
/*
	printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

	printf("attachment:      %i\n", wm->exp.type);
	printf("speaker:         %i\n", WIIC_USING_SPEAKER(wm));
	printf("ir:              %i\n", WIIC_USING_IR(wm));
	printf("leds:            %i %i %i %i\n", WIIC_IS_LED_SET(wm, 1), WIIC_IS_LED_SET(wm, 2), WIIC_IS_LED_SET(wm, 3), WIIC_IS_LED_SET(wm, 4));
	printf("battery:         %f %%\n", wm->battery_level);
	*/
}


/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *
 *	This can happen if the POWER button is pressed, or
 *	if the connection is interrupted.
 */
void handle_disconnect(wiimote* wm) {
	//printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
	jni_send_error(120, "DISCONNECTED: [wiimote id %i]", wm->unid);
	sig_exit = 1;
}
