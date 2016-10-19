package com.opengl.shader;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.util.Log;

public class ViewRenderer implements GLSurfaceView.Renderer {

	private static final String TAG = "ViewRenderer";
	
	native static void initialize(int width, int height);
	native static void drawFrame(int ticks);
	native static void setRotationSpeed(int speed);
	
	static {
		System.loadLibrary("icosahedron");
	}
	
	@Override
	public void onDrawFrame(GL10 arg0) {
		//Log.d(TAG, "onDrawFrame");
		int ticks = (int)System.currentTimeMillis();
		
		drawFrame(ticks);
	}

	@Override
	public void onSurfaceChanged(GL10 arg0, int w, int h) {
		Log.d(TAG, "onSurfaceChanged w=" + w + " h=" + h);
		
		initialize(w, h);
	}

	@Override
	public void onSurfaceCreated(GL10 arg0, EGLConfig conf) {
		Log.d(TAG, "onSurfaceCreated " + conf);
		
	}

}
