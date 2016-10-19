package com.opengl.shader;

import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;

public class ShadersActivity extends Activity {
    /** Called when the activity is first created. */
	
	ShadersView view;
	int width ;
	int height;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.main);
        
        width = getWindowManager().getDefaultDisplay().getWidth();
        height = getWindowManager().getDefaultDisplay().getHeight();
        
        String[] args = {};
        
        view = new ShadersView(this);
        view.setRenderer(args ,false, 0, 0);
        
        setContentView(view);
    }
    
    int speed = 10;
    
    float p1X, p1Y, p2X, p2Y;
    float DX1, DX2;
    int fingers =0;
    
    @Override
    public boolean onTouchEvent(MotionEvent e) {
    	int count = e.getPointerCount();
    	int action = e.getAction();
    	float X1 = 0f, Y1 = 0f, X2 = 0f, Y2 = 0f;
    	
    	
    	if ( action == MotionEvent.ACTION_DOWN) {
    		p1X = e.getX(0);
    		p1Y = e.getY(0);
    		//System.out.println("P1 down x=" + p1X + " y=" + p1Y);
    		fingers = 1;
    	}

    	if ( action == MotionEvent.ACTION_POINTER_2_DOWN) {
    		p2X = e.getX(1);
    		p2Y = e.getY(1);
    		//System.out.println("P2 down x=" + p2X + " y=" + p2Y);
    		fingers = 2;
    	}

//    	System.out.println("count=" + count + " action=" + action
//    			+ " P1(" + e.getX(0) + "," + e.getY(0) + ")"
//    			+ " O1(" + p1X + "," + p1Y + ")"
//    			+ " P2(" + e.getX(1) + "," + e.getY(1) + ")"
//    			+ " O2(" + p2X + "," + p2Y + ")"
//    			);
    	/*
    	if ( action == MotionEvent.ACTION_POINTER_1_UP) {
        	//if ( count == 1 && p2X == 0) {
        		X1 = e.getX(0);
        		Y1 = e.getY(0);
        		//setRotationSpeed(X1, p1X);
        		
        		DX1 = X1 -p1X;
        		
        		System.out.println("P1 Up DX1=" + DX1 + " DX2=" + DX2);
        		//p1X = p1Y = 0f;
        		//return true;
        	//}
    	} */
   	
    	if ( action == MotionEvent.ACTION_UP) {
    		X1 = e.getX(0);
    		Y1 = e.getY(0);
			DX1 = X1 - p1X;
    		
    		X2 = e.getX(1);
    		Y2 = e.getY(1);
    		
    		DX2 = X2 - p2X;
    	
    		//System.out.println("P1/2 UP DX1=" + DX1 + " DX2=" + DX2);
    	}
    	
    	if ( action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_POINTER_2_UP) {
    		//System.out.println("ACTION DX1=" + DX1 + " DX2=" + DX2 + " fingers=" + fingers);
    		
    		if ( fingers == 1) {
    			//System.out.println("Swipe DX=" + DX1);
    			setRotationSpeed(DX1);
    		}
    		else if ( fingers == 2) {
    			setPinch(DX1, DX2);
    		}
    		p1X = p1Y = p2X = p2Y = DX1 = DX2 = 0f;
    		fingers = 0;
    	}
    	return super.onTouchEvent(e);
    }
    
    private void setPinch(float DX1, float DX2) {
    	if ( DX1 > 0 && DX2 < 0 ) {
    		width *= 0.6;
    		height *= 0.8;
    		System.out.println("PINCH IN w=" + width + " h=" + height);
    		view.setVideoSize(width, height);
    	}
    	else {
    		width *= 1.4;
    		height *= 1.2;
    		System.out.println("PINCH OUT w=" + width + " h=" + height);
    		view.setVideoSize(width, height);
    	}
    }
    
    // 0-50 left, 50-100 right
    //private void setRotationSpeed(float x1 , float x0) {
    private void setRotationSpeed(float DX) {
		// left -> or rite -> left
		//float DX = x1 -x0;
		
		//System.out.println("X0=" + x0 + " X1=" + x1 + " DX=" + DX);
		
		if ( DX < 0 ) {
			speed -= 20;
		}
		else {
			speed += 20;
		}
		// clamp 0-100
		if ( speed < 0) speed = 0;
		if (speed > 100) speed = 100;
		
		view.setRotationSpeed(speed);
    	
    }
}