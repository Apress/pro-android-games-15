package game.engine.controller;


public interface IController 
{
	public static final int MAX_CONTROLLERS = 2;
	
	/**
	 * Used to handle controller events
	 * @author New Owner
	 *
	 */
	public static interface IControllerListener 
	{
		public void onButtonEvent( boolean pressed, int ascii, int scanCode);
		public void connected();
		public void disconnected(String reason);
		public void onMessage(String message);
		public void onJoystickEvent(float angleDeg, float distance);
	}

	/**
	 * Used to handle actions when a controller dialog is clicked (pause)
	 * @author New Owner
	 *
	 */
	public static interface IDialogClickListener 
	{
		public void onDialogDismissed( );
	}
	
	/**
	 * Connect to the controller
	 */
	public void connect() ;
	
	/**
	 * Disconnect
	 */
	public void disconnect() ;
	
//	public void connectWithBlueTooth();
	
	/**
	 * Listen for controller events
	 */
	public void setListener(IController.IControllerListener listener);
	
	public boolean isConnected();
	
	// Dialogs
//	public void showConnectDlg(); 
//	
//	public void showConnectedInfoDlg(); 
//	
//	public void showDisconnectedInfoDlg(String title);
//	
//	public void showBindingsDlg(final IDialogClickListener listener);  
}
