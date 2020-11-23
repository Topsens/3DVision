package cn.topsens.userextractor;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.ImageView;
import android.widget.Toast;

import cn.topsens.toolkit.*;
import cn.topsens.*;
import cn.topsens.Error;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Sensor.initialize(this);
    }

    @Override
    protected void onStart() {
        super.onStart();

        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.O && PackageManager.PERMISSION_GRANTED != ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)) {
            ActivityCompat.requestPermissions(this, new String[]{ Manifest.permission.CAMERA }, 0);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O && PackageManager.PERMISSION_DENIED == ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)) {
            Toast.makeText(this, "Camera permission is needed", Toast.LENGTH_SHORT).show();
            Log.i("MainActivity", "No camera permission");
            return;
        }

        if (null == this.sensor) {
            int count = Sensor.count();

            if (count < 0) {
                Toast.makeText(this, "Failed to get sensor count: " + Error.fromInt(-count).toString(), Toast.LENGTH_SHORT).show();
                Log.e("MainActivity", "Failed to get sensor count: " + Error.fromInt(-count).toString());
                return;
            }

            if (0 == count) {
                Toast.makeText(this, "No sensor found", Toast.LENGTH_SHORT).show();
                Log.i("MainActivity", "No sensor found");
                return;
            }

            this.sensor = Sensor.open(0);
            if (null == this.sensor) {
                Toast.makeText(this, "Failed to open sensor: " + Sensor.error().toString(), Toast.LENGTH_SHORT).show();
                Log.e("MainActivity", "Failed to open sensor: " + Sensor.error().toString());
                return;
            }

            this.sensor.requestPermission(new Sensor.PermissionListener() {
                @Override
                public void onPermissionResult(Sensor sensor, boolean granted) {
                    if (!granted) {
                        Toast.makeText(MainActivity.this, "Sensor permission has been denied", Toast.LENGTH_SHORT).show();
                        Log.i("MainActivity", "Sensor permission has been denied");
                        return;
                    }

                    start();
                }
            });
        }
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (null != this.sensor && !this.sensor.isRequestingPermission()) {
            this.stop();
        }
    }

    @Override
    protected void onDestroy() {
        Sensor.uninitialize();
        super.onDestroy();
    }

    private void start() {
        Resolution res = Resolution.VGA;

        this.extractor = new UserExtractor();

        Error err = this.extractor.initialize(res);
        if (Error.Ok != err) {
            Toast.makeText(this, "Failed to initialize extractor: " + err.toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to initialize extractor: " + err.toString());
            return;
        }

        err = this.sensor.setMaskEnabled(true);
        if (Error.Ok != err) {
            Toast.makeText(this, "Failed to enable user mask: " + err.toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to enable user mask: " + err.toString());
            return;
        }

        err = this.sensor.setDepthAligned(true);
        if (Error.Ok != err) {
            Toast.makeText(this, "Failed to set depth alignment: " + err.toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to set depth alignment: " + err.toString());
            return;
        }

        err = this.sensor.start(res, res, true);
        if (Error.Ok != err) {
            Toast.makeText(this, "Failed to start sensor: " + err.toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to start sensor: " + err.toString());
            return;
        }

        this.thread = new Thread() {
            @Override
            public void run() {
                ColorFrame cframe = new ColorFrame();
                UsersFrame uframe = new UsersFrame();

                while (!Thread.currentThread().isInterrupted()) {
                    Error err = sensor.waitUsers(uframe, 1);
                    if (Error.Ok == err) {
                        err = sensor.waitColor(cframe, 34);
                        if (Error.Ok != err) {
                            Log.e("MainActivity", "Failed to wait color frame: " + err.toString());
                            continue;
                        }

                        if (null == extracted || extracted.length != cframe.Width * cframe.Height) {
                            extracted = new int[cframe.Width * cframe.Height];
                        }

                        err = extractor.extract(cframe, uframe, extracted);
                        if (Error.Ok != err) {
                            Log.e("MainActivity", "Failed to extract users: " + err.toString());
                            continue;
                        }

                        if (null == bitmap || bitmap.getWidth() != cframe.Width || bitmap.getHeight() != cframe.Height) {
                            bitmap = Bitmap.createBitmap(cframe.Width, cframe.Height, Bitmap.Config.ARGB_8888);
                        }

                        synchronized (bitmap) {
                            bitmap.setPixels(extracted, 0, cframe.Width, 0, 0, cframe.Width, cframe.Height);
                        }

                        handler.sendEmptyMessage(0);

                    } else if (Error.Timeout != err) {
                        Log.e("MainActivity", "Failed to wait users frame: " + err.toString());
                    }
                }
            }
        };
        this.thread.start();
    }

    private void stop() {
        if (null != this.thread) {
            this.thread.interrupt();

            try {
                this.thread.join();

            } catch (Exception e) {
                Log.e("MainActivity", e.getMessage());
            }

            this.thread = null;
        }

        if (null != this.sensor) {
            this.sensor.stop();
            this.sensor.close();
            this.sensor = null;
        }

        if (null != this.extractor) {
            this.extractor.close();
            this.extractor = null;
        }
    }

    private Sensor sensor;
    private UserExtractor extractor;

    private int[]  extracted;
    private Bitmap bitmap;

    private Thread  thread;
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message message) {
            synchronized (bitmap) {
                ((ImageView)findViewById(R.id.view)).setImageBitmap(bitmap);
            }
        }
    };
}
