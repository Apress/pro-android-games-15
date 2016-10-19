package controller.test;

import controller.test.R;

import game.engine.controller.IController;
import game.engine.controller.Zeemote;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class ZeeTestActivity extends Activity {
	
	static final String TAG =  "TEST";
	
	IController zee;
	
	Handler mHandler = new Handler();
	
	EditText etLog;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.zee);
    
        etLog = (EditText)findViewById(R.id.editText2);
        
        zee = new Zeemote(this, 1);
        
        zee.setListener(new IController.IControllerListener() {
			
			@Override
			public void onButtonEvent(boolean pressed, int ascii, int scanCode) {
				Log.d(TAG,"Event pressed=" + pressed + " ASCII=" + ascii + " sc=" + scanCode);
				postLog("Event pressed=" + pressed + " ASCII=" + ascii);
			}
			
			@Override
			public void disconnected(String reason) {
				Log.e(TAG, "Disconnected from wiimote");
				postLog("Disconnected: " + reason);
			}
			
			@Override
			public void connected() {
				Log.d(TAG, "Connected to wiimote");
				postLog("Connected to wiimote");
			}

			@Override
			public void onMessage(String message) {
				postLog(message);
			}

			@Override
			public void onJoystickEvent(float angleDeg, float distance) {
				// TODO Auto-generated method stub
				Log.d(TAG, "Joy angle:" + angleDeg + " distance:" + distance);
				postLog("Joy angle:" + angleDeg + " d:" + distance);
				
			}
		});
        
        Button b = (Button)findViewById(R.id.button1);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
		        zee.connect();
		        //((Wiimote)wii).connect("58:BD:A3:C9:99:20");
			}
		});

        b = (Button)findViewById(R.id.button2);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				zee.disconnect();
			}
		});
        
        b = (Button)findViewById(R.id.button3);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				System.exit(0);
			}
		});

        
        // status
        b = (Button)findViewById(R.id.button5);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				Log.d(TAG, "Status");
			}
		});

        
        // clear log
        b = (Button)findViewById(R.id.button7);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				etLog.setText("");
			}
		});

    }
    
    void postLog(final String text) {
    	mHandler.post(new Runnable() {
			public void run() {
				etLog.append(text + "\n");
			}
		});
    }
}