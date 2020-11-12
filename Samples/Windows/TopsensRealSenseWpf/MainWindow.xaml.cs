using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using Topsens;
using Intel.RealSense;

namespace TopsensRealSenseWpf
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this.Resize(640, 480);

            this.panel.start.Click += this.OnStart;
            this.panel.stop.Click += this.OnStop;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            this.OnStop(null, null);
            this.panel.start.Click -= this.OnStart;
            this.panel.stop.Click -= this.OnStop;
        }

        private void OnStart(object sender, RoutedEventArgs args)
        {
            var serial = this.panel.Serial;
            if (null == serial)
            {
                this.Status("No RealSense camera connected");
                return;
            }

            var res = this.panel.Resolution;
            if (null == res)
            {
                this.Status("Resolution not selected");
                return;
            }

            this.Resize(res.Width, res.Height);

            var flip   = this.panel.Flip();
            var orient = this.panel.Orientation();

            this.stop = false;
            this.thread = new Thread(()=>
            {
                var cfg = new Config();
                cfg.EnableStream(Stream.Depth, res.Width, res.Height, Format.Z16, 30);
                cfg.EnableDevice(serial);

                PipelineProfile prof;

                using (var pipe = new Pipeline())
                using (var people = new People())
                {
                    try
                    {
                        prof = pipe.Start(cfg);

                    } catch (Exception e)
                    {
                        this.Status("Failed to start RealSense: " + e.Message);
                        return;
                    }

                    Intrinsics intr;
                    using (var stream = prof.GetStream<VideoStreamProfile>(Stream.Depth))
                    {
                        intr = prof.GetStream<VideoStreamProfile>(Stream.Depth).GetIntrinsics();
                    }

                    var err = people.SetOrientation(orient);
                    if (Error.Ok != err)
                    {
                        this.Status("Failed to set image orientation: " + this.GetError(err));
                        pipe.Stop();
                        return;
                    }

                    err = people.SetImageFlipped(flip);
                    if (Error.Ok != err)
                    {
                        this.Status("Failed to set image flip: " + this.GetError(err));
                        pipe.Stop();
                        return;
                    }

                    err = people.Initialize(res.Width, res.Height, intr.fx, intr.fy, intr.ppx, intr.ppy);
                    if (Error.Ok != err)
                    {
                        this.Status("Failed to initialize people: " + this.GetError(err));
                        pipe.Stop();
                        return;
                    }

                    short[] depth = new short[res.Width * res.Height];

                    UsersFrame users = new UsersFrame();

                    while (!this.stop)
                    {
                        using (var frames = pipe.WaitForFrames())
                        using (var frame  = frames.First(Stream.Depth, Format.Z16))
                        {
                            if (null != frame)
                            {
                                Marshal.Copy(frame.Data, depth, 0, depth.Length);
                        
                                if (flip)
                                {
                                    for (int i = 0; i < res.Height; i++)
                                    {
                                        var off0 = i * res.Width;
                                        var off1 = off0 + res.Width - 1;

                                        for (; off0 < off1; off0++, off1--)
                                        {
                                            var d = depth[off0];
                                            depth[off0] = depth[off1];
                                            depth[off1] = d;
                                        }
                                    }
                                }

                                this.dview.Draw(depth, res.Width, res.Height);

                                err = people.Detect(depth, res.Width, res.Height, (long)frame.Timestamp, users);
                                if (Error.Ok == err)
                                {
                                    this.uview.Draw(users);
                                    this.Status("");
                                }
                                else
                                {
                                    this.Status("Failed to detect: " + this.GetError(err));
                                }
                            }
                        }
                    }

                    people.Uninitialize();
                    pipe.Stop();
                }
            });

            this.thread.Start();
            this.panel.Disable();
        }

        private void OnStop(object sender, RoutedEventArgs args)
        {
            this.stop = true;
            if (null != this.thread)
            {
                this.thread.Join();
                this.thread = null;
            }

            this.panel.Enable();
        }

        private void Resize(int w, int h)
        {
            this.dview.Width  = w;
            this.dview.Height = h;
        }

        private void Status(String info)
        {
            this.Dispatcher.BeginInvoke(new Action(()=>
            {
                this.status.Text = info;
            }));
        }

        private string GetError(Error err)
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

        private bool stop;
        private Thread thread;
    }
}
