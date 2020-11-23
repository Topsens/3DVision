package cn.topsens.squat;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import cn.topsens.*;
import cn.topsens.Error;
import cn.topsens.toolkit.*;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.squat = BitmapFactory.decodeResource(this.getResources(), R.drawable.squat);
        this.stand = BitmapFactory.decodeResource(this.getResources(), R.drawable.stand);

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

        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.O && PackageManager.PERMISSION_GRANTED != ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)) {
            this.toast("Camera permission is needed to access the sensor");
            return;
        }

        int count = Sensor.count();
        if (count < 0) {
            this.toast("Failed to get sensor count: " + Sensor.error().toString());
            return;
        }

        if (0 == count) {
            this.toast("No sensor connected");
            return;
        }

        Sensor sensor = Sensor.open(0);
        if (!sensor.isRequestingPermission()) {
            sensor.requestPermission(new Sensor.PermissionListener() {
                @Override
                public void onPermissionResult(Sensor sensor, boolean granted) {
                    if (!granted) {
                        toast("Sensor permission has been denied");
                        return;
                    }

                    start(sensor);
                }
            });
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        this.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Sensor.uninitialize();
    }

    private void start(Sensor sensor) {
        this.sensor = sensor;
        this.extractor = new UserExtractor();

        Resolution res = Resolution.VGA;

        Error err = this.extractor.initialize(res);
        if (Error.Ok != err) {
            this.toast("Failed to initialize user extractor: " + err.toString());
            this.stop();
            return;
        }

        err = this.sensor.setOrientation(this.orient);
        if (Error.Ok != err) {
            this.toast("Failed to set sensor orientation: " + err.toString());
            this.stop();
            return;
        }

        err = this.sensor.setDepthAligned(true);
        if (Error.Ok != err) {
            this.toast("Failed to set depth alignment: " + err.toString());
            this.stop();
            return;
        }

        err = this.sensor.setMaskEnabled(true);
        if (Error.Ok != err) {
            this.toast("Failed to enable user mask: " + err.toString());
            this.stop();
            return;
        }

        err = this.sensor.start(res, res, true);
        if (Error.Ok != err) {
            this.toast("Failed to start sensor: " + err.toString());
            this.stop();
            return;
        }

        this.thread = new Thread() {
            @Override
            public void run() {
                ColorFrame cframe = new ColorFrame();
                UsersFrame uframe = new UsersFrame();

                int[] extracted = null;

                while (!Thread.currentThread().isInterrupted()) {
                    if (cframe.Timestamp <= uframe.Timestamp) {
                        Error err = MainActivity.this.sensor.waitColor(cframe, 5);

                        if (Error.Ok != err && Error.Timeout != err) {
                            Log.e("MainActivity", "Failed to wait color: " + err);
                        }
                    }

                    Error err = MainActivity.this.sensor.waitUsers(uframe, 5);

                    if (Error.Ok == err) {
                        if (null != cframe.Pixels) {
                            if (null == extracted || cframe.Pixels.length != extracted.length) {
                                extracted = new int[cframe.Pixels.length];
                            }

                            err = extractor.extract(cframe, uframe, extracted);
                            if (Error.Ok != err) {
                                Log.e("MainActivity", "Failed to extract users: " + err.toString());
                            }

                            if (null == bitmap || bitmap.getWidth() != cframe.Width || bitmap.getHeight() != cframe.Height) {
                                bitmap = Bitmap.createBitmap(cframe.Width, cframe.Height, Bitmap.Config.ARGB_8888);
                            }

                            synchronized (bitmap) {
                                bitmap.setPixels(extracted, 0, cframe.Width, 0, 0, cframe.Width, cframe.Height);
                            }
                            handler.sendEmptyMessage(0);

                            count(uframe);
                        }
                    }
                    else if (Error.Timeout != err) {
                        Log.e("MainActivity", "Failed to wait users: " + err.toString());
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
                Log.e("Exception", e.getMessage());
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

    private void count(UsersFrame users) {
        if (users.GroundPlane.isNaN()) {
            this.prompt(null, "请保持距摄像头两米左右", Calib);
            return;
        }

        if (0 == users.UserCount) {
            this.prompt("侦测用户中", "请保持距摄像头两米左右", Calib);
            this.progress(0);
            this.id = -1;
            this.count = 0;
            this.state = Calib;
            return;
        }

        switch (this.state) {
            case Calib:
            {
                this.prompt("校准中", "请面对摄像头直身站立", Calib);
                this.progress(0);

                if (this.id < 0) {
                    this.id = users.Skeletons[0].UserId;
                    this.count = 0;
                    this.height = 0;
                }

                int index = -1;
                for (int i = 0; i < users.UserCount; i++) {
                    if (this.id == users.Skeletons[i].UserId) {
                        index = i;
                        break;
                    }
                }

                if (index < 0) {
                    this.id = -1;
                    return;
                }

                Vector3 neck = users.Skeletons[index].Joints[JointIndex.Neck].Position;
                float height = Math.abs(neck.X * users.GroundPlane.X + neck.Y * users.GroundPlane.Y + neck.Z * users.GroundPlane.Z + users.GroundPlane.W);

                if (height > this.height) {
                    this.height = height;
                }

                if (++this.count > 45) {
                    this.count = 0;
                    this.state = Squat;
                    this.prompt("计数：" + Integer.toString(this.count), "请下蹲", this.state);
                }
                break;
            }

            case Squat:
            {
                int index = -1;
                for (int i = 0; i < users.UserCount; i++) {
                    if (this.id == users.Skeletons[i].UserId) {
                        index = i;
                        break;
                    }
                }

                if (index < 0) {
                    this.id = -1;
                    return;
                }

                Vector3 neck = users.Skeletons[index].Joints[JointIndex.Neck].Position;
                float height = Math.abs(neck.X * users.GroundPlane.X + neck.Y * users.GroundPlane.Y + neck.Z * users.GroundPlane.Z + users.GroundPlane.W);
                float target = this.height * Target;

                if (height < target) {
                    this.state = Stand;
                    this.prompt("计数：" + Integer.toString(this.count), "请直立", this.state);
                }
                else {
                    float percent = (height - target) / (this.height - target);
                    this.progress(1.0f - percent);
                }

                break;
            }

            case Stand:
            {
                int index = -1;
                for (int i = 0; i < users.UserCount; i++) {
                    if (this.id == users.Skeletons[i].UserId) {
                        index = i;
                        break;
                    }
                }

                if (index < 0) {
                    this.id = -1;
                    return;
                }

                Vector3 neck = users.Skeletons[index].Joints[JointIndex.Neck].Position;
                float height = Math.abs(neck.X * users.GroundPlane.X + neck.Y * users.GroundPlane.Y + neck.Z * users.GroundPlane.Z + users.GroundPlane.W);

                if (height > this.height - 0.05f) {
                    this.count++;
                    this.state = Squat;
                    this.prompt("计数：" + Integer.toString(this.count), "请下蹲", this.state);
                }
                else {
                    float target  = this.height * Target;
                    float percent = (height - target) / (this.height - target - 0.05f);
                    this.progress(percent);
                }

                break;
            }

            default: break;
        }
    }

    private void toast(String info) {
        final String i = info;

        this.handler.post(new Runnable() {
            @Override
            public void run() {
                if (null != i) {
                    Toast.makeText(MainActivity.this, i, Toast.LENGTH_LONG).show();
                    Log.i("MainActivity", i);
                }
            }
        });
    }

    private void prompt(String counter, String info, int action) {
        final String c = counter;
        final String i = info;
        final int    a = action;

        this.handler.post(new Runnable() {
            @Override
            public void run() {
                if (null != c) {
                    ((TextView)findViewById(R.id.counter)).setText(c);
                }

                if (null != i) {
                    ((TextView)findViewById(R.id.info)).setText(i);
                }

                Bitmap bitmap = null;
                switch (a) {
                    case Squat: {
                        bitmap = squat;
                        break;
                    }

                    case Stand: {
                        bitmap = stand;
                        break;
                    }

                    default: break;
                }

                ((ImageView)findViewById(R.id.action)).setImageBitmap(bitmap);
            }
        });
    }

    private void progress(float percentage) {
        final int p = (int)(Math.max(0.0f, Math.min(1.0f, percentage)) * 100);

        this.handler.post(new Runnable() {
            @Override
            public void run() {
                ((ProgressBar)findViewById(R.id.progress)).setProgress(p);
            }
        });
    }

    private Sensor sensor;
    private Thread thread;
    private Bitmap bitmap;
    private Bitmap squat;
    private Bitmap stand;

    private int   id;
    private int   count;
    private int   state;
    private float height;

    private UserExtractor extractor;

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message message) {
            synchronized (bitmap) {
                if (Orientation.Landscape == orient) {
                    ((ImageView)findViewById(R.id.view)).setImageBitmap(bitmap);
                }
                else {
                    Matrix m = new Matrix();

                    if (Orientation.PortraitClockwise == orient) {
                        m.postRotate(90);
                    }
                    else {
                        m.postRotate(-90);
                    }

                    ((ImageView)findViewById(R.id.view)).setImageBitmap(Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), m, true));
                }
            }
        }
    };

    private Orientation orient = Orientation.Landscape;

    private final int Calib = 0;
    private final int Squat = 1;
    private final int Stand = 2;
    private final float Target = 0.7f;
}