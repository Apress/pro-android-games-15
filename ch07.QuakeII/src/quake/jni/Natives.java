package quake.jni;


import java.nio.ByteBuffer;


import android.util.Log;


public class Natives 
{
	public static final String TAG = "Natives";

	public static final int EV_KEYDOWN = 0;
	public static final int EV_KEYUP = 1;
	public static final int EV_MOUSE = 2;
	
	private static EventListener listener;
	
	public static interface EventListener 
	{
		void OnInitVideo(int w, int h);
		void OnSwapBuffers();
		void OnSysError(String text);
		void OnSysQuit();
	}
	
	public static void setListener (EventListener l) {
		listener = l;
	}
	
	/**
	 * Main Quake Sub 
	 * @param argv
	 * @return
	 */
	public static native int QuakeMain(String[] argv);

	/**
	 * Render 1 frame
	 * @return
	 */
	public static native int RenderFrame();
	
	/**
	 * Key press
	 * @param key ascii code
	 * @return
	 */
	public static native int keyPress(int key);
	
	/**
	 * Key release
	 * @param key ascii code
	 * @return
	 */
	public static native int keyRelease(int key);

	/**
	 * Mouse move. Coordinates relative to the center of the screen.
	 * @param mouseX YAW from the center of the screen X - width/2
	 * @param mouseY PITCH from the center of the screen Y - height/2
	 * @return
	 */
	public static native int mouseLook(int mouseX, int mouseY);

	/**
	 * Forward movement using mouse coords
	 * @param mouseX
	 * @param mouseY
	 * @return
	 */
	public static native int mouseMove(int mouseX, int mouseY);
	
	
	/**
	 * Used by Q3 style mods to send motion info to the native layer
	 * @param action DOWN=0, UP=1, MOVE=2
	 * @param x
	 * @param y
	 * @param pressure
	 */
	public static native void motionEvent(int action, int x, int y, float pressure);
	
	/**
	 * Native audio painter. The native code will write audio bytes to the buffer.
	 * Mostly PCM 16 stero 22500  (WAV)
	 */
	public static native int PaintAudio( ByteBuffer buf );
	
	
	 /**
	  * Vladimir Send a command
	  * @param command
	  * @return
	  */
	 public static native boolean sendCommand(String command);
	

	
	/**
	 * Quit
	 * @return
	 */
	public static native int SysQuit ();
	
	/**
	 * Send a key event to the native layer
	 * @param type
	 * @param sym
	 */
	public static void sendNativeKeyEvent (int type, int sym) {
		try {
			if ( type == EV_KEYDOWN)
				Natives.keyPress(sym);
			else
				Natives.keyRelease(sym);
		} catch (UnsatisfiedLinkError e) {
			Log.e(TAG, e.toString());
		}
	}
	
	/***********************************************************
	 * C - Callbacks
	 ***********************************************************/
	

	
	/**
	 * Fires on init graphics
	 * @param w width of the image
	 * @param h
	 */
	private static void OnInitVideo(int w, int h) {
		if ( listener != null)
			listener.OnInitVideo(w, h);
	}
	
	
	/**
	 * Fires when the C lib calls exit()
	 * @param message
	 */
	private static void OnSysError(final String message) {
		Log.e(TAG, "Natives::OnSysError " + message );
		
		if ( listener != null)
			listener.OnSysError(message);
		
	}
	
	
	private static void OnSwapBuffers() {
		if ( listener != null)
			listener.OnSwapBuffers();
	}
	
	private static void OnSysQuit() {
		if ( listener != null)
			listener.OnSysQuit();
		
	}
	
}
