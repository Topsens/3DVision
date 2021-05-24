using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Topsens;
using Topsens.Unity;

using Vector3 = UnityEngine.Vector3;

public class StickmanBehaviour : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        this.torso    = this.gameObject.transform.GetChild(0);
        this.head     = this.torso.GetChild(0);
        this.larm     = this.torso.GetChild(1);
        this.rarm     = this.torso.GetChild(2);
        this.lthigh   = this.torso.GetChild(3);
        this.rthigh   = this.torso.GetChild(4);
        this.lforearm = this.larm.GetChild(0);
        this.rforearm = this.rarm.GetChild(0);
        this.lleg     = this.lthigh.GetChild(0);
        this.rleg     = this.rthigh.GetChild(0);
    }

    // Update is called once per frame
    void Update()
    {
        lock (this)
        {
            if (this.skeleton.Tracked)
            {
                var r = this.gameObject.transform.rotation;

                if (!this.ground.IsNaN())
                {
                    // 修正地面倾斜度
                    switch (this.orientation)
                    {
                        case Orientation.Landscape:
                        {
                            r *= Quaternion.FromToRotation(this.ground.Y > 0.0f ? new Vector3( this.ground.X,  this.ground.Y,  this.ground.Z)
                                                                                : new Vector3(-this.ground.X, -this.ground.Y, -this.ground.Z),
                                                                                  new Vector3(0.0f, 1.0f, 0.0f));
                            break;
                        }

                        case Orientation.PortraitClockwise:
                        {
                            r *= Quaternion.FromToRotation(this.ground.X < 0.0f ? new Vector3( this.ground.X,  this.ground.Y,  this.ground.Z)
                                                                                : new Vector3(-this.ground.X, -this.ground.Y, -this.ground.Z),
                                                                                  new Vector3(-1.0f, 0.0f, 0.0f));
                            break;
                        }

                        case Orientation.PortraitAntiClockwise:
                        {
                            r *= Quaternion.FromToRotation(this.ground.X > 0.0f ? new Vector3( this.ground.X,  this.ground.Y,  this.ground.Z)
                                                                                : new Vector3(-this.ground.X, -this.ground.Y, -this.ground.Z),
                                                                                  new Vector3(1.0f, 0.0f, 0.0f));
                            break;

                        }

                        default: break;
                    }
                }

                var joints = this.skeleton.Joints;
                this.torso.rotation     = r * joints[JointIndex.Torso].Rotation.Quaternion();
                this.head.rotation      = r * joints[JointIndex.Head].Rotation.Quaternion();
                this.larm.rotation      = r * joints[JointIndex.LElbow].Rotation.Quaternion();
                this.rarm.rotation      = r * joints[JointIndex.RElbow].Rotation.Quaternion();
                this.lforearm.rotation  = r * joints[JointIndex.LHand].Rotation.Quaternion();
                this.rforearm.rotation  = r * joints[JointIndex.RHand].Rotation.Quaternion();
                this.lthigh.rotation    = r * joints[JointIndex.LKnee].Rotation.Quaternion();
                this.rthigh.rotation    = r * joints[JointIndex.RKnee].Rotation.Quaternion();
                this.lleg.rotation      = r * joints[JointIndex.LFoot].Rotation.Quaternion();
                this.rleg.rotation      = r * joints[JointIndex.RFoot].Rotation.Quaternion();
            }
            else
            {
                var r = this.gameObject.transform.rotation * Quaternion.identity;
                this.torso.rotation     = r;
                this.head.rotation      = r;
                this.larm.rotation      = r;
                this.rarm.rotation      = r;
                this.lforearm.rotation  = r;
                this.rforearm.rotation  = r;
                this.lthigh.rotation    = r;
                this.rthigh.rotation    = r;
                this.lleg.rotation      = r;
                this.rleg.rotation      = r;
            }
        }
    }

    void OnEnable()
    {
        if (null != Camera.main)
        {
            this.sensor = Camera.main.GetComponent<SensorBehaviour>();
            if (null != this.sensor)
            {
                this.sensor.UsersReady += this.OnUsers;
            }
        }
    }

    void OnDisable()
    {
        if (null != this.sensor)
        {
            this.sensor.UsersReady -= this.OnUsers;
            this.sensor = null;
        }
    }

    private void OnUsers(UsersFrame frame, Orientation orientation)
    {
        lock (this)
        {
            this.ground = frame.GroundPlane;
            this.orientation = orientation;

            if (frame.UserCount > 0)
            {
                frame.Skeletons[0].CopyTo(this.skeleton);
            }
            else
            {
                this.skeleton.Tracked = false;
            }
        }
    }

    private Topsens.Vector4 ground;
    private SensorBehaviour sensor;
    private Skeleton skeleton = new Skeleton();
    private Orientation orientation;

    private Transform torso;
    private Transform head;
    private Transform larm;
    private Transform rarm;
    private Transform lforearm;
    private Transform rforearm;
    private Transform lthigh;
    private Transform rthigh;
    private Transform lleg;
    private Transform rleg;
}