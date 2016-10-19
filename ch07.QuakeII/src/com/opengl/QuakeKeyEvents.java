package com.opengl;

import quake.jni.Natives;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;
import android.view.KeyEvent;

public class QuakeKeyEvents {
	private static final String TAG = "QKeys";
	
	// copied from Quake keys.h
	// these are the key numbers that should be passed to Key_Event
	//
	public static final int K_TAB = 9;
	public static final int K_ENTER = 13;
	public static final int K_ESCAPE = 27;
	public static final int K_SPACE = 32;

	// normal keys should be passed as lowercased ascii

	public static final int K_BACKSPACE = 127;
	public static final int K_UPARROW = 128;
	public static final int K_DOWNARROW = 129;
	public static final int K_LEFTARROW = 130;
	public static final int K_RIGHTARROW = 131;

	public static final int K_ALT = 132;
	public static final int K_CTRL = 133;
	public static final int K_SHIFT = 134;
	public static final int K_F1 = 135;
	public static final int K_F2 = 136;
	public static final int K_F3 = 137;
	public static final int K_F4 = 138;
	public static final int K_F5 = 139;
	public static final int K_F6 = 140;
	public static final int K_F7 = 141;
	public static final int K_F8 = 142;
	public static final int K_F9 = 143;
	public static final int K_F10 = 144;
	public static final int K_F11 = 145;
	public static final int K_F12 = 146;
	public static final int K_INS = 147;
	public static final int K_DEL = 148;
	public static final int K_PGDN = 149;
	public static final int K_PGUP = 150;
	public static final int K_HOME = 151;
	public static final int K_END = 152;

	public static final int K_PAUSE = 255;

	public static boolean mShiftKeyPressed  = false;
	public static boolean mAltKeyPressed = false;

    private static final int[] sKeyCodeToQuakeCode = {
        '$', K_ESCAPE, '$', '$',  K_ESCAPE, K_CTRL, '$', '0', //  0.. 7
        '1', '2', '3', '4',  '5', '6', '7', '8', //  8..15
        '9', '$', '$', K_UPARROW,  K_DOWNARROW, K_LEFTARROW, K_RIGHTARROW, K_ENTER, // 16..23
        '$', '$', '$', K_HOME,  '$', 'a', 'b', 'c', // 24..31

        'd', 'e', 'f', 'g',  'h', 'i', 'j', 'k', // 32..39
        'l', 'm', 'n', 'o',  'p', 'q', 'r', 's', // 40..47
        't', 'u', 'v', 'w',  'x', 'y', 'z', ',', // 48..55
        '.', K_ALT, K_ALT, K_SHIFT,  K_SHIFT, K_TAB, ' ', '$', // 56..63
        '$', '$', K_ENTER, K_BACKSPACE, '`', '-',  '=', '[', // 64..71
        ']', '\\', ';', '\'', '/', K_CTRL,  '#', '$', // 72..79
        K_CTRL, '$', K_ESCAPE, '$',  K_SPACE     // 80..84
    };

    private static final int sKeyCodeToQuakeCodeShift[] =
    {
        0, 0, 0, 0,  0, 0, 0, ')', //  0.. 7
        '!', '@', '#', '$',  '%', '^', '&', '*', //  8..15
        '(', 0, 0, 0,  0, 0, 0, 0, // 16..23
        0, 0, 0, 0,  0, 0, ']', 0, // 24..31

        '\\', '_', '{', '}',  ':', '-', ';', '"', // 32..39
        '\'', '>', '<', '+',  '=', 0, 0, '|', // 40..47
        0, 0, '[', '`',  0, 0, K_PAUSE, ';', // 48..55
        0, 0, 0, 0,  0, 0, 0, 0, // 56..63
        0, 0, 0, 0,  0, 0, 0, 0, // 64..71
        0, 0, '?', '0',  0, K_CTRL, 0, 0, // 72..79
        0, 0, 0, 0,  0                       // 80..84
    };

    private static final int sKeyCodeToQuakeCodeAlt[] =
    {
        0, 0, 0, 0,  0, 0, 0, K_F10, //  0.. 7
        K_F1, K_F2, K_F3, K_F4,  K_F5, K_F6, K_F7, K_F8, //  8..15
        K_F9, 0, 0, 0,  0, 0, 0, 0, // 16..23
        0, 0, 0, 0,  0, 0, '+', '_', // 24..31

        '#', '3', '$', '%',  '=', '8', '&', '*', // 32..39
        '(', '\'', '"', '9',  '0', '1', '4', '!', // 40..47
        '5', '7', '-', '2',  '>', '6', '<', ';', // 48..55 K_F11 K_F12
        ':', 0, 0, 0,  0, 0, 0, 0, // 56..63
        0, 0, 0, 0,  0, 0, 0, 0, // 64..71
        0, 0, ')', 0,  0, '~', 0, 0, // 72..79
        0, 0, 0, 0,  0           // 80..83
    };

	public static boolean weWantThisKeyCode(int keyCode) {
		return (keyCode != KeyEvent.KEYCODE_VOLUME_UP)
				&& (keyCode != KeyEvent.KEYCODE_VOLUME_DOWN)
				&& (keyCode != KeyEvent.KEYCODE_MENU)
				&& (keyCode != KeyEvent.KEYCODE_BACK);
	}
	
	public static boolean weWantVolumeKeys(int keyCode) {
		return (keyCode != KeyEvent.KEYCODE_VOLUME_UP)
				&& (keyCode != KeyEvent.KEYCODE_VOLUME_DOWN);
	}


	/**
	 * Key Down handler: Convert an android key to ASCII & send it to the native engine
	 * @param keyCode
	 * @param event
	 * @return
	 */
	public static boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_ALT_RIGHT:
		case KeyEvent.KEYCODE_ALT_LEFT:
			mAltKeyPressed = true;
			break;
		case KeyEvent.KEYCODE_SHIFT_RIGHT:
		case KeyEvent.KEYCODE_SHIFT_LEFT:
			mShiftKeyPressed = true;
			break;
		}

		try {
			Natives.keyPress(keyCodeToQuakeCode(keyCode));
		} catch (UnsatisfiedLinkError e) {
			Log.d(TAG, e.toString());
		}
		
		return true;
	}

	/**
	 * Key up handler: Convert an android key to ASCII & send it to the native engine
	 * @param keyCode Android key
	 * @param event
	 * @return
	 */
	public static boolean onKeyUp(int keyCode, KeyEvent event) {
		//System.out.println(" *****key code=" + keyCode + " ASCII=" + keyCodeToQuakeCode(keyCode));

		switch (keyCode) {
		case KeyEvent.KEYCODE_ALT_RIGHT:
		case KeyEvent.KEYCODE_ALT_LEFT:
			mAltKeyPressed = false;
			break;
		case KeyEvent.KEYCODE_SHIFT_RIGHT:
		case KeyEvent.KEYCODE_SHIFT_LEFT:
			mShiftKeyPressed = false;
			break;
		}
		
		try {
			Natives.keyRelease(keyCodeToQuakeCode(keyCode));
		} catch (UnsatisfiedLinkError e) {
			Log.d(TAG, e.toString());
		}

		return true;
	}

	/**
	 * Translate keys from Android to ASCII
	 * @param keyCode Android key code
	 * @return ASCII symbol
	 */
	public static int keyCodeToQuakeCode(int keyCode) {
		int key = 0;
		if (keyCode >= sKeyCodeToQuakeCode.length) {
			return 0;
		}
		if (mAltKeyPressed) {
			key = sKeyCodeToQuakeCodeAlt[keyCode];
			if (key == 0) {
				key = sKeyCodeToQuakeCodeShift[keyCode];
				if (key == 0) {
					key = sKeyCodeToQuakeCode[keyCode];
				}
			}
		} else if (mShiftKeyPressed) {
			key = sKeyCodeToQuakeCodeShift[keyCode];
			if (key == 0) {
				key = sKeyCodeToQuakeCode[keyCode];
			}
		} else {
			key = sKeyCodeToQuakeCode[keyCode];
		}
		if (key == 0) {
			key = '$';
		}
		return key;
	}

	/**
	 * 
	 * @param ascii
	 * @return
	 * @deprecated Use androKeyEventToEngineCmd
	 */
	public static String Ascii2String( int ascii) {
		if ( ascii < 127)
			return new String( new byte[]{ (byte)ascii});
		else
			return "" + ascii ;
	}

}
