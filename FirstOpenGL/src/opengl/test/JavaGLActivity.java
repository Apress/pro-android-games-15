package opengl.test;

import opengl.test.MyGLRenderer;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.opengl.GLSurfaceView;


public class JavaGLActivity extends ActionBarActivity {

	  private GLSurfaceView glView;   // Use GLSurfaceView
	  
	   // Call back when the activity is started, to initialize the view
	   @Override
	   protected void onCreate(Bundle savedInstanceState) {
	      super.onCreate(savedInstanceState);
	      glView = new GLSurfaceView(this);           // Allocate a GLSurfaceView
	      glView.setRenderer(new MyGLRenderer(this)); // Use a custom renderer
	      this.setContentView(glView);                // This activity sets to GLSurfaceView
	   }
	   
	   // Call back when the activity is going into the background
	   @Override
	   protected void onPause() {
	      super.onPause();
	      glView.onPause();
	   }
	   
	   // Call back after onPause()
	   @Override
	   protected void onResume() {
	      super.onResume();
	      glView.onResume();
	   }
	}