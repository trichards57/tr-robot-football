using System;
using System.Drawing;

namespace RobotFootballCore.Utilities
{
    /// <summary>
    /// Extension methods used to help manipulate sizes
    /// </summary>
    public static class SizeExtensions
    {
        /// <summary>
        /// Takes a SizeF and inverts each dimension
        /// </summary>
        /// <param name="size">The SizeF to invert</param>
        /// <returns>
        /// @f$ width = 1.0f / width @f$
        /// 
        /// @f$ height = 1.0f / height @f$
        /// </returns>
        public static SizeF Invert(this SizeF size)
        {
            if (size.Width == 0 || size.Height == 0)
                throw new ArgumentOutOfRangeException("size", size, "size must not have zero width or height");
            var width = 1.0f / size.Width;
            var height = 1.0f / size.Height;

            return new SizeF(width, height);
        }
        /// <summary>
        /// Takes a Size and inverts each dimension
        /// </summary>
        /// <param name="size">The Size to invert</param>
        /// <returns>
        /// @f$ width = 1.0f / width @f$
        /// 
        /// @f$ height = 1.0f / height @f$
        /// </returns>
        public static SizeF Invert(this Size size)
        {
            if (size.Width == 0 || size.Height == 0)
                throw new ArgumentOutOfRangeException("size", size, "size must not have zero width or height");
            var width = 1.0f / size.Width;
            var height = 1.0f / size.Height;

            return new SizeF(width, height);
        }
        /// <summary>
        /// Scales a SizeF
        /// </summary>
        /// <param name="size">The SizeF to scale</param>
        /// <param name="scale">The respective width and height to divide the size by</param>
        /// <returns>
        /// @f$ width = size.Width / resolution.Width @f$
        /// 
        /// @f$ height = size.Height / resolution.Height @f$
        /// </returns>
        public static SizeF Scale(this SizeF size, SizeF scale)
        {
            var width = size.Width / scale.Width;
            var height = size.Height / scale.Height;

            return new SizeF(width, height);
        }
        /// <summary>
        /// Scales a Size
        /// </summary>
        /// <param name="size">The Size to scale</param>
        /// <param name="scale">The integer to divide the size by</param>
        /// <returns>
        /// @f$ width = size.Width / resolution.Width @f$
        /// 
        /// @f$ height = size.Height / resolution.Height @f$
        /// </returns>
        public static SizeF Scale(this Size size, float scale)
        {
            var width = size.Width / scale;
            var height = size.Height / scale;

            return new SizeF(width, height);
        }
        /// <summary>
        /// Scales a Size
        /// </summary>
        /// <param name="size">The Size to scale</param>
        /// <param name="scale">The respective width and height to divide the size by</param>
        /// <returns>
        /// @f$ width = size.Width / resolution.Width @f$
        /// 
        /// @f$ height = size.Height / resolution.Height @f$
        /// </returns>
        public static SizeF Scale(this Size size, SizeF scale)
        {
            var width = size.Width / scale.Width;
            var height = size.Height / scale.Height;

            return new SizeF(width, height);
        }
        /// <summary>
        /// Scales a SizeF
        /// </summary>
        /// <param name="size">The SizeF to scale</param>
        /// <param name="scale">The float to divide the size by</param>
        /// <returns>
        /// @f$ width = size.Width / resolution.Width @f$
        /// 
        /// @f$ height = size.Height / resolution.Height @f$
        /// </returns>
        public static SizeF Scale(this SizeF size, float scale)
        {
            var width = size.Width / scale;
            var height = size.Height / scale;

            return new SizeF(width, height);
        }
        /// <summary>
        /// Converts a SizeF to a Size by using the ceiling function
        /// </summary>
        /// <param name="size">The point to convert</param>
        /// <returns>An integer representation of size</returns>
        public static Size Ceiling(this SizeF size)
        {
            var width = (int)Math.Ceiling(size.Width);
            var height = (int)Math.Ceiling(size.Height);

            return new Size(width, height);
        }
    }
}
