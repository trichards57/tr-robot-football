using System;
using System.Drawing;

namespace RobotFootballCore.Interfaces
{
    /// <summary>
    /// Base class for all route finders.
    /// </summary>
    public abstract class RouteFinder
    {
        /// <summary>
        /// The resolution to use for algorithms that discretize the field to find a route.
        /// </summary>
        protected Size Resolution { get; set; }

        /// <summary>
        /// Calculates the Euclidean distance between two points.
        /// </summary>
        /// <param name="startPoint">The start point</param>
        /// <param name="endPoint">The end point</param>
        /// <returns>A float that represents the distance between the two points</returns>
        protected static float CalculateLength(PointF startPoint, PointF endPoint)
        {
            var xLength = startPoint.X - endPoint.X;
            var yLength = startPoint.Y - endPoint.Y;

            return (float)Math.Sqrt(xLength * xLength + yLength * yLength);
        }
    }
}
