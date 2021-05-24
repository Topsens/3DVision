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

using Intel.RealSense;
using Topsens;
using Orientation = Topsens.Orientation;

namespace TopsensRealSenseWpf
{
    /// <summary>
    /// Interaction logic for Panel.xaml
    /// </summary>
    public partial class Panel : UserControl
    {
        public Panel()
        {
            InitializeComponent();

            this.context = new Context();
            this.context.OnDevicesChanged += this.OnDevicesChanged;

            this.Refresh();
        }

        public string Serial { get; private set; }

        public ResolutionItem Resolution
        {
            get
            {
                return (ResolutionItem)this.resolution.SelectedItem;
            }
        }

        public bool Flip()
        {
            return this.flip.IsChecked ?? false;
        }

        public Orientation Orientation()
        {
            if (this.land.IsChecked ?? false)
            {
                return Topsens.Orientation.Landscape;
            }

            if (this.clock.IsChecked ?? false)
            {
                return Topsens.Orientation.PortraitClockwise;
            }

            if (this.antic.IsChecked ?? false)
            {
                return Topsens.Orientation.PortraitAntiClockwise;
            }

            return Topsens.Orientation.Aerial;
        }

        public void Enable()
        {
            this.resolution.IsEnabled = true;
            this.device.IsEnabled = true;
            this.flipGroup.IsEnabled = true;
            this.orientGroup.IsEnabled = true;
            this.start.IsEnabled = true;
            this.stop.IsEnabled = false;
        }

        public void Disable()
        {
            this.resolution.IsEnabled = false;
            this.device.IsEnabled = false;
            this.flipGroup.IsEnabled = false;
            this.orientGroup.IsEnabled = false;
            this.start.IsEnabled = false;
            this.stop.IsEnabled = true;
        }

        private void OnDevicesChanged(DeviceList removed, DeviceList added)
        {
            this.Refresh();
        }

        private void OnDeviceChanged(object sender, EventArgs args)
        {
            if (this.device.SelectedIndex < 0)
            {
                return;
            }

            if (this.device.SelectedIndex < this.context.Devices.Count)
            {
                var device = this.context.Devices[this.device.SelectedIndex];
                var serial = device.Info.GetInfo(CameraInfo.SerialNumber);

                if (string.Equals(this.Serial, serial))
                {
                    return;
                }

                this.Serial = serial;
                this.resolution.Items.Clear();

                var sensors = device.QuerySensors();
                foreach (var sensor in sensors)
                {
                    var streams = sensor.StreamProfiles;
                    foreach (var stream in streams)
                    {
                        if (Stream.Depth == stream.Stream &&
                            Format.Z16 == stream.Format &&
                            30 == stream.Framerate)
                        {
                            var prof = stream.As<VideoStreamProfile>();
                            this.resolution.Items.Add(new ResolutionItem(prof.Width, prof.Height));

                            if (640 == prof.Width && 480 == prof.Height)
                            {
                                this.resolution.SelectedIndex = this.resolution.Items.Count - 1;
                            }
                        }
                    }
                }
            }
        }

        private void Refresh()
        {
            this.Dispatcher.BeginInvoke(new Action(()=>
            {
                this.device.Items.Clear();

                if (this.context.Devices.Count > 0)
                {
                    foreach (var device in this.context.Devices)
                    {
                        var name = device.Info.GetInfo(CameraInfo.Name);
                        this.device.Items.Add(name);

                        var serial = device.Info.GetInfo(CameraInfo.SerialNumber);
                        if (string.Equals(this.Serial, serial))
                        {
                            this.device.SelectedIndex = this.device.Items.Count - 1;
                        }
                    }

                    if (this.device.SelectedIndex < 0)
                    {
                        this.device.SelectedIndex = 0;
                    }
                }
                else
                {
                    this.resolution.Items.Clear();
                    this.Serial = null;
                }
            }));
        }

        private Context context;
    }
}