package game.engine.controller;

import java.io.ByteArrayInputStream;
import java.util.Properties;

import android.app.Activity;
import android.os.Handler;
import android.util.Log;
import android.view.KeyEvent;

public class Zeemote
	implements 	IController 

{
	static final String TAG =  "Zeemote";
	
	static {
		System.loadLibrary("zeemote");
	}

	// Native subs
	static private native void Initialize();
	
	// this subs blocks execution
	static private native void DiscoverAndConnect();
	static private native void DisconnectAll();

	
	private Activity mActivity;
	
	private Handler mHandler;
	
	private IController.IControllerListener mListener;
	
	// Dpad Arrow states: UP,DOWN, LEFT, RITE
	private boolean[] mArrowState = {false, false, false, false};

	// ARROW CODES: May be SDL or SCAN CODES
	// Default to scan codes
	private int ARROW_UP = 103, ARROW_DOWN = 108;
	private int ARROW_LEFT = 105, ARROW_RIGHT = 106;
	
	// Controller ID (1/2)
	private int mID = 0;
	
	
	/**
	 * Constructor
	 * @param ctx
	 */
	public Zeemote(Activity ctx, int id) { //, eTargetApp target) {
		mActivity = ctx;
		mHandler = new Handler(mActivity.getMainLooper());
		mID = id;
		
		loadArrowCodes();
		
		Initialize();
	}
	
	private void loadArrowCodes() {
		Log.d(TAG, "Zeemote " + mID + " Arrow scan codes[UDLR]=" 
				+ ARROW_UP + "," + ARROW_DOWN + "," + ARROW_LEFT + "," + ARROW_RIGHT);
	}
	
	
	public void setListener(IController.IControllerListener listener) {
		mListener = listener;
	}
	
	public void connect() {
		Log.d(TAG, "Starting connection thread");
		
		new Thread(new Runnable() {
			public void run() {
				DiscoverAndConnect();
			}
		}).start();
	}
	
	public boolean isConnected() {
		return true; //mZeeController != null ? mZeeController.isConnected() : false;
	}
	
	private void sendEvent(boolean pressed, int ascii, int scanCode)  {
		if ( mListener != null) {
			mListener.onButtonEvent(pressed, ascii, scanCode);
		}
	}
	
	/**
	 * JNI: Fires when there is a controller event: key, ....
	 * @param code
	 * @param message
	 */
	private static void OnEvent(int code, String message) {
		//System.out.println("OnEvent code=" + code + " msg=" + message);
		
		if ( message == null) return;
		
		// parse message as props: event=NAME|key1=val1|....
		try {
			Properties payload = new Properties();
			payload.load(new ByteArrayInputStream(message.replaceAll("\\|", "\n").getBytes()));

			String type = payload.getProperty("event");
			
			if ( type == null) {
				Log.e(TAG, "Invalid message:" + message);
				return;
			}
			//Log.d(TAG, "Event:" + payload);
			
			if ( type.equalsIgnoreCase("EV_KEY")) {
				handleButtonEvent(payload);
			}
			else if ( type.equalsIgnoreCase("EV_JOYSTICK")) {
				handleJoyEvent(payload);
			}
			else if ( type.equalsIgnoreCase("EV_BATTERY")) {
				handleBatteryEvent(payload);
			}
			else if ( type.equalsIgnoreCase("EV_STATUS")) {
				handleStatusEvent(payload);
			}
			else if ( type.equalsIgnoreCase("EV_DISCOVER")) {
				handleOtherEvent(payload);
			}
			else {
				Log.e(TAG, "Unhandled event " + type);
			}
		} catch (Exception e) {
			Log.e(TAG, "Zee:OnEvent: " + e);
		}
		
	}
	
	private static void handleButtonEvent(Properties payload){
		String label = payload.getProperty("label");
		int button = Integer.parseInt(payload.getProperty("button"));
		int pressed = Integer.parseInt(payload.getProperty("pressed"));
		
		Log.d(TAG, "Ev Button " + button + " Pressed=" + pressed + " Lbl:" + label);
	}

	private static void handleJoyEvent(Properties payload){
		int scaleX = Integer.parseInt(payload.getProperty("SX"));
		int scaleY = Integer.parseInt(payload.getProperty("SY"));
		
		Log.d(TAG, "Ev Joy X= " + scaleX + " Y=" + scaleY );
	}

	private static void handleBatteryEvent(Properties payload){
		float value = Float.parseFloat(payload.getProperty("value"));
		String units = payload.getProperty("unit");
		
		Log.d(TAG, "Ev Battery= " + value + " " + units );
	}
	
	private static void handleStatusEvent(Properties payload){
		String message = payload.getProperty("message");
		
		Log.d(TAG, "Status " + message);
	}
	
	private static void handleOtherEvent(Properties payload){
		String message = payload.getProperty("message");
		String status = payload.getProperty("status");
		
		Log.d(TAG, "Other Event: Status:" + status + " m: " + message);
	}
	
	/************************************************
	 * ZEEMOTE EVENTS - START
	 ************************************************/

	
	protected void processDpad(int axis, int idx1, int idx2, int dpad1, int dpad2) {
		if ( axis >= 50) {
			// rite/bottom arrow press
			if ( ! mArrowState[idx2]) {
				sendArrowScancode(KeyEvent.ACTION_DOWN, dpad2);
			}
			mArrowState[idx2] = true; 

		}
		else if ( axis <= -50) {
			// left/top arrow press
			if ( ! mArrowState[idx1]) {
				sendArrowScancode(KeyEvent.ACTION_DOWN, dpad1);
			}
			mArrowState[idx1] = true; 
		}
		else {
			if ( mArrowState[idx1] ) {
				// send left arrow release
				sendArrowScancode(KeyEvent.ACTION_UP, dpad1);
			}
			if ( mArrowState[idx2] ) {
				// send rite arrow release
				sendArrowScancode(KeyEvent.ACTION_UP, dpad2);
			}
			mArrowState[idx1] = mArrowState[idx2] = false;
		}
	}
	
	/*
	 * Arrows have fixed scan codes U=103, D=108, 
	 */
	private void sendArrowScancode(int action, int scanCode) {
		if ( action == KeyEvent.ACTION_DOWN) {
			//System.out.println("ARROW PRESS=" + action + " code=" + scanCode);

			// Send down SC
			sendEvent(true, scanCode, scanCode);
		}
		else if ( action == KeyEvent.ACTION_UP) {
			//System.out.println("ARROW RELEASE=" + action + " code=" + scanCode);
			
			// Send UP SC
			sendEvent(false, scanCode, scanCode);
		}
	}

	/************************************************
	 * END ZEEMOTE EVENTS
	 ************************************************/

//	public void connectWithBlueTooth()
//	{
//		try {
//			Log.d(TAG, "Bluetooh enabled. start Zee connect");
//			//zeeConnect(useAsciiArrows, showInfoDlg);
//			connect();
//		} catch (Exception e) {
//			Log.e(TAG, e.toString());
//		}
//	}
	
	/**
	 * Controller Dialogs
	 */
//	public void showConnectDlg() 
//	{
//	}
	
	/**
	 * Connected info dialog
	 */
//	public void showConnectedInfoDlg() {
//	}

	/**
	 * Disconnected Dialog
	 */
//	public void showDisconnectedInfoDlg(String title) {
//	}
	
	
	/**
	 * Zeemote key mappings as a Dialog. Can be invoked when the Native APP is active
	 * @param ctx
	 * @param target Target Application: SNES, ARCADE, etc
	 */
//	public void showBindingsDlg(final IDialogClickListener listener)
//	{
//	}

	@Override
	public void disconnect() {
		DisconnectAll();
	}
	
}
