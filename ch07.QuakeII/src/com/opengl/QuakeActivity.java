package com.opengl;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;

/**
 * Quake launcher
 * Store your game files in /sdcard/quake/base
 * @author Administrator
 *
 */
public class QuakeActivity extends Activity {
  /** Called when the activity is first created. */
  static final String TAG = "QuakeActivity";
  
  QuakeView mQuakeView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // setContentView(R.layout.main);
    mQuakeView = new QuakeView(this);

    
    int w = getWindow().getWindowManager().getDefaultDisplay().getWidth();
    int h = getWindow().getWindowManager().getDefaultDisplay().getHeight();

    Log.d(TAG, "Display Size:" + w + "," + h );

    // Put the game file(s) pak0.pak under /sdcard/quake2/baseq2
    final String baseDir = "/sdcard/quake2";
    
    // Deafult 569x320
    String gl_mode = "3"; 
    
    if ( w == 800 && h == 480 ) gl_mode = "4";
    
    String args = "quake2,"
        + "+set,basedir," +  baseDir + ","
        + "+set,skill,0,"
        + "+set,nocdaudio,1,"
        + "+set,cd_nocd,1,"
        + "+set,s_initsound,1,"
        // Open GL
        + "+set,vid_ref,glx,"
        + "+set,gl_mode," +  gl_mode;
    
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