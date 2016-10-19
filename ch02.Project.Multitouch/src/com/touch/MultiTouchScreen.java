package com.touch;

import java.util.ArrayList;

import android.graphics.Point;
import android.view.MotionEvent;

public class MultiTouchScreen {

    private ArrayList<MultiTouchGesture> mGestures;
    
    /**
     * Constructor
     * @param gestures
     */
    public MultiTouchScreen(ArrayList<MultiTouchGesture> gestures) { 
        mGestures = gestures;
    }
    
    
    /**
     * Touch Event.
     * Events with bogus pressure pop up when using 1 finger 
     * @param e
     */
    public void onTouchEvent(MotionEvent e) 
    {
        final int action = e.getAction();
        int count = e.getPointerCount();
        
        Point[] points = new Point[count];
        
        // for each finger extract coords
        for (int i = 0; i < points.length; i++) {
            points[i] = new Point((int)e.getX(i), (int)e.getY(i));
        }
        
        // for each gesture
        for ( MultiTouchGesture g : mGestures) {
            // for each finger (pointer)
            for (int j = 0; j < count; j++) { 
                if ( g.bounds.contains(points[j].x, points[j].y) ) {
                    g.execute(action, points[j]);
                }
            }
        }
        // reset when finger goes up
        if ( action == MotionEvent.ACTION_UP ) {
            for (MultiTouchGesture g : mGestures) {
                g.reset();
            }
        } 
        
    }
}
