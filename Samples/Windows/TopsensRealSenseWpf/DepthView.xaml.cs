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
using Orientation = Topsens.Orientation;

namespace TopsensRealSenseWpf
{
    /// <summary>
    /// Interaction logic for DepthView.xaml
    /// </summary>
    public partial class DepthView : UserControl
    {
        public DepthView()
        {
            InitializeComponent();
            InitPalette();
        }

        public void Draw(short[] depth, int width, int height, Orientation orientation)
        {
            var w = this.image.ActualWidth;
            var h = this.image.ActualHeight;
            if (null == depth || width <= 0 || height <= 0)
            {
                return;
            }

            if (null == this.pixels || this.pixels.Length != width * height)
            {
                this.pixels = new int[width * height];
            }

            lock (this.pixels)
            {
                for (int i = 0; i < this.pixels.Length; i++)
                {
                    var d = (ushort)depth[i];
                    unchecked { this.pixels[i] = d < palette.Length ? palette[d] : (int)0xFFFF0000; }
                }
            }

            this.Dispatcher.BeginInvoke(new Action(()=>
            {
                if (null == this.bitmap || this.bitmap.PixelWidth != width || this.bitmap.PixelHeight != height)
                {
                    this.bitmap = new WriteableBitmap(width, height, 96.0, 96.0, PixelFormats.Bgra32, null);
                    this.image.Source = this.bitmap;
                }

                lock (this.pixels)
                {
                    if (Orientation.Landscape == orientation)
                    {
                        this.LayoutTransform = new RotateTransform(0.0);
                    }
                    else if (Orientation.PortraitClockwise == orientation)
                    {
                        this.LayoutTransform = new RotateTransform(90.0);
                    }
                    else
                    {
                        this.LayoutTransform = new RotateTransform(-90.0);
                    }

                    this.bitmap.WritePixels(new Int32Rect(0, 0, width, height), this.pixels, width * 4, 0);
                }
            }), DispatcherPriority.Render);
        }

        private static void InitPalette()
        {
            if (null != palette)
            {
                return;
            }

            palette = new int[0x1000];

            for (uint d = 0; d < 0x190; d++)
            {
                unchecked
                {
                    palette[d] = (int)0xFF000000;
                }
            }

            for (uint d = 0x190; d < 0x400; d++)
            {
                uint r = 0xFF - 0xFF * (d - 0x190) / (0x400 - 0x190);
                uint g = 0;
                uint b = 0xFF;

                palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }

            for (uint d = 0x400; d < 0x600; d++)
            {
                uint r = 0;
                uint g = 0xFF * (d - 0x400) / (0x600 - 0x400);
                uint b = 0xFF;

                palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }

            for (uint d = 0x600; d < 0x800; d++)
            {
                uint r = 0;
                uint g = 0xFF;
                uint b = 0xFF - 0xFF * (d - 0x600) / (0x800 - 0x600);

                palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }

            for (uint d = 0x800; d < 0xC00; d++)
            {
                uint r = 0xFF * (d - 0x800) / (0xC00 - 0x800);
                uint g = 0xFF;
                uint b = 0;

                palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }

            for (uint d = 0xC00; d < 0x1000; d++)
            {
                uint r = 0xFF;
                uint g = 0xFF - 0xFF * (d - 0xC00) / (0x1000 - 0xC00);
                uint b = 0;

                palette[d] = (int)(b | (g << 8) | (r << 16) | 0xFF000000);
            }
        }

        private int[] pixels;
        private WriteableBitmap bitmap;

        static private int[] palette;
    }
}
