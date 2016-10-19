package quake.audio;

import java.nio.ByteBuffer;

import quake.jni.Natives;

import android.media.AudioFormat;
import android.media.AudioTrack;
import android.util.Log;

public class NativeAudio {
  private static final String TAG = "Audio";

  // private static AudioTrack mTrack;
  private static boolean mDone = false;

  private static boolean mPaused = false;

  private static boolean mStarted = false;

  public static void start() {
    if (mStarted) {
      Log.e(TAG, "Audio already started.");
      return;
    }

    mStarted = true;

    new Thread(new Runnable() {
      public void run() {
        AudioTrack mTrack;
        mTrack = new AudioTrack(android.media.AudioManager.STREAM_MUSIC,
            22050, // 44100,
            AudioFormat.CHANNEL_CONFIGURATION_STEREO,
            AudioFormat.ENCODING_PCM_16BIT, 4 * (22050 / 5), // 200 millisecond
                                                             // buffer => impact
                                                             // on audio latency
            AudioTrack.MODE_STREAM);

        int audioSize = (2048 * 4);
        ByteBuffer audioBuffer = ByteBuffer.allocateDirect(audioSize);

        byte[] audioData = new byte[audioSize];

        // GameTools.sleep(3000);

        Log.d(TAG, "Audio start.");
        mTrack.play();

        while (!mDone) {
          if (!mPaused) {
            Natives.PaintAudio(audioBuffer);

            audioBuffer.position(0);
            audioBuffer.get(audioData);

            // Write the byte array to the track
            mTrack.write(audioData, 0, audioData.length);
          } else {
            try {
              Thread.sleep(100);
            } catch (Exception e) {
            }
          }
        }

      }
    }).start();

  }

  public static void pause() {
    Log.i(TAG, "Audio Pause.");
    mPaused = true;
  }

  public static void resume() {
    Log.i(TAG, "Audio Resume.");
    mPaused = false;
  }

  public static void quit() {
    Log.d(TAG, "Audio Quit.");
    mDone = true;
    mStarted = false;
  }
}
