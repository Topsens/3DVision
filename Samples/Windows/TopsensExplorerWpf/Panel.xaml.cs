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

using Topsens;

namespace TopsensExplorerWpf
{
    /// <summary>
    /// Interaction logic for Panel.xaml
    /// </summary>
    public partial class Panel : UserControl
    {
        public Panel()
        {
            InitializeComponent();
        }

        public Topsens.Orientation Orientation()
        {
            if (this.land.IsChecked ?? false)
            {
                return Topsens.Orientation.Landscape;
            }

            if (this.clock.IsChecked ?? false)
            {
                return Topsens.Orientation.PortraitClockwise;
            }

            return Topsens.Orientation.PortraitAntiClockwise;
        }

        private void OnUsersNo(object sender, RoutedEventArgs e)
        {
            this.groundNo.IsChecked = true;
        }

        private void OnGroundYes(object sender, RoutedEventArgs e)
        {
            this.users.IsChecked = true;
        }
    }
}
