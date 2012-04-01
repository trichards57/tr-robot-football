using System;
using System.Drawing;

/// @brief Utilities to help simplify functions for the Robot Football UI
/// 
/// Intended to contain all the extension classes and other helper functions used by the Robot Football Ui
namespace RobotFootballCore.Utilities
{
    /// <summary>
    /// Extension methods used to help manipulate points
    /// </summary>
    public static class PointExtensions
    {
        /// <summary>
        /// Scales a PointF
        /// </summary>
        /// <param name="point">The PointF to scale</param>
        /// <param name="resolution">The respective width and height to divide the point by</param>
        /// <returns>
        /// @f$ x = point.X / resolution.Width @f$
        /// 
        /// @f$ y = point.Y / resolution.Height @f$
        /// </returns>
        public static PointF Scale(this PointF point, SizeF resolution)
        {
            var x = point.X / resolution.Width;
            var y = point.Y / resolution.Height;

            return new PointF(x, y);
        }
        /// <summary>
        /// Scales a Point
        /// </summary>
        /// <param name="point">The Point to scale</param>
        /// <param name="resolution">The respective width and height to divide the point by</param>
        /// <returns>
        /// @f$x = point.X / resolution.Width@f$
        /// 
        /// @f$y = point.Y / resolution.Height@f$
        /// </returns>
        public static PointF Scale(this Point point, SizeF resolution)
        {
            var x = point.X / resolution.Width;
            var y = point.Y / resolution.Height;

            return new PointF(x, y);
        }
        /// <summary>
        /// Scales a PointF
        /// </summary>
        /// <param name="point">The PointF to scale</param>
        /// <param name="size">The value to divide the point by</param>
        /// <returns>
        /// @f$x = point.X / size@f$
        /// 
        /// @f$y = point.Y / size@f$
        /// </returns>
        public static PointF Scale(this Point point, int size)
        {
            var x = (float)point.X / size;
            var y = (float)point.Y / size;

            return new PointF(x, y);
        }
        /// <summary>
        /// Converts an array index to a Point using the width of the grid
        /// </summary>
        /// <param name="index">The index to convert</param>
        /// <param name="width">The width of the grid</param>
        /// <returns>A Point containing the x and y as integers</returns>
        /// Converts an array index into a set of coordinates, using the width of the field. Works using
        /// equations:
        ///
        /// \f$ x = index \% width \f$
        ///
        /// \f$ y = index / width \f$
        public static Point FromIndex(int index, int width)
        {
            return new Point(index % width, index / width);
        }
        /// <summary>
        /// Converts a Point into an array index using the width of the grid
        /// </summary>
        /// <param name="point">The coordinate</param>
        /// <param name="width">The width of the grid</param>
        /// <returns>An int containing the index</returns>
        /// Converts a coordinate into an array index, using the width of the field. Works using the equation:
        ///
        /// \f$ index = x + y \times width \f$
        public static int ToIndex(this Point point, int width)
        {
            return point.X + point.Y * width;
        }
        /// <summary>
        /// Converts a PointF to a Point by using the Floor function
        /// </summary>
        /// <param name="point">The point to convert</param>
        /// <returns>An integer representation of point</returns>
        public static Point Floor(this PointF point)
        {
            return new Point((int)Math.Floor(point.X), (int)Math.Floor(point.Y));
        }
    }
}
