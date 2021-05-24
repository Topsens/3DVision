using UnityEngine;
using Topsens;
using Resolution = Topsens.Resolution;

#if UNITY_ANDROID && !UNITY_EDITOR
using Topsens.Unity;
#endif

public delegate void DepthReadyHandler(DepthFrame depth, Orientation orientation);
public delegate void UsersReadyHandler(UsersFrame users, Orientation orientation);

public class SensorBehaviour : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
#if UNITY_ANDROID && !UNITY_EDITOR
        if (!UnityEngine.Android.Permission.HasUserAuthorizedPermission(UnityEngine.Android.Permission.Camera) && AndroidSdkInt() > 26)
        {
            UnityEngine.Android.Permission.RequestUserPermission(UnityEngine.Android.Permission.Camera);
        }
#endif
    }

    // Update is called once per frame
    void Update()
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

    void OnEnable()
    {
    #if UNITY_ANDROID && !UNITY_EDITOR
        Permission.Initialize();
    #else
        int count;

        var err = Sensor.Count(out count);
        if (Error.Ok != err)
        {
            Debug.LogError("Failed to get sensor count: " + GetError(err));
            return;
        }

        if (0 == count)
        {
            Debug.Log("No sensor found");
            return;
        }

        this.StartSensor(0);
    #endif
    }

    void OnDisable()
    {
    #if UNITY_ANDROID && !UNITY_EDITOR
        Permission.Uninitialize();
    #else
        this.StopSensor();
    #endif
    }

    #if UNITY_ANDROID && !UNITY_EDITOR
    void OnApplicationFocus(bool focused)
    {
        if (focused)
        {
            if (null == this.permission)
            {
                if (AndroidSdkInt() > 26 && !UnityEngine.Android.Permission.HasUserAuthorizedPermission(UnityEngine.Android.Permission.Camera))
                {
                    return;
                }

                int count;
                var err = Permission.SensorCount(out count);

                if (Error.Ok != err)
                {
                    Debug.LogError("Failed to get sensor count: " + GetError(err));
                    return;
                }

                if (0 == count)
                {
                    Debug.Log("No sensor found");
                    return;
                }

                this.permission = new Permission();

                var requesting = this.permission.Request(0, permission =>
                {
                    if (permission.IsGranted)
                    {
                        Debug.Log("permission granted");
                        this.StartSensor(permission.GetIdentity());
                    }
                    else
                    {
                        Debug.Log("Permission is denied");
                        this.permission = null;
                    }
                });

                if (!requesting)
                {
                    Debug.LogError("Failed to request permission");
                    this.permission = null;
                }
            }
        }
        else if (null != this.permission && !this.permission.IsRequesting())
        {
            this.StopSensor();
            this.permission.Dispose();
            this.permission = null;
        }
    }

    private static int AndroidSdkInt()
    {
        return new AndroidJavaClass("android.os.Build$VERSION").GetStatic<int>("SDK_INT");
    }
    #endif

    private void StartSensor(int index)
    {
        this.StopSensor();

        this.sensor = new Sensor();
        this.sensor.DepthReady += this.OnDepth;
        this.sensor.UsersReady += this.OnUsers;

        this.orientation = Orientation.Landscape;

        var err = this.sensor.Open(index);
        if (Error.Ok != err)
        {
            Debug.LogError("Failed to open sensor: " + GetError(err));
            return;
        }

        err = this.sensor.SetOrientation(this.orientation);
        if (Error.Ok != err)
        {
            Debug.LogError("Failed to set image orientation: " + GetError(err));
            return;
        }

        // This sample supports only flip mode. To make it work in upflip mode, you need a unflipped stickman model.
        err = this.sensor.SetImageFlipped(true);
        if (Error.Ok != err)
        {
            Debug.LogError("Failed to set image flipping: " + GetError(err));
            return;
        }

        err = this.sensor.SetDepthAligned(false);
        if (Error.Ok != err)
        {
            Debug.LogError("Failed to set depth alignment: " + GetError(err));
            return;
        }

        err = this.sensor.Start(SensorBehaviour.Resolution, SensorBehaviour.Resolution, true);
        if (Error.Ok != err)
        {
            Debug.LogError("Failed to start sensor: " + GetError(err));
        }
    }

    private void StopSensor()
    {
        if (null != this.sensor)
        {
            this.sensor.Stop();
            this.sensor.Close();
            this.sensor.DepthReady -= this.OnDepth;
            this.sensor.UsersReady -= this.OnUsers;
            this.sensor = null;
        }
    }

    private void OnDepth(Sensor sensor, DepthFrame depth, Error err)
    {
        if (Error.Ok == err)
        {
            var ready = this.DepthReady;
            if (null != ready)
            {
                ready(depth, this.orientation);
            }
        }
    }

    private void OnUsers(Sensor sensor, UsersFrame users, Error err)
    {
        if (Error.Ok == err)
        {
            var ready = this.UsersReady;
            if (null != ready)
            {
                ready(users, this.orientation);
            }
        }
    }

    private static string GetError(Error err)
    {
        switch (err)
        {
            case Error.Ok:
                return "No error";

            case Error.Timeout:
                return "Timeout";

            case Error.InvalidParameter:
                return "Invalid parameter";

            case Error.InvalidOperation:
                return "Invalid operation";

            case Error.Initialization:
                return "Not initialized/Initialization failed";

            case Error.NotSupported:
                return "Not supported";

            case Error.NotFound:
                return "Target not found";

            case Error.Closed:
                return "Object is closed";

            case Error.DeviceClosed:
                return "Device closed";

            case Error.DevicePermission:
                return "Device permission not granted";

            case Error.StreamDisabled:
                return "Stream disabled";

            case Error.StreamContent:
                return "Stream content error";

            case Error.StreamCodec:
                return "Stream encoding error";

            case Error.StreamEnd:
                return "Stream end reached";

            case Error.StreamIO:
                return "Stream IO failure";

            case Error.Resource:
                return "Failed to load resource";

            default:
                return string.Format("{0}", (int)err);
        }
    }
    
    public static Resolution Resolution
    {
        get
        {
#if UNITY_ANDROID && !UNITY_EDITOR
            return Resolution.QVGA;
#else
            return Resolution.VGA;
#endif
        }
    }

    public event DepthReadyHandler DepthReady;
    public event UsersReadyHandler UsersReady;

    private Sensor sensor;
    private Resolution resolution;
    private Orientation orientation;

    #if UNITY_ANDROID && !UNITY_EDITOR
    private Permission permission;
    #endif
}