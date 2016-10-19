package com.touch;

import android.graphics.Point;
import android.graphics.Rect;
import android.view.MotionEvent;

public class MultiTouchGesture {
    public enum eGestureType {MOVE, LOOK};
    
    Rect bounds;
    eGestureType type;

    public MultiTouchGesture(eGestureType type,  Rect bounds) {
        this.type = type;
        this.bounds = bounds;
    }
    
    /**
     * Execute gesture 
     * @param action {@link MotionEvent} action: ACTION_UP, ACTION_MOVE,...
     * @param p Finger point XY coordinates
     * @return
     */
    public boolean execute(int action, Point p) {
        switch (type) {
        case MOVE:
            doMove(action, p);
            break;
            
        case LOOK:
            doLook(action, p);
            break;
            
        default:
            break;
        }
        return true; 
    }
    
    public void reset() {
        switch (type) {
        case MOVE:
            
            break;
        case LOOK:
            break;
            
        default:
            break;
        }
    }

    private void doMove(int action, Point p) {
        // Tell the native engine to move
    }

    private void doLook(int action, Point p) {
        // Tell native engine to look
    }
    
}
