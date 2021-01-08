package cn.topsens.realsense;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.Handler;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.Type;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;

import java.lang.reflect.Field;
import java.util.ArrayList;

import com.intel.realsense.librealsense.*;

import cn.topsens.*;
import cn.topsens.Error;
import cn.topsens.Config;

public class MainActivity extends AppCompatActivity {
    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        for (int i = 0; i < permissions.length; i++) {
            if (Manifest.permission.CAMERA.equals(permissions[i])) {
                if (PackageManager.PERMISSION_GRANTED == grantResults[i]) {
                    this.initialize();
                }
                else {
                    this.toast("Camera permission denied");
                }
                return;
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.findViewById(R.id.resolution).setEnabled(false);
        this.findViewById(R.id.flip).setEnabled(false);

        Spinner device = (Spinner)this.findViewById(R.id.device);
        device.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                onDevice();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });

        ArrayAdapter<String> orientations = new ArrayAdapter<String>(this, R.layout.spinner_item, new String[]{ "Landscape", "Clockwise", "AntiClock" });
        ((Spinner)this.findViewById(R.id.orientation)).setAdapter(orientations);

        Button start = (Button)this.findViewById(R.id.start);
        start.setEnabled(false);
        start.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                start();
            }
        });

        Button stop = (Button)this.findViewById(R.id.stop);
        stop.setEnabled(false);
        stop.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                stop();
            }
        });

        this.rs = RenderScript.create(this);
        this.rd = new ScriptC_RenderDepth(this.rs);
    }

    @Override
    protected void onStart() {
        super.onStart();

        if (PackageManager.PERMISSION_GRANTED != ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, 0);
        } else {
            this.initialize();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        this.stop();

        if (null != this.context) {
            this.context.close();
            this.context = null;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.rd.destroy();
        this.rs.destroy();
    }

    private void onDevice() {
        Spinner device = (Spinner)this.findViewById(R.id.device);
        int pos = device.getSelectedItemPosition();
        if (pos < 0) {
            return;
        }

        try (DeviceList devices = this.context.queryDevices()) {
            if (pos < devices.getDeviceCount()) {
                try (Device dev = devices.createDevice(pos)) {
                    String serial = dev.getInfo(CameraInfo.SERIAL_NUMBER);
                    if (serial.equals(this.serial)) {
                        return;
                    }

                    this.serial = serial;

                    String name = dev.getInfo(CameraInfo.NAME);
                    if (name.contains("D415")) {
                        this.config = Config.RSD415;
                    } else if (name.contains("D435")) {
                        this.config = Config.RSD435;
                    } else {
                        this.config = 0;
                    }

                    ArrayList<String> list = new ArrayList<String>();

                    int prefered = 0;
                    for (Sensor sensor : dev.querySensors()) {
                        for (StreamProfile stream : sensor.getStreamProfiles()) {
                            if (StreamType.DEPTH == stream.getType() &&
                                    StreamFormat.Z16 == stream.getFormat()) {
                                VideoStreamProfile prof = (VideoStreamProfile) stream.as(Extension.VIDEO_PROFILE);

                                String res = Integer.toString(prof.getWidth()) + 'x' + Integer.toString(prof.getHeight()) + '@' + Integer.toString(stream.getFrameRate());

                                // Pick up only streams that have 30 fps supported
                                if (30 == stream.getFrameRate()) {
                                    list.add(res);
                                    Log.i("MainActivity", "Selected: " + res);

                                    if (640 == prof.getWidth() && 480 == prof.getHeight()) {
                                        prefered = list.size() - 1;
                                    }
                                }
                                else {
                                    Log.i("MainActivity", "Discarded: " + res);
                                }
                            }
                        }
                    }

                    Spinner resolution = (Spinner)this.findViewById(R.id.resolution);
                    resolution.setAdapter(new ArrayAdapter<String>(this, R.layout.spinner_item, list.toArray(new String[list.size()])));
                    resolution.setSelection(prefered);
                }
            }
        }
    }

    private void initialize() {
        RsContext.init(this);

        this.context = new RsContext();
        this.context.setDevicesChangedCallback(new DeviceListener() {
            @Override
            public void onDeviceAttach() {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        refresh();
                    }
                });
            }

            @Override
            public void onDeviceDetach() {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        refresh();
                    }
                });
            }
        });

        this.refresh();
        this.enable();
    }

    private void refresh() {
        try (DeviceList devices = this.context.queryDevices()) {
            int count = devices.getDeviceCount();

            if (count > 0) {
                ArrayList<String> list = new ArrayList<String>();

                int selected = 0;
                for (int i = 0; i < count; i++) {
                    try (Device device = devices.createDevice(i)) {
                        String name = device.getInfo(CameraInfo.NAME);
                        int index = name.indexOf("Intel RealSense ");
                        if (index >= 0) {
                            name = name.substring(16);
                        }
                        list.add(name);

                        if (device.getInfo(CameraInfo.SERIAL_NUMBER).equals(this.serial)) {
                            selected = i;
                        }
                    }
                }

                Spinner device = (Spinner)this.findViewById(R.id.device);
                device.setAdapter(new ArrayAdapter<String>(this, R.layout.spinner_item, list.toArray(new String[list.size()])));
                device.setSelection(selected);

            } else {
                Spinner device = (Spinner)this.findViewById(R.id.device);
                device.setAdapter(new ArrayAdapter<String>(this, R.layout.spinner_item, new String[0]));

                Spinner resolution = (Spinner)this.findViewById(R.id.resolution);
                resolution.setAdapter(new ArrayAdapter<String>(this, R.layout.spinner_item, new String[0]));

                this.serial = null;
            }
        }
    }

    private void start() {
        if (null == this.serial) {
            Toast.makeText(this, "No RealSense camera selected", Toast.LENGTH_SHORT).show();
            Log.i("MainActivity", "No RealSense camera selected");
            return;
        }

        String res = (String)((Spinner)this.findViewById(R.id.resolution)).getSelectedItem();

        final int w = getWidth(res);
        final int h = getHeight(res);
        final int fps = getFps(res);
        final boolean flip = ((Switch)this.findViewById(R.id.flip)).isChecked();
        final Orientation orient = Orientation.fromInt(((Spinner)this.findViewById(R.id.orientation)).getSelectedItemPosition());

        if (Orientation.Landscape == orient) {
            this.rotate = 0;
        } else if (Orientation.PortraitClockwise == orient) {
            this.rotate = 90;
        } else {
            this.rotate = -90;
        }

        this.bitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);

        this.thread = new Thread() {
            @Override
            public void run() {
                Pipeline pipe = new Pipeline();
                People people = new People();

                try ( com.intel.realsense.librealsense.Config cfg = new  com.intel.realsense.librealsense.Config()) {
                    cfg.enableDevice(serial);
                    cfg.enableStream(StreamType.DEPTH, -1, w, h, StreamFormat.Z16, fps);

                    try (PipelineProfile prof = pipe.start(cfg)) {
                        Error err = people.setImageFlipped(flip);
                        if (Error.Ok != err) {
                            toast("Failed to set image flip: " + err.toString());
                            return;
                        }

                        err = people.setOrientation(orient);
                        if (Error.Ok != err) {
                            toast("Failed to set image orientation: " + err.toString());
                            return;
                        }

                        // if camera is offically supported, preset config can be used to initialize people object.
                        // otherwise we have to retrieve camera parameters to initialize people.
                        if (0 != config) {
                            err = people.setConfig(config);
                            if (Error.Ok != err) {
                                toast("Failed to set config: " + err.toString());
                                return;
                            }

                            err = people.initialize(w, h);
                        }
                        else {
                            float fx, fy, cx, cy;
                            Intrinsic intr = getIntrisic(prof, w, h);
                            if (null == intr) {
                                toast("Failed to get intrinsic");
                                return;
                            }

                            fx = getFloat(intr, "mFx");
                            fy = getFloat(intr, "mFy");
                            cx = getFloat(intr, "mPpx");
                            cy = getFloat(intr, "mPpy");

                            err = people.initialize(w, h, fx, fy, cx, cy);
                        }

                        if (Error.Ok != err) {
                            toast("Failed to initialize people: " + err.toString());
                            return;
                        }

                        byte[] data = new byte[w * h * 2];
                        Allocation depth = Allocation.createTyped(rs, new Type.Builder(rs, Element.U8(rs)).setX(w * 2).setY(h).create());
                        UsersFrame users = new UsersFrame();

                        while (!Thread.currentThread().interrupted()) {
                            try (FrameSet frames = pipe.waitForFrames()) {
                                try (Frame frame = frames.first(StreamType.DEPTH, StreamFormat.Z16)) {
                                    frame.getData(data);
                                    depth.copy1DRangeFrom(0, data.length, data);

                                    if (flip) {
                                        flipDepth(depth, data);
                                    }

                                    err = people.detect(data, w, h, (long)frame.getTimestamp(), users);
                                    if (Error.Ok != err) {
                                        Log.e("MainActivity", "Failed to detect: " + err.toString());
                                    }

                                    paint(depth, users);
                                }
                            }
                        }

                        depth.destroy();

                    } catch (Exception e) {
                        toast("Error: " + e.getMessage());
                    } finally {
                        people.uninitialize();
                        people.close();
                        pipe.stop();
                        pipe.close();
                    }
                }
            }
        };
        this.thread.start();
        this.disable();
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
        this.enable();
    }

    private void enable() {
        this.findViewById(R.id.orientation).setEnabled(true);
        this.findViewById(R.id.resolution).setEnabled(true);
        this.findViewById(R.id.device).setEnabled(true);
        this.findViewById(R.id.flip).setEnabled(true);
        this.findViewById(R.id.start).setEnabled(true);
        this.findViewById(R.id.stop).setEnabled(false);
    }

    private void disable() {
        this.findViewById(R.id.orientation).setEnabled(false);
        this.findViewById(R.id.resolution).setEnabled(false);
        this.findViewById(R.id.device).setEnabled(false);
        this.findViewById(R.id.flip).setEnabled(false);
        this.findViewById(R.id.start).setEnabled(false);
        this.findViewById(R.id.stop).setEnabled(true);
    }

    private void toast(String info) {
        this.info = info;

        this.handler.post(new Runnable() {
            @Override
            public void run() {
                String info = MainActivity.this.info;

                if (null != info) {
                    Toast.makeText(MainActivity.this, info, Toast.LENGTH_LONG).show();
                    Log.i("MainActivity", info);
                }
            }
        });
    }

    private void flipDepth(Allocation depth, byte[] data) {
        Allocation alloc = Allocation.createTyped(rs, new Type.Builder(rs, Element.U8(rs)).setX(depth.getType().getX()).setY(depth.getType().getY()).create());
        alloc.copyFrom(depth);
        this.rd.set_depth(alloc);
        this.rd.set_width(alloc.getType().getX());
        this.rd.forEach_flip(depth);
        depth.copyTo(data);
        alloc.destroy();
    }

    private void paint(Allocation depth, UsersFrame users) {
        synchronized (this.bitmap) {
            this.paintDepth(depth);
            this.paintUsers(users);
        }
        this.handler.post(new Runnable() {
            @Override
            public void run() {
                ImageView dview = (ImageView)findViewById(R.id.dview);

                float scale = 1.0f;

                if (0 != rotate) {
                    if ((float)dview.getWidth() / dview.getHeight() > (float)bitmap.getWidth() / bitmap.getHeight()) {
                        scale = (float)bitmap.getHeight() / bitmap.getWidth();

                        if (dview.getHeight() * scale > dview.getWidth()) {
                            scale = (float)dview.getWidth() / dview.getHeight();
                        }
                    } else {
                        scale = (float)bitmap.getWidth() / bitmap.getHeight();

                        if (dview.getWidth() * scale > dview.getHeight()) {
                            scale = (float)dview.getHeight() / dview.getWidth();
                        }
                    }
                }

                dview.setScaleX(scale);
                dview.setScaleY(scale);
                dview.setRotation(rotate);

                synchronized (bitmap) {
                    dview.setImageBitmap(bitmap);
                }
            }
        });
    }

    private void paintDepth(Allocation depth) {
        Allocation color = Allocation.createFromBitmap(rs, bitmap);
        this.rd.set_depth(depth);
        this.rd.forEach_render(color);
        color.copyTo(this.bitmap);
        color.destroy();
    }

    private void paintUsers(UsersFrame frame) {
        if (0 == frame.UserCount) {
            return;
        }

        Canvas canvas = new Canvas(this.bitmap);
        Paint paint = new Paint();
        paint.setColor(Color.LTGRAY);
        paint.setStrokeWidth(3.f);

        for (int i = 0; i < frame.UserCount; i++) {
            Joint[] joints = frame.Skeletons[i].Joints;

            this.paintBone(frame, canvas, paint, joints[JointIndex.Head], joints[JointIndex.Neck]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.LShoulder], joints[JointIndex.Neck]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.RShoulder], joints[JointIndex.Neck]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.LShoulder], joints[JointIndex.LElbow]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.RShoulder], joints[JointIndex.RElbow]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.LShoulder], joints[JointIndex.RWaist]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.RShoulder], joints[JointIndex.LWaist]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.LElbow], joints[JointIndex.LHand]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.RElbow], joints[JointIndex.RHand]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.LWaist], joints[JointIndex.LKnee]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.RWaist], joints[JointIndex.RKnee]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.LKnee], joints[JointIndex.LFoot]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.RKnee], joints[JointIndex.RFoot]);
            this.paintBone(frame, canvas, paint, joints[JointIndex.LWaist], joints[JointIndex.RWaist]);

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

    private static int getWidth(String res) {
        return Integer.parseInt(res.substring(0, res.indexOf('x')));
    }

    private static int getHeight(String res) {
        return Integer.parseInt(res.substring(res.indexOf('x') + 1, res.indexOf('@')));
    }

    private static int getFps(String res) {
        return Integer.parseInt(res.substring(res.indexOf('@') + 1));
    }

    private static float getFloat(Intrinsic intr, String field) {
        try {
            Field f = intr.getClass().getDeclaredField(field);
            f.setAccessible(true);
            return f.getFloat(intr);

        } catch (Exception e) {
            return 0.f;
        }
    }

    private static Intrinsic getIntrisic(PipelineProfile prof, int w, int h) {
        for (Sensor sensor : prof.getDevice().querySensors()) {
            for (StreamProfile stream : sensor.getStreamProfiles()) {
                if (StreamType.DEPTH == stream.getType()) {
                    try {
                        Intrinsic intr = ((VideoStreamProfile)stream.as(Extension.VIDEO_PROFILE)).getIntrinsic();
                        Log.i("MainActivity", Integer.toString(intr.getWidth()) + ' ' + Integer.toString(intr.getHeight()));

                        if (intr.getWidth() == w || intr.getHeight() == h) {
                            return intr;
                        }
                    } catch (Exception e) {
                    }
                }
            }
        }

        return null;
    }

    private int rotate;
    private int config;
    private String info;
    private String serial;
    private Bitmap bitmap;
    private Thread thread;
    private RsContext context;
    private RenderScript rs;
    private ScriptC_RenderDepth rd;

    private Handler handler = new Handler();
}