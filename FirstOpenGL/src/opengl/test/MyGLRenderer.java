package opengl.test;


import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
  
public class MyGLRenderer implements GLSurfaceView.Renderer {
   
   Triangle triangle;     // ( NEW )
      
   // Constructor
   public MyGLRenderer(Context context) {
      // Set up the data-array buffers for these shapes ( NEW )
      triangle = new Triangle();   // ( NEW )
   }

  
   // Call back when the surface is first created or re-created
   @Override
   public void onSurfaceCreated(GL10 gl, EGLConfig config) {
      gl.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set color's clear-value to black
      gl.glClearDepthf(1.0f);            // Set depth's clear-value to farthest
      gl.glEnable(GL10.GL_DEPTH_TEST);   // Enables depth-buffer for hidden surface removal
      gl.glDepthFunc(GL10.GL_LEQUAL);    // The type of depth testing to do
      gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT, GL10.GL_NICEST);  // nice perspective view
      gl.glShadeModel(GL10.GL_SMOOTH);   // Enable smooth shading of color
      gl.glDisable(GL10.GL_DITHER);      // Disable dithering for better performance
  
      // You OpenGL|ES initialization code here
      // ......
   }

     
   
   // Call back after onSurfaceCreated() or whenever the window's size changes
   @Override
   public void onSurfaceChanged(GL10 gl, int width, int height) {
      if (height == 0) height = 1;   // To prevent divide by zero
      float aspect = (float)width / height;
   
      // Set the viewport (display area) to cover the entire window
      gl.glViewport(0, 0, width, height);
  
      // Setup perspective projection, with aspect ratio matches viewport
      gl.glMatrixMode(GL10.GL_PROJECTION); // Select projection matrix
      gl.glLoadIdentity();                 // Reset projection matrix
      // Use perspective projection
      GLU.gluPerspective(gl, 45, aspect, 0.1f, 100.f);
  
      gl.glMatrixMode(GL10.GL_MODELVIEW);  // Select model-view matrix
      gl.glLoadIdentity();                 // Reset
  
      // You OpenGL|ES display re-sizing code here
      // ......
   }


   // Call back to draw the current frame.
   @Override
   public void onDrawFrame(GL10 gl) {
      // Clear color and depth buffers using clear-values set earlier 
      gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
  
      gl.glLoadIdentity();                 // Reset model-view matrix ( NEW )
      gl.glTranslatef(-1.5f, 0.0f, -6.0f); // Translate left and into the screen ( NEW )
      triangle.draw(gl);                   // Draw triangle ( NEW )
  
      // Translate right, relative to the previous translation ( NEW )
      gl.glTranslatef(3.0f, 0.0f, 0.0f);
   }
}