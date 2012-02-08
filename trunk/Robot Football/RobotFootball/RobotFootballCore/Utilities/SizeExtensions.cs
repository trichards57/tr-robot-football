using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace RobotFootballCore.Utilities
{
    public static class SizeExtensions
    {
        public static SizeF Invert(this SizeF size)
        {
            if (size.Width == 0 || size.Height == 0)
                throw new ArgumentOutOfRangeException("size", size, "size must not have zero width or height");
            var width = 1.0f / size.Width;
            var height = 1.0f / size.Height;

            return new SizeF(width, height);
        }

        public static SizeF Invert(this Size size)
        {
            if (size.Width == 0 || size.Height == 0)
                throw new ArgumentOutOfRangeException("size", size, "size must not have zero width or height");
            var width = 1.0f / size.Width;
            var height = 1.0f / size.Height;

            return new SizeF(width, height);
        }

        public static SizeF Scale(this SizeF size, SizeF scale)
        {
            var width = size.Width / scale.Width;
            var height = size.Height / scale.Height;

            return new SizeF(width, height);
        }

        public static SizeF Scale(this Size size, float scale)
        {
            var width = (float)size.Width / scale;
            var height = (float)size.Height / scale;

            return new SizeF(width, height);
        }

        public static SizeF Scale(this Size size, SizeF scale)
        {
            var width = (float)size.Width / scale.Width;
            var height = (float)size.Height / scale.Height;

            return new SizeF(width, height);
        }

        public static SizeF Scale(this SizeF size, float scale)
        {
            var width = size.Width / scale;
            var height = size.Height / scale;

            return new SizeF(width, height);
        }

        public static Size Ceiling(this SizeF size)
        {
            var width = (int)Math.Ceiling(size.Width);
            var height = (int)Math.Ceiling(size.Height);

            return new Size(width, height);
        }
    }
}
