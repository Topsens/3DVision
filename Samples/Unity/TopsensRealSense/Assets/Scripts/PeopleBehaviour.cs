using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Topsens;

public delegate void UsersHandler(UsersFrame users);

public class PeopleBehaviour : MonoBehaviour
{
    void Awake()
    {
        #if UNITY_ANDROID && !UNITY_EDITOR
        Topsens.Unity.Resources.Initialize();
        #endif
        Error err = People.Preload(new int[]{ 0, 1 });
        if (Error.Ok != err)
        {
            Debug.LogError("Preload failed: " + err);
        }
    }

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        if (null == this.people)
        {
            if (null != this.sensor && 0 != this.sensor.Width)
            {
                var people = new People();

                var err = people.SetOrientation(Orientation);
                if (Error.Ok != err)
                {
                    Debug.LogError("Failed to set image orientation: " + err);
                    return;
                }

                // This sample supports only flip mode. To make it work in upflip mode, you need a unflipped stickman model.
                err = people.SetImageFlipped(true);
                if (Error.Ok != err)
                {
                    Debug.LogError("Failed to set image flip: " + err);
                    return;
                }

                var i = this.sensor.Intrinsics;
                err = people.Initialize(this.sensor.Width, this.sensor.Height, i.fx, i.fy, i.ppx, i.ppy);
                if (Error.Ok != err)
                {
                    Debug.LogError("Failed to initialize people: " + err);
                    return;
                }

                this.people = people;
                this.sensor.DepthReady += this.OnDepth;
            }
        }
    }

    void OnEnable()
    {
        if (null != Camera.main)
        {
            this.sensor = Camera.main.GetComponent<SensorBehaviour>();
        }
    }

    void OnDisable()
    {
        if (null != this.sensor)
        {
            this.sensor.DepthReady -= this.OnDepth;
            this.sensor = null;
        }

        if (null != this.people)
        {
            this.people.Uninitialize();
            this.people.Dispose();
            this.people = null;
        }
    }

    private void OnDepth(short[] depth, long timestamp)
    {
        if (null != this.people)
        {
            if (null == this.users)
            {
                this.users = new UsersFrame();
            }

            var len = depth.Length;
            var err = this.people.Detect(depth, this.sensor.Width, this.sensor.Height, timestamp, this.users);
            if (Error.Ok != err)
            {
                Debug.LogError("Failed to detect: " + err);
                return;
            }

            if (null != this.UsersReady)
            {
                this.UsersReady(this.users);
            }
        }
    }

    public event UsersHandler UsersReady;

    public static Orientation Orientation { get{ return Orientation.Landscape; }}

    private People people;
    private UsersFrame users;
    private SensorBehaviour sensor;
}
