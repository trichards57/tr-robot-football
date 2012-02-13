using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace RobotFootballCore.Utilities
{
    public static class PointExtensions
    {
        public static PointF Scale(this PointF point, SizeF resolution)
        {
            var x = point.X / resolution.Width;
            var y = point.Y / resolution.Height;

            return new PointF(x, y);
        }

        public static PointF Scale(this Point point, SizeF resolution)
        {
            var x = (float)point.X / resolution.Width;
            var y = (float)point.Y / resolution.Height;

            return new PointF(x, y);
        }

        public static PointF Scale(this Point point, int size)
        {
            var x = (float)point.X / size;
            var y = (float)point.Y / size;

            return new PointF(x, y);
        }

        public static Point FromIndex(int index, int width)
        {
            return new Point(index % width, index / width);
        }

        public static int ToIndex(this Point point, int width)
        {
            return point.X + point.Y * width;
        }

        public static Point Floor(this PointF point)
        {
            return new Point((int)Math.Floor(point.X), (int)Math.Floor(point.Y));
        }
    }
}
