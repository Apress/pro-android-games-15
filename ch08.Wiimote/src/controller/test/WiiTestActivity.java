package controller.test;

import controller.test.R;

import game.engine.controller.IController;
import game.engine.controller.Wiimote;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class WiiTestActivity extends Activity {
	
	static final String TAG =  "WiiTest";
	
	IController wii;
	//IController zee;
	
	Handler mHandler = new Handler();
	
	EditText etLog;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    
        etLog = (EditText)findViewById(R.id.editText2);
        
        wii = new Wiimote(this);
        
        wii.setListener(new IController.IControllerListener() {
			
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
				
			}
		});
        
        // Connect button
        Button b = (Button)findViewById(R.id.button1);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// Connect
		        wii.connect();
			}
		});

        // Disconnect button
        b = (Button)findViewById(R.id.button2);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				((Wiimote)wii).disconnect();
			}
		});
        
        // Exit button
        b = (Button)findViewById(R.id.button3);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				System.exit(0);
			}
		});

        // Rumble button
        b = (Button)findViewById(R.id.button4);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				((Wiimote)wii).rumble();
			}
		});
        
        // Get status button
        b = (Button)findViewById(R.id.button5);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				((Wiimote)wii).setLed(0);
				((Wiimote)wii).getStatus();
			}
		});

        // Send bytes button
        b = (Button)findViewById(R.id.button6);
        b.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				String s = ((TextView)findViewById(R.id.editText1)).getText().toString();
				String[] array = s.split(",");
				byte[] bytes = new byte [array.length];
				
				for (int i = 0; i < bytes.length; i++) {
					bytes[i] = Byte.parseByte(array[i], 16);
					Log.d(TAG, "S[" + i + "]=" + array[i] + " byte=" + bytes[i]);
				}
				((Wiimote)wii).sendBytes(bytes);
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