package cn.topsens.squat;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Window;
import android.widget.Toast;

import cn.topsens.Sensor;
import cn.topsens.Error;

public class SplashActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getSupportActionBar().hide();
        setContentView(R.layout.activity_splash);

        Sensor.initialize(this);

        this.handler = new Handler();

        new Thread() {
            @Override
            public void run() {
                // Preload resources to minimize sensor start delay.
                // If skeleton is not needed, this can be omitted.
                final Error err = Sensor.preload(new int[] { 0, 1 });
                if (Error.Ok != err) {
                    handler.post(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(SplashActivity.this, "Failed to pre-load resources: " + err.toString(), Toast.LENGTH_SHORT).show();
                        }
                    });
                    Log.i("SplashActivity", "Failed to pre-load resources: " + err.toString());
                }

                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        Sensor.uninitialize();

                        Intent intent = new Intent(getApplicationContext(), MainActivity.class);
                        startActivity(intent);
                        finish();
                    }
                });
            }
        }.start();
    }

    private Handler handler;
}