package cn.topsens.explorer;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.Type;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;

import java.util.ArrayList;

import cn.topsens.*;
import cn.topsens.Error;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ArrayAdapter<Resolution> resolutions = new ArrayAdapter<Resolution>(this, R.layout.spinner_item, new Resolution[]{ Resolution.Disabled, Resolution.QVGA, Resolution.VGA });

        Spinner cres = (Spinner)this.findViewById(R.id.cres);
        cres.setAdapter(resolutions);
        cres.setSelection(1);

        Spinner dres = (Spinner)this.findViewById(R.id.dres);
        dres.setAdapter(resolutions);
        dres.setSelection(1);

        ArrayAdapter<String> orientations = new ArrayAdapter<String>(this, R.layout.spinner_item, new String[]{ "Landscape", "Clockwise", "AntiClock" });
        ((Spinner)this.findViewById(R.id.orient)).setAdapter(orientations);

        ((Button)this.findViewById(R.id.refresh)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                onRefreshClicked();
            }
        });

        ((Button)this.findViewById(R.id.start)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                onStartClicked();
            }
        });

        ((Button)this.findViewById(R.id.stop)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                onStopClicked();
            }
        });

        this.rs = RenderScript.create(this);
        this.rd = new ScriptC_RenderDepth(this.rs);

        Sensor.initialize(this);

        if (enableFake) {
            Fake.open(Environment.getExternalStorageDirectory().getAbsolutePath() + "/topsens.tsr");
        }
    }

    @Override
    protected void onStart() {
        super.onStart();

        ArrayList<String> list = new ArrayList<String>();

        if (this.enableFake && PackageManager.PERMISSION_GRANTED != ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)) {
            list.add(Manifest.permission.READ_EXTERNAL_STORAGE);
        }

        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.O && PackageManager.PERMISSION_GRANTED != ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)) {
            list.add(Manifest.permission.CAMERA);
        }

        if (!list.isEmpty()) {
            ActivityCompat.requestPermissions(this, list.toArray(new String[list.size()]), 0);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.O && PackageManager.PERMISSION_DENIED == ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)) {
            Toast.makeText(this, "Camera permission is needed", Toast.LENGTH_SHORT).show();
            Log.i("MainActivity", "No camera permission");
        }

        this.onRefreshClicked();
        this.disableControls();
    }

    @Override
    protected void onPause() {
        super.onPause();
        this.stop();
    }

    @Override
    protected void onDestroy() {
        Sensor.uninitialize();
        this.rd.destroy();
        this.rs.destroy();
        super.onDestroy();

        if (enableFake) {
            Fake.close();
        }
    }

    private void onRefreshClicked() {
        this.disableOptions();

        int count = Sensor.count();

        if (count < 0) {
            Toast.makeText(this, "Failed to get sensor count: " + Sensor.error().toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to get sensor count: " + Sensor.error().toString());
            return;
        }

        if (0 == count) {
            ((Spinner)this.findViewById(R.id.device)).setAdapter(null);
            Toast.makeText(this, "No sensor found", Toast.LENGTH_SHORT).show();
            Log.i("MainActivity", "No sensor found");
            return;
        }

        String[] devices = new String[count];
        for (int i = 0; i < count; i++) {
            devices[i] = Integer.toString(i);
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, R.layout.spinner_item, devices);
        ((Spinner)this.findViewById(R.id.device)).setAdapter(adapter);

        this.enableOptions();
    }

    private void onStartClicked() {
        Sensor sensor = Sensor.open(((Spinner)this.findViewById(R.id.device)).getSelectedItemPosition());

        if (null == sensor) {
            Toast.makeText(this, "Failed to open sensor: " + Sensor.error().toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to open sensor: " + Sensor.error().toString());
            return;
        }

        if (sensor.hasPermission()) {
            this.sensor = sensor;
            this.start();
        } else {
            sensor.requestPermission(new Sensor.PermissionListener() {
                @Override
                public void onPermissionResult(Sensor sensor, boolean granted) {
                    if (!granted) {
                        Toast.makeText(MainActivity.this, "Sensor permission denied", Toast.LENGTH_SHORT).show();
                        Log.i("MainActivity", "Sensor permission denied");
                        return;
                    }

                    MainActivity.this.sensor = sensor;
                    MainActivity.this.start();
                }
            });
        }
    }

    private void onStopClicked() {
        this.stop();
    }

    private void start() {
        Resolution cres = (Resolution)((Spinner)this.findViewById(R.id.cres)).getSelectedItem();
        Resolution dres = (Resolution)((Spinner)this.findViewById(R.id.dres)).getSelectedItem();
        final boolean generateUsers = ((Switch)this.findViewById(R.id.users)).isChecked();

        Orientation orientation = Orientation.fromInt(((Spinner)this.findViewById(R.id.orient)).getSelectedItemPosition());

        Error err = this.sensor.setOrientation(orientation);
        if (Error.Ok != err) {
            Toast.makeText(MainActivity.this, "Failed to set sensor orientation: " + err.toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to set sensor orientation: " + err.toString());
            return;
        }

        err = this.sensor.setDepthAligned(((Switch)this.findViewById(R.id.align)).isChecked());
        if (Error.Ok != err) {
            Toast.makeText(MainActivity.this, "Failed to set depth alignment: " + err.toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to set depth alignment: " + err.toString());
            return;
        }

        err = this.sensor.setImageFlipped(((Switch)this.findViewById(R.id.flip)).isChecked());
        if (Error.Ok != err) {
            Toast.makeText(MainActivity.this, "Failed to set image flipping: " + err.toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to set image flipping: " + err.toString());
            return;
        }

        err = this.sensor.start(cres, dres, generateUsers);
        if (Error.Ok != err) {
            Toast.makeText(MainActivity.this, "Failed to start sensor: " + err.toString(), Toast.LENGTH_SHORT).show();
            Log.e("MainActivity", "Failed to start sensor: " + err.toString());
            return;
        }

        this.thread = new Thread() {
            @Override
            public void run() {
                ColorFrame cframe = new ColorFrame();
                DepthFrame dframe = new DepthFrame();
                UsersFrame uframe = new UsersFrame();

                ImageView cview = findViewById(R.id.cview);
                ImageView dview = findViewById(R.id.dview);

                Bitmap cbitmap = null;
                Bitmap dbitmap = null;

                while (!Thread.currentThread().isInterrupted()) {
                    Error err = sensor.waitColor(cframe, 1);

                    if (Error.Ok == err) {
                        if (null == cbitmap || cbitmap.getWidth() != cframe.Width || cbitmap.getHeight() != cframe.Height) {
                            cbitmap = Bitmap.createBitmap(cframe.Width, cframe.Height, Bitmap.Config.ARGB_8888);
                        }

                        this.paintColor(cframe, cbitmap);
                        this.updateView(cbitmap, cview);

                    } else if (Error.Timeout != err) {
                        Log.e("Thread", "Color error: " + err.toString());
                    }

                    err = sensor.waitDepth(dframe, 1);

                    if (Error.Ok == err) {
                        if (null == dbitmap || dbitmap.getWidth() != dframe.Width || dbitmap.getHeight() != dframe.Height) {
                            dbitmap = Bitmap.createBitmap(dframe.Width, dframe.Height, Bitmap.Config.ARGB_8888);
                        }

                        this.paintDepth(dframe, dbitmap);

                        if (generateUsers) {
                            err = sensor.waitUsers(uframe, 0);

                            if (Error.Ok == err) {
                                this.paintUsers(uframe, dbitmap);

                            } else {
                                Log.e("Thread", "Users error: " + err.toString());
                            }
                        }

                        this.updateView(dbitmap, dview);

                    } else if (Error.Timeout != err) {
                        Log.e("Thread", "Depth error: " + err.toString());
                    }
                }
            }

            private void paintColor(ColorFrame frame, Bitmap bitmap) {
                synchronized (bitmap) {
                    bitmap.setPixels(frame.Pixels, 0, frame.Width, 0, 0, frame.Width, frame.Height);
                }
            }

            private void paintDepth(DepthFrame frame, Bitmap bitmap) {
                Allocation depth = Allocation.createTyped(rs, new Type.Builder(rs, Element.I16(rs)).setX(frame.Width).setY(frame.Height).create());
                Allocation color = Allocation.createTyped(rs, new Type.Builder(rs, Element.RGBA_8888(rs)).setX(frame.Width).setY(frame.Height).create());

                depth.copy1DRangeFrom(0, frame.Width * frame.Height, frame.Pixels);
                rd.forEach_render(depth, color);

                synchronized (bitmap) {
                    color.copyTo(bitmap);
                }

                color.destroy();
                depth.destroy();
            }

            private void paintUsers(UsersFrame frame, Bitmap bitmap) {
                if (0 == frame.UserCount) {
                    return;
                }

                Canvas canvas = new Canvas(bitmap);
                Paint  paint  = new Paint();
                paint.setColor(Color.LTGRAY);
                paint.setStrokeWidth(3.f);

                for (int i = 0; i < frame.UserCount; i++) {
                    Joint[] joints = frame.Skeletons[i].Joints;

                    this.paintBone(frame, canvas, paint, joints[JointIndex.Head],      joints[JointIndex.Neck]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.LShoulder], joints[JointIndex.Neck]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.RShoulder], joints[JointIndex.Neck]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.LShoulder], joints[JointIndex.LElbow]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.RShoulder], joints[JointIndex.RElbow]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.LShoulder], joints[JointIndex.RWaist]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.RShoulder], joints[JointIndex.LWaist]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.LElbow],    joints[JointIndex.LHand]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.RElbow],    joints[JointIndex.RHand]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.LWaist],    joints[JointIndex.LKnee]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.RWaist],    joints[JointIndex.RKnee]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.LKnee],     joints[JointIndex.LFoot]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.RKnee],     joints[JointIndex.RFoot]);
                    this.paintBone(frame, canvas, paint, joints[JointIndex.LWaist],    joints[JointIndex.RWaist]);

                    this.paintJoint(frame, canvas, paint, joints[JointIndex.Head]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.Neck]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.LShoulder]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.RShoulder]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.LElbow]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.RElbow]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.LHand]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.RHand]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.LWaist]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.RWaist]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.LKnee]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.RKnee]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.LFoot]);
                    this.paintJoint(frame, canvas, paint, joints[JointIndex.RFoot]);
                }
            }

            private void paintJoint(UsersFrame frame, Canvas canvas, Paint paint, Joint joint) {
                Vector2 pos = frame.MapTo2D(joint.Position);
                if (pos.isNaN()) {
                    return;
                }

                canvas.drawCircle(pos.X, pos.Y, 4, paint);
            }

            private void paintBone(UsersFrame frame, Canvas canvas, Paint paint, Joint beg, Joint end) {
                Vector2 beg2d = frame.MapTo2D(beg.Position);
                Vector2 end2d = frame.MapTo2D(end.Position);

                if (beg2d.isNaN() || end2d.isNaN()) {
                    return;
                }

                canvas.drawLine(beg2d.X, beg2d.Y, end2d.X, end2d.Y, paint);
            }

            private void updateView(Bitmap bitmap, ImageView view) {
                Message message = new Message();
                message.obj = new Object[] { bitmap, view };
                handler.sendMessage(message);
            }
        };
        this.thread.start();
        this.disableOptions();
        this.enableControls();
    }

    private void stop() {
        if (null != this.thread) {
            this.thread.interrupt();

            try {
                this.thread.join();

            } catch (InterruptedException e) {
                Log.e("MainActivity", "Thread join exception: " + e.getMessage());
            }

            this.thread = null;
        }

        if (null != this.sensor) {
            this.sensor.stop();
            this.sensor.close();
            this.sensor = null;
        }

        this.disableControls();
        this.enableOptions();
    }

    private void enableOptions() {
        this.findViewById(R.id.device).setEnabled(true);
        this.findViewById(R.id.orient).setEnabled(true);
        this.findViewById(R.id.cres).setEnabled(true);
        this.findViewById(R.id.dres).setEnabled(true);
        this.findViewById(R.id.flip).setEnabled(true);
        this.findViewById(R.id.align).setEnabled(true);
        this.findViewById(R.id.users).setEnabled(true);
        this.findViewById(R.id.start).setEnabled(true);
    }

    private void disableOptions() {
        this.findViewById(R.id.device).setEnabled(false);
        this.findViewById(R.id.orient).setEnabled(false);
        this.findViewById(R.id.cres).setEnabled(false);
        this.findViewById(R.id.dres).setEnabled(false);
        this.findViewById(R.id.flip).setEnabled(false);
        this.findViewById(R.id.align).setEnabled(false);
        this.findViewById(R.id.users).setEnabled(false);
        this.findViewById(R.id.start).setEnabled(false);
    }

    private void enableControls() {
        this.findViewById(R.id.stop).setEnabled(true);
        this.findViewById(R.id.refresh).setEnabled(false);
    }

    private void disableControls() {
        this.findViewById(R.id.stop).setEnabled(false);
        this.findViewById(R.id.refresh).setEnabled(true);
    }

    private Sensor  sensor;
    private Thread  thread;
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message message) {
            Bitmap    bitmap = (Bitmap)((Object[])message.obj)[0];
            ImageView viewer = (ImageView)((Object[])message.obj)[1];

            synchronized (bitmap) {
                viewer.setImageBitmap(bitmap);
            }
        }
    };
    private RenderScript rs;
    private ScriptC_RenderDepth rd;

    // Set true to enable tsr playback. Copy tsr file to storage and pass the file path to Fake.open()
    private static final boolean enableFake = false;
}