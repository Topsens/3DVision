using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using Topsens;
using Orientation = Topsens.Orientation;

namespace TopsensExplorerWpf
{
    class UserPainter
    {
        public UserPainter(Canvas canvas)
        {
            this.canvas = canvas;
        }

        public void Paint(UsersFrame frame)
        {
            this.frame = frame;

            int bIndex = 0;
            int jIndex = 0;

            for (int i = 0;  i < frame.UserCount; i++)
            {
                var joints = frame.Skeletons[i].Joints;

                this.PaintBone(joints[JointIndex.LShoulder], joints[JointIndex.LElbow], ref bIndex);
                this.PaintBone(joints[JointIndex.RShoulder], joints[JointIndex.RElbow], ref bIndex);
                this.PaintBone(joints[JointIndex.LShoulder], joints[JointIndex.RWaist], ref bIndex);
                this.PaintBone(joints[JointIndex.RShoulder], joints[JointIndex.LWaist], ref bIndex);
                this.PaintBone(joints[JointIndex.LElbow],    joints[JointIndex.LHand],  ref bIndex);
                this.PaintBone(joints[JointIndex.RElbow],    joints[JointIndex.RHand],  ref bIndex);

                if (Orientation.Aerial != this.Orientation)
                {
                    this.PaintBone(joints[JointIndex.Head],      joints[JointIndex.Neck],   ref bIndex);
                    this.PaintBone(joints[JointIndex.LShoulder], joints[JointIndex.Neck],   ref bIndex);
                    this.PaintBone(joints[JointIndex.RShoulder], joints[JointIndex.Neck],   ref bIndex);
                    this.PaintBone(joints[JointIndex.LWaist],    joints[JointIndex.LKnee],  ref bIndex);
                    this.PaintBone(joints[JointIndex.RWaist],    joints[JointIndex.RKnee],  ref bIndex);
                    this.PaintBone(joints[JointIndex.LKnee],     joints[JointIndex.LFoot],  ref bIndex);
                    this.PaintBone(joints[JointIndex.RKnee],     joints[JointIndex.RFoot],  ref bIndex);
                    this.PaintBone(joints[JointIndex.LWaist],    joints[JointIndex.RWaist], ref bIndex);
                }

                this.PaintJoint(joints[JointIndex.Head],      ref jIndex);
                this.PaintJoint(joints[JointIndex.LShoulder], ref jIndex);
                this.PaintJoint(joints[JointIndex.RShoulder], ref jIndex);
                this.PaintJoint(joints[JointIndex.LElbow],    ref jIndex);
                this.PaintJoint(joints[JointIndex.RElbow],    ref jIndex);
                this.PaintJoint(joints[JointIndex.LHand],     ref jIndex);
                this.PaintJoint(joints[JointIndex.RHand],     ref jIndex);

                if (Orientation.Aerial != this.Orientation)
                {
                    this.PaintJoint(joints[JointIndex.Neck],      ref jIndex);
                    this.PaintJoint(joints[JointIndex.LWaist],    ref jIndex);
                    this.PaintJoint(joints[JointIndex.RWaist],    ref jIndex);
                    this.PaintJoint(joints[JointIndex.LKnee],     ref jIndex);
                    this.PaintJoint(joints[JointIndex.RKnee],     ref jIndex);
                    this.PaintJoint(joints[JointIndex.LFoot],     ref jIndex);
                    this.PaintJoint(joints[JointIndex.RFoot],     ref jIndex);
                }
            }

            while (bIndex < this.bones.Count())
            {
                this.bones[bIndex++].Visibility = Visibility.Collapsed;
            }

            while (jIndex < this.joints.Count())
            {
                this.joints[jIndex++].Visibility = Visibility.Collapsed;
            }
        }

        private void PaintBone(Joint beg, Joint end, ref int index)
        {
            var beg2d = this.frame.MapTo2D(beg.Position);
            var end2d = this.frame.MapTo2D(end.Position);

            if (beg2d.IsNaN() || end2d.IsNaN())
            {
                return;
            }

            if (this.bones.Count() == index)
            {
                var l = new Line();
                l.Stroke = Brushes.LightGray;
                l.StrokeThickness = 6;

                this.bones.Add(l);
                this.canvas.Children.Add(l);
            }

            var b = this.bones[index++];
            b.Visibility = Visibility.Visible;
            b.X1 = beg2d.X * this.Ratio;
            b.Y1 = beg2d.Y * this.Ratio;
            b.X2 = end2d.X * this.Ratio;
            b.Y2 = end2d.Y * this.Ratio;
        }

        private void PaintJoint(Joint joint, ref int index)
        {
            var pos2d = this.frame.MapTo2D(joint.Position);

            if (pos2d.IsNaN())
            {
                return;
            }

            if (this.joints.Count() == index)
            {
                var e = new Ellipse();
                e.Fill   = Brushes.LightGray;
                e.Width  = 16;
                e.Height = 16;

                this.joints.Add(e);
                this.canvas.Children.Add(e);
            }

            var j = this.joints[index++];
            j.Visibility = Visibility.Visible;

            Canvas.SetLeft(j, pos2d.X  * this.Ratio - j.Width * 0.5f);
            Canvas.SetTop(j, pos2d.Y  * this.Ratio - j.Height * 0.5f);
        }

        public double Ratio { get; set; }

        public Orientation Orientation { get; set; }

        private UsersFrame frame;

        private Canvas canvas;
        private List<Line> bones = new List<Line>();
        private List<Ellipse> joints = new List<Ellipse>();
    }
}