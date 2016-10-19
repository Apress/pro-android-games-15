package controller;

import android.content.Context;
import android.util.Log;

import com.zeemote.zc.Controller;
import com.zeemote.zc.event.BatteryEvent;
import com.zeemote.zc.event.ButtonEvent;
import com.zeemote.zc.event.ControllerEvent;
import com.zeemote.zc.event.DisconnectEvent;
import com.zeemote.zc.event.IButtonListener;
import com.zeemote.zc.event.IJoystickListener;
import com.zeemote.zc.event.IStatusListener;
import com.zeemote.zc.event.JoystickEvent;
import com.zeemote.zc.ui.android.ControllerAndroidUi;

public class Zeemote implements IStatusListener, IJoystickListener,
    IButtonListener

{
  private static final String TAG = "Zeemote";
  private Controller mZeeController;
  private Context mContext;
  
  public Zeemote(Context context) {
    mContext = context;
    mZeeController = new Controller(Controller.CONTROLLER_1);

    mZeeController.addStatusListener(this);
    mZeeController.addJoystickListener(this);
    mZeeController.addButtonListener(this);

  }

  public void  connect() {
    ControllerAndroidUi controllerUi = new ControllerAndroidUi(mContext,
        mZeeController);
    controllerUi.startConnectionProcess();
  }
  
  /************************************************
   * ZEEMOTE EVENTS
   ************************************************/
  public void batteryUpdate(BatteryEvent event) {
    int id = event.getController().getId();
    int max = event.getMaximumLevel();
    int min = event.getMinimumLevel();
    int warn = event.getWarningLevel();
    int cur = event.getCurrentLevel();
    int pctLeft = (int) (((float) (cur - min) / (float) (max - min)) * 100);

    Log.d(TAG, "Battery Update: Controller ID=" + id + " cur=" + cur + ", max="
        + max + ", min=" + min + ", warn=" + warn + " %left=" + pctLeft);

    /* battery low? */
    if (cur <= warn) {
      // do somthing
    }
  }

  public void connected(ControllerEvent event) {
    com.zeemote.zc.Configuration config = event.getController()
        .getConfiguration();

    Log.d(TAG, "Connected to controller:");
    Log.d(TAG, "Num Buttons=" + config.getButtonCount());
    Log.d(TAG, "Num Joysticks=" + config.getJoystickCount());

  }

  public void disconnected(DisconnectEvent event) {
    Log.d(TAG, "Disconnected from controller: "
        + (event.isUnexpected() ? "unexpected" : "expected"));

    if (mZeeController != null) {
      Log.d(TAG, "Removing Zee listeners.");

      mZeeController.removeStatusListener(this);
      mZeeController.removeJoystickListener(this);
      mZeeController.removeButtonListener(this);
    }
  }

  /************************************************
   * ZEEMOTE BUTTON EVENTS
   ************************************************/
  public void buttonPressed(ButtonEvent event) {
    int b = event.getButtonID();
    String label = event.getController().getConfiguration().getButtonLabel(b);

  }

  public void buttonReleased(ButtonEvent event) {
    String buttonName = event.getController().getConfiguration().getButtonLabel(event.getButtonID());
  }

  /************************************************
   * ZEEMOTE JOYSTIC EVEN
   ************************************************/
  public void joystickMoved(JoystickEvent e) {
    // A joystick moved. Scale the values between -100 and 100
    int x = e.getScaledX(-100, 100);
    int y = e.getScaledY(-100, 100);

    Log.d(TAG, "X=" + x + ",Y=" + y);
  }
}
