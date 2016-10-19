package com.opengl;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import quake.audio.NativeAudio;
import quake.jni.Natives;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class QuakeView extends GLSurfaceView implements Natives.EventListener {
  private static final String TAG = "QuakeView";

  {
    // Load native lib
    System.loadLibrary("quake2");
  }

  boolean mGameLoaded = false;

  // private Context mContext;
  private String[] mArgs;

  public QuakeView(Context context) {
    super(context);
    init(context);
  }

  public QuakeView(Context context, AttributeSet attrs) {
    super(context, attrs);
    init(context);
  }

  private void init(Context context) {
    // We want events.
    setFocusable(true);
    setFocusableInTouchMode(true);
    requestFocus();

    Log.d(TAG, "QuakeView init");

    // Listen for JNI events
    Natives.setListener(this);
  }

  public void setRenderer(String[] args) {
    mArgs = args;

    Log.d(TAG, "Setting startup args & renderer");

    setRenderer(new QuakeRenderer());
  }

  /**
   * Key events
   * 
   * @param keyCode
   * @param event
   * @return
   */
  public boolean keyDown(final int keyCode, final KeyEvent event) {
    queueEvent(new Runnable() {
      public void run() {
        QuakeKeyEvents.onKeyDown(keyCode, event);
      }
    });

    return true;
  }

  public boolean keyUp(final int keyCode, final KeyEvent event) {

    queueEvent(new Runnable() {
      public void run() {
        QuakeKeyEvents.onKeyUp(keyCode, event);
      }
    });
    return true;
  }

  private float moveX = 0f;
  private float moveY = 0f;
  
  public boolean onTouchEvent(final MotionEvent e) {
    final int action = e.getAction();

    queueEvent(new Runnable() {
      public void run() {
        if (action == MotionEvent.ACTION_DOWN) {
          // QuakeKeyEvents.onKeyDown(KeyEvent.KEYCODE_ENTER, null);
          moveX = e.getX();
          moveY = e.getY();
        } 
        else if (action == MotionEvent.ACTION_UP) {
          moveX = moveY = 0;
        } 
        else if (action == MotionEvent.ACTION_MOVE) {
          final float dx = e.getX() - moveX;
          final float dy = e.getY() - moveY;

          final float DX = Math.abs(dx);
          final float DY = Math.abs(dy);

          if (DX < 30 && DY < 30) {
            return;
          }
          //Natives.mouseLook((int) (dx * 1.5), (int) dy);
          Natives.mouseMove((int) dx, (int) dy);
        }
      }
    });
    return true;
  }

  /**
   * Look around
   * 
   * @param deltaX
   * @param deltaY
   */
  public void mouseLook(final int deltaX, final int deltaY) {
    queueEvent(new Runnable() {
      public void run() {
        Natives.mouseLook(deltaX, deltaY);
      }
    });
  }

  /**
   * Mouse movement
   * 
   * @param deltaX
   * @param deltaY
   */
  public void mouseMove(final int deltaX, final int deltaY) {
    queueEvent(new Runnable() {
      public void run() {
        Natives.mouseMove(deltaX, deltaY);
      }
    });
  }

  /**
   * Renderer
   * 
   * @author Administrator
   * 
   */
  public class QuakeRenderer implements GLSurfaceView.Renderer {

    @Override
    public void onDrawFrame(GL10 arg0) {
      if (mGameLoaded) {
        Natives.RenderFrame();
      }

    }

    @Override
    public void onSurfaceChanged(GL10 arg0, int width, int height) {
      Log.d(TAG, "onSurfaceChanged w=" + width + " h=" + height);

    }

    @Override
    public void onSurfaceCreated(GL10 arg0, EGLConfig arg1) {
      Log.d(TAG, "onSurfaceCreated");

      if (mArgs != null) {
        mGameLoaded = true;
        Natives.QuakeMain(mArgs);
      }

    }

  }

  /**
   * Native Callbacks
   */
  @Override
  public void OnInitVideo(int w, int h) {
    Log.d(TAG, "OnInitVideo. " + w + "x" + h + " Starting native audio.");

    // Native audio
    NativeAudio.start();

  }

  @Override
  public void OnSwapBuffers() {
  }

  @Override
  public void OnSysError(String text) {
    Log.e(TAG, text);

    // Abort!
    System.exit(0);
  }

  @Override
  public void OnSysQuit() {
  }

}
