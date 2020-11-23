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

        public void Enable()
        {
            this.devices.IsEnabled = true;
            this.cres.IsEnabled = true;
            this.dres.IsEnabled = true;
            this.start.IsEnabled = true;
            this.usersGroup.IsEnabled = true;
            this.alignGroup.IsEnabled = true;
            this.flipGroup.IsEnabled = true;
            this.orientGroup.IsEnabled = true;
            this.recordGroup.IsEnabled = true;
            this.groundGroup.IsEnabled = true;
            this.refresh.IsEnabled = true;
            this.start.IsEnabled = true;
            this.stop.IsEnabled = false;
        }

        public void Disable()
        {
            this.devices.IsEnabled = false;
            this.cres.IsEnabled = false;
            this.dres.IsEnabled = false;
            this.start.IsEnabled = false;
            this.usersGroup.IsEnabled = false;
            this.alignGroup.IsEnabled = false;
            this.flipGroup.IsEnabled = false;
            this.orientGroup.IsEnabled = false;
            this.recordGroup.IsEnabled = false;
            this.groundGroup.IsEnabled = false;
            this.refresh.IsEnabled = false;
            this.start.IsEnabled = false;
            this.stop.IsEnabled = true;
        }

        public void Count(int count)
        {
            this.devices.Items.Clear();

            for (int i = 0; i < count; i++)
            {
                this.devices.Items.Add(i.ToString());
            }

            if (count > 0)
            {
                this.devices.SelectedIndex = 0;
                this.Enable();
            }
            else
            {
                this.Disable();
                this.refresh.IsEnabled = true;
                this.stop.IsEnabled = false;
            }
        }

        public bool Flip()
        {
            return this.flip.IsChecked ?? false;
        }

        public bool Align()
        {
            return this.align.IsChecked ?? false;
        }

        public bool Record()
        {
            return this.record.IsChecked ?? false;
        }

        public bool GenUsers()
        {
            return this.users.IsChecked ?? false;
        }

        public Resolution ColorRes()
        {
            return (Resolution)this.cres.SelectedIndex;
        }

        public Resolution DepthRes()
        {
            return (Resolution)this.dres.SelectedIndex;
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
