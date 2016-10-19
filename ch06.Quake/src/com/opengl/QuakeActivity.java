package com.opengl;

import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;

/**
 * Quake launcher
 * Store your game files in /sdcard/quake/base
 * @author Administrator
 *
 */
public class QuakeActivity extends Activity {
  /** Called when the activity is first created. */

  QuakeView mQuakeView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // setContentView(R.layout.main);
    mQuakeView = new QuakeView(this);

    int w = getWindow().getWindowManager().getDefaultDisplay().getWidth();
    int h = getWindow().getWindowManager().getDefaultDisplay().getHeight();
    
    // Put the game file pak0.pak files under /sdcard/quake/base!
    final String baseDir = "/sdcard/quake";
    
    String args = "quake"
        + ",-width," + String.valueOf(w) 
        + ",-height," + String.valueOf(h)
        + ",-basedir," + baseDir
        // use base for retail
        + ",-game,base"// + game 
        + ",+skill,0"
        + ",+showpause,0"
        + ",+crosshair,1"
        + ",+gl_ztrick,0";
    
    mQuakeView.setRenderer(args.split(","));

    setContentView(mQuakeView);

  }
  
  @Override
  public boolean onKeyDown(int keyCode, KeyEvent event) {
    return mQuakeView.keyDown(keyCode, event);
  }
  
  @Override
  public boolean onKeyUp(int keyCode, KeyEvent event) {
    return mQuakeView.keyUp(keyCode, event);
  }
}