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
    /// Interaction logic for UsersView.xaml
    /// </summary>
    public partial class UsersView : UserControl
    {
        public UsersView()
        {
            InitializeComponent();
        }

        public void Draw(UsersFrame frame, Orientation orientation)
        {
            if (null == frame)
            {
                return;
            }

            if (null == this.frame)
            {
                this.frame = new UsersFrame();
            }

            lock (this.frame)
            {
                frame.CopyTo(this.frame);
            }

            this.Dispatcher.BeginInvoke(new Action(()=>
            {
                if (null == this.painter)
                {
                    this.painter = new UserPainter(this.canvas);
                }

                lock (this.frame)
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

                    this.painter.Paint(this.frame);
                }
            }), DispatcherPriority.Render);
        }

        private UsersFrame  frame;
        private UserPainter painter;
    }
}
