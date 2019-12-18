package link.basov.asnp;

import android.os.Vibrator;
import android.os.Environment;
import android.os.VibrationEffect;
import android.content.Context;
import android.app.Activity;
import java.io.IOException;
import android.net.wifi.WifiManager;
import android.os.Build;

public class VPlay{
    protected static Vibrator vibrator = null;
    private static WifiManager.MulticastLock cLock;

        public static boolean startVibrator(android.app.Activity activity){
            //WifiManager wifi = (WifiManager) activity.getSystemService(Context.WIFI_SERVICE);
            //cLock = wifi.createMulticastLock("MOMApp");
            //cLock.acquire();


            vibrator = (Vibrator)activity.getSystemService(activity.VIBRATOR_SERVICE);
                return vibrator!=null;
	}
	
        public static boolean playLong(){
                if (vibrator!=null){
                    if (vibrator.hasVibrator()) {
                        if (Build.VERSION.SDK_INT >= 26) {
                            vibrator.vibrate(VibrationEffect.createOneShot(1500, VibrationEffect.DEFAULT_AMPLITUDE));
                        }else{
                            vibrator.vibrate(1500);
                        }
                    }
                }
                return true;
	}
        public static boolean playShort(){
                if (vibrator!=null){
                    if (vibrator.hasVibrator()) {
                        if (Build.VERSION.SDK_INT >= 26) {
                            vibrator.vibrate(VibrationEffect.createOneShot(500, VibrationEffect.DEFAULT_AMPLITUDE));
                        }else{
                            vibrator.vibrate(1500);
                        }
                    }
                }
                return true;
	}
	
        public static boolean stop(){
                if (vibrator!=null){
                    vibrator.cancel();
                }
                return true;
	}
}
