package ch02.project;

import java.util.ArrayList;

import com.touch.MultiTouchGesture;
import com.touch.MultiTouchScreen;
import com.touch.MultiTouchGesture.eGestureType;

import controller.Zeemote;

import jni.Natives;

import android.app.Activity;
import android.graphics.Rect;
import android.os.Bundle;
import android.view.MotionEvent;


public class MainActivity extends Activity 
{
    MultiTouchScreen mtScreen;

	{
		System.loadLibrary("ch02");
	}
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        initMultiTouchScreen();
        
        try {
        	
        	// Run it
			String[] argv = {"MyLib", "arg1", "arg2"};
			
			Natives.LibMain(argv);
			
		} catch (Exception e) {
			e.printStackTrace();
		}
    }
    
    private void initMultiTouchScreen() {
        // init multi touch screen
        ArrayList<MultiTouchGesture> gestures = new ArrayList<MultiTouchGesture>();
        
        @SuppressWarnings("deprecation")
		int w = getWindow().getWindowManager().getDefaultDisplay().getWidth();
        int h = getWindow().getWindowManager().getDefaultDisplay().getHeight();
        
       
        
        // move: left side of the screen
        gestures.add(new MultiTouchGesture(eGestureType.MOVE, new Rect(0,0, w/2, h )));
        
        // look right side
        gestures.add(new MultiTouchGesture(eGestureType.LOOK, new Rect(w/2,0, w, h )));
        
        mtScreen = new MultiTouchScreen(gestures);
        
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        mtScreen.onTouchEvent(event);
        return true;
    }
    
    private void connectZeemote() {
      Zeemote zee = new Zeemote(this);
      zee.connect();
    }
    
    /**
     * Bluetooth enabled check. If not enabled will start the SYS enable BT stuff
     * @param ctx
     * @return true if BT enabled, false otherwise
     */
  
    
}