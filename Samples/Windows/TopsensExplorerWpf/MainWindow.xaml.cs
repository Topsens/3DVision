﻿using System;
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

namespace TopsensExplorerWpf
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            this.InitPalette();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this.RefreshDevices();

            this.panel.refresh.Click += this.OnRefresh;
            this.panel.start.Click += this.OnStart;
            this.panel.stop.Click += this.OnStop;

            // Preload resources to minimize sensor start delay.
            // If skeleton is not needed, this can be omitted.
            Sensor.Preload(new int[]{ 0, 1 });
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            this.OnStop(null, null);
            this.panel.refresh.Click -= this.OnRefresh;
            this.panel.start.Click -= this.OnStart;
            this.panel.stop.Click -= this.OnStop;
        }

        private void OnRefresh(object sender, RoutedEventArgs args)
        {
            this.RefreshDevices();
        }

        private void OnStart(object sender, RoutedEventArgs args)
        {
            this.sensor = new Sensor();

            var err = this.sensor.Open(this.panel.devices.SelectedIndex);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to open sensor: " + this.GetError(err));
                this.RefreshDevices();
                return;
            }

            err = this.sensor.SetOrientation(this.panel.Orientation());
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to set sensor orientation: " + this.GetError(err));
                return;
            }

            err = this.sensor.SetDepthAligned(this.panel.align.IsChecked ?? false);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to set depth alignmnent: " + this.GetError(err));
                return;
            }

            err = this.sensor.SetImageFlipped(this.panel.flip.IsChecked ?? false);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to set image flipped: " + this.GetError(err));
                return;
            }

            err = this.sensor.SetRecording(this.panel.record.IsChecked ?? false);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to set stream recording: " + this.GetError(err));
                return;
            }

            err = this.sensor.Start((Resolution)this.panel.cres.SelectedIndex, (Resolution)this.panel.dres.SelectedIndex, this.panel.users.IsChecked ?? false);
            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to start sensor: " + this.GetError(err));
                return;
            }

            this.groundPlane = Vector4.NaN;

            this.sensor.ColorReady += this.OnColorReady;
            this.sensor.DepthReady += this.OnDepthReady;
            this.sensor.UsersReady += this.OnUsersReady;

            this.DisableOptions();
            this.panel.refresh.IsEnabled = false;
            this.panel.stop.IsEnabled = true;
        }

        private void OnStop(object sender, RoutedEventArgs args)
        {
            if (null != this.sensor)
            {
                this.sensor.Stop();
                this.sensor.Close();

                this.sensor.ColorReady -= this.OnColorReady;
                this.sensor.DepthReady -= this.OnDepthReady;
                this.sensor.UsersReady -= this.OnUsersReady;
                this.sensor = null;
            }

            this.RefreshDevices();
            this.panel.refresh.IsEnabled = true;
            this.panel.stop.IsEnabled = false;
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

            lock (this.cframe)
            {
                this.cframe.Swap(frame);
            }

            this.Dispatcher.BeginInvoke(new Action(()=>
            {
                lock (this.cframe)
                {
                    if (null == this.cbitmap || this.cbitmap.PixelWidth != this.cframe.Width || this.cbitmap.PixelHeight != this.cframe.Height)
                    {
                        this.cbitmap = new WriteableBitmap(this.cframe.Width, this.cframe.Height, 96.0, 96.0, PixelFormats.Bgra32, null);
                        this.cimage.Source = this.cbitmap;
                    }

                    this.cbitmap.WritePixels(new Int32Rect(0, 0, this.cframe.Width, this.cframe.Height), this.cframe.Pixels, this.cframe.Width * 4, 0);
                }
            }), DispatcherPriority.Render);
        }

        private void OnDepthReady(Sensor sensor, DepthFrame frame, Error err)
        {
            if (Error.Ok != err)
            {
                return;
            }

            if (null == this.dframe)
            {
                this.dframe = new DepthFrame();
            }

            lock (this.dframe)
            {
                this.dframe.Swap(frame);
            }

            this.Dispatcher.BeginInvoke(new Action(()=>
            {
                int w, h;

                lock (this.dframe)
                {
                    w = this.dframe.Width;
                    h = this.dframe.Height;

                    if (null == this.dpixels || this.dpixels.Length != this.dframe.Pixels.Length)
                    {
                        this.dpixels = new int[this.dframe.Pixels.Length];
                    }

                    if ((this.panel.groundYes.IsChecked ?? false) && !this.groundPlane.IsNaN())
                    {
                        this.dframe.ToPointCloud(ref this.cloud);

                        for (int i = 0; i < this.dpixels.Length; i++)
                        {
                            var d = this.dframe.Pixels[i];
                            if (d < this.palette.Length)
                            {
                                var c = this.cloud.Pixels[i];
                                if (Math.Abs(c.X * this.groundPlane.X + c.Y * this.groundPlane.Y + c.Z * this.groundPlane.Z + this.groundPlane.W) < 0.035f)
                                {
                                    unchecked { this.dpixels[i] = (int)0xFF808080; }
                                }
                                else
                                {
                                    this.dpixels[i] = this.palette[d];
                                }
                            }
                        }
                    }
                    else
                    {
                        for (int i = 0; i < this.dpixels.Length; i++)
                        {
                            var d = this.dframe.Pixels[i];
                            unchecked { this.dpixels[i] = d < this.palette.Length ? this.palette[d] : (int)0xFFFF0000; }
                        }
                    }
                }

                if (null == this.dbitmap || this.dbitmap.PixelWidth != w || this.dbitmap.PixelHeight != h)
                {
                    this.dbitmap = new WriteableBitmap(w, h, 96.0, 96.0, PixelFormats.Bgra32, null);
                    this.dimage.Source = this.dbitmap;
                }

                this.dbitmap.WritePixels(new Int32Rect(0, 0, w, h), this.dpixels, w * 4, 0);

            }), DispatcherPriority.Render);
        }

        private void OnUsersReady(Sensor sensor, UsersFrame frame, Error err)
        {
            if (Error.Ok != err)
            {
                return;
            }

            this.groundPlane = frame.GroundPlane;

            if (null == this.uframe)
            {
                this.uframe = new UsersFrame();
            }

            lock (this.uframe)
            {
                frame.CopyTo(this.uframe);
            }

            this.Dispatcher.BeginInvoke(new Action(()=>
            {
                if (null == this.userPainter)
                {
                    this.userPainter = new UserPainter(this.canvas);
                }

                lock (this.uframe)
                {
                    this.userPainter.Paint(this.uframe);
                }
            }), DispatcherPriority.Render);
        }

        private void InitPalette()
        {
            this.palette = new int[0x1000];

            for (uint d = 0; d < 0x190; d++)
            {
                unchecked
                {
                    this.palette[d] = (int)0xFF000000;
                }
            }

            for (uint d = 0x190; d < 0x400; d++)
            {
                uint r = 0xFF - 0xFF * (d - 0x190) / (0x400 - 0x190);
                uint g = 0;
                uint b = 0xFF;

                this.palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }

            for (uint d = 0x400; d < 0x600; d++)
            {
                uint r = 0;
                uint g = 0xFF * (d - 0x400) / (0x600 - 0x400);
                uint b = 0xFF;

                this.palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }

            for (uint d = 0x600; d < 0x800; d++)
            {
                uint r = 0;
                uint g = 0xFF;
                uint b = 0xFF - 0xFF * (d - 0x600) / (0x800 - 0x600);

                this.palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }

            for (uint d = 0x800; d < 0xC00; d++)
            {
                uint r = 0xFF * (d - 0x800) / (0xC00 - 0x800);
                uint g = 0xFF;
                uint b = 0;

                this.palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }

            for (uint d = 0xC00; d < 0x1000; d++)
            {
                uint r = 0xFF;
                uint g = 0xFF - 0xFF * (d - 0xC00) / (0x1000 - 0xC00);
                uint b = 0;

                this.palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }
        }

        private void RefreshDevices()
        {
            int count;
            var err = Sensor.Count(out count);

            if (Error.Ok != err)
            {
                MessageBox.Show("Failed to get sensor count: " + this.GetError(err));
                return;
            }

            this.panel.devices.Items.Clear();

            if (count > 0)
            {
                for (int i = 0; i < count; i++)
                {
                    this.panel.devices.Items.Add(i.ToString());
                }

                this.panel.devices.SelectedIndex = 0;
                this.EnableOptions();
            }
            else
            {
                this.DisableOptions();
            }
        }

        private void EnableOptions()
        {
            this.panel.devices.IsEnabled = true;
            this.panel.cres.IsEnabled = true;
            this.panel.dres.IsEnabled = true;
            this.panel.start.IsEnabled = true;
            this.panel.usersGroup.IsEnabled = true;
            this.panel.alignGroup.IsEnabled = true;
            this.panel.flipGroup.IsEnabled = true;
        }

        private void DisableOptions()
        {
            this.panel.devices.IsEnabled = false;
            this.panel.cres.IsEnabled = false;
            this.panel.dres.IsEnabled = false;
            this.panel.start.IsEnabled = false;
            this.panel.usersGroup.IsEnabled = false;
            this.panel.alignGroup.IsEnabled = false;
            this.panel.flipGroup.IsEnabled = false;
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

        Vector4 groundPlane;

        private int[] dpixels;
        private int[] palette;

        private Sensor sensor;
        private ColorFrame cframe;
        private DepthFrame dframe;
        private UsersFrame uframe;
        private CloudFrame cloud;

        private WriteableBitmap cbitmap;
        private WriteableBitmap dbitmap;

        private UserPainter userPainter;
    }
}