using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
using System.Windows.Threading;
using Topsens;
using Topsens.Toolkit;

namespace TopsensUserExtractorWpf
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
            Resolution res = Resolution.VGA;

            this.extractor = new UserExtractor();

            var err = this.extractor.Initialize(res);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to initialize extractor: " + this.GetError(err));
                return;
            }

            int count;

            err = Sensor.Count(out count);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to get sensor count: " + this.GetError(err));
                return;
            }

            if (0 == count)
            {
                MessageBox.Show("No sensor found");
                return;
            }

            this.sensor = new Sensor();

            err = this.sensor.Open(0);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to open first sensor: " + this.GetError(err));
                return;
            }

            err = this.sensor.SetDepthAligned(true);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to set depth alignment: " + this.GetError(err));
                return;
            }

            err = this.sensor.SetMaskEnabled(true);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to enable user mask: " + this.GetError(err));
                return;
            }

            err = this.sensor.Start(res, res, true);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to start sensor: " + this.GetError(err));
                return;
            }

            this.sensor.ColorReady += this.OnColorReady;
            this.sensor.UsersReady += this.OnUsersReady;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (null != this.sensor)
            {
                this.sensor.Stop();
                this.sensor.Close();

                this.sensor.ColorReady -= this.OnColorReady;
                this.sensor.UsersReady -= this.OnUsersReady;
            }

            if (null != this.extractor)
            {
                this.extractor.Dispose();
            }
        }

        private void OnColorReady(Sensor sensor, ColorFrame frame, Error err)
        {
            if (Error.Ok != err)
            {
                return;
            }

            if (null == this.cframe)
            {
                this.cframe = new ColorFrame();
            }

            this.cframe.Swap(frame);
        }

        private void OnUsersReady(Sensor sensor, UsersFrame frame, Error err)
        {
            if (Error.Ok != err)
            {
                return;
            }

            if (null == this.cframe)
            {
                return;
            }

            this.w = this.cframe.Width;
            this.h = this.cframe.Height;

            lock (this.extractLock)
            {
                err = this.extractor.Extract(this.cframe, frame, ref this.extracted);
            }

            if (Error.Ok == err)
            {
                this.Dispatcher.BeginInvoke(new Action(()=>
                {
                    lock (this.extractLock)
                    {
                        if (null == this.bitmap || this.bitmap.PixelWidth != this.w || this.bitmap.PixelHeight != this.h)
                        {
                            this.bitmap = new WriteableBitmap(this.w, this.h, 96.0, 96.0, PixelFormats.Bgra32, null);
                            this.foreground.Width  = this.background.ActualWidth;
                            this.foreground.Height = this.background.ActualHeight;
                            this.foreground.Source = this.bitmap;
                        }

                        this.bitmap.WritePixels(new Int32Rect(0, 0, this.w, this.h), extracted, this.w * 4, 0);
                    }
                }), DispatcherPriority.Render);
            }
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

        private Sensor sensor;
        private ColorFrame cframe;
        private UserExtractor extractor;

        private WriteableBitmap bitmap;

        private int w, h;
        private int[] extracted;
        private object extractLock = new object();
    }
}
