using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
using UnityEngine;
using Intel.RealSense;

public delegate void DepthHandler(short[] depth, long timestmap);

public class SensorBehaviour : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        this.CheckOnQuit();
    }

    #if UNITY_ANDROID && !UNITY_EDITOR
    void Awake()
    {
        if (!UnityEngine.Android.Permission.HasUserAuthorizedPermission(UnityEngine.Android.Permission.Camera) && AndroidSdkInt() > 26)
        {
            UnityEngine.Android.Permission.RequestUserPermission(UnityEngine.Android.Permission.Camera);
        }
        
        RsContextInit();
    }

    void OnApplicationFocus(bool focused)
    {
        if (focused)
        {
            if (AndroidSdkInt() > 26 && !UnityEngine.Android.Permission.HasUserAuthorizedPermission(UnityEngine.Android.Permission.Camera))
            {
                Debug.Log("No camera permission");
            }
            else
            {
                this.StartSensor();
            }
        }
        else
        {
            this.StopSensor();
        }
    }

    private static int AndroidSdkInt()
    {
        return new AndroidJavaClass("android.os.Build$VERSION").GetStatic<int>("SDK_INT");
    }
    
    private static void RsContextInit()
    {
        using (var RsContext = new AndroidJavaClass("com.intel.realsense.librealsense.RsContext"))
        using (var UnityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer"))
        using (var activity = UnityPlayer.GetStatic<AndroidJavaObject>("currentActivity"))
        {
            RsContext.CallStatic("init", activity.Call<AndroidJavaObject>("getApplicationContext"));
        }
    }
    #else
    void OnEnable()
    {
        this.StartSensor();
    }

    void OnDisable()
    {
        this.StopSensor();
    }
    #endif

    private void StartSensor()
    {
        var cfg = new Config();
        cfg.EnableStream(Stream.Depth, 480, 270, Format.Z16, 30);

        this.pipe = new Pipeline();
        try
        {
            using (var stream = this.pipe.Start(cfg).GetStream<VideoStreamProfile>(Stream.Depth))
            {
                this.Width = stream.Width;
                this.Height = stream.Height;
                this.Intrinsics = stream.GetIntrinsics();
            }
        } catch (Exception e)
        {
            this.pipe.Dispose();
            this.pipe = null;
            Debug.LogError("Failed to start RealSense: " + e.Message);
            return;
        }

        this.stop = false;
        this.thread = new Thread(()=>
        {
            short[] depth = null;

            while (!this.stop)
            {
                using (var frames = this.pipe.WaitForFrames())
                using (var frame  = frames.First(Stream.Depth, Format.Z16))
                {
                    if (null != frame)
                    {
                        var ready = this.DepthReady;
                        if (null != ready)
                        {
                            if (null == depth || depth.Length != frame.DataSize / 2)
                            {
                                depth = new short[frame.DataSize / 2];
                            }

                            Marshal.Copy(frame.Data, depth, 0, frame.DataSize / 2);
                            this.FlipDepth(depth);

                            ready(depth, (long)frame.Timestamp);
                        }
                    }
                }
            }
        });
        this.thread.Start();
    }

    private void StopSensor()
    {
        this.stop = true;

        if (null != this.thread)
        {
            this.thread.Join();
            this.thread = null;
        }

        if (null != this.pipe)
        {
            this.pipe.Stop();
            this.pipe.Dispose();
            this.pipe = null;
        }

        this.Width  = 0;
        this.Height = 0;
    }

    private void FlipDepth(short[] depth)
    {
        for (int i = 0; i < this.Height; i++)
        {
            int off0 = i * this.Width;
            int off1 = off0 + this.Width - 1;
            
            for (; off0 < off1; off0++, off1--)
            {
                var d = depth[off0];
                depth[off0] = depth[off1];
                depth[off1] = d;
            }
        }
    }

    private void CheckOnQuit()
    {
        if (Input.GetKeyUp(KeyCode.Escape) || Input.GetKeyUp(KeyCode.Home))
        {
            #if UNITY_ANDROID && !UNITY_EDITOR
            // Application.Quit causes crash in Android sometimes
            using (var UnityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer"))
            using (var activity = UnityPlayer.GetStatic<AndroidJavaObject>("currentActivity"))
            {
                activity.Call<bool>("moveTaskToBack", true);
            }
            #else
            Application.Quit();
            #endif
        }
    }

    public int Width { get; private set; }
    public int Height { get; private set; }
    public Intrinsics Intrinsics{ get; private set; }

    bool stop;
    private Thread thread;
    private Pipeline pipe;

    public event DepthHandler DepthReady;
}
