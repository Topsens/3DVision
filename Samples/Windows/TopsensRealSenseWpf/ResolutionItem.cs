using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TopsensRealSenseWpf
{
    public class ResolutionItem
    {
        public ResolutionItem(int width, int height)
        {
            this.Width  = width;
            this.Height = height;
        }

        public override string ToString()
        {
            return this.Width.ToString() + "x" + this.Height.ToString();
        }

        public int Width  { get; private set; }
        public int Height { get; private set; }
    }
}