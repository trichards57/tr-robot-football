using System;
using System.Drawing;
using RobotFootballCore.Utilities;

namespace RobotFootballCore.RouteObjects
{
    /// <summary>
    /// Represents a straight line segment in a route
    /// </summary>
    public class LineSegment : RouteSegment
    {
        /// <summary>
        /// Initializes a new instance of the LineSegment class
        /// </summary>
        /// <param name="startPoint">The start point of the segment</param>
        /// <param name="endPoint">The end point of the segment</param>
        public LineSegment(PointF startPoint, PointF endPoint)
        {
            StartPoint = startPoint;
            EndPoint = endPoint;
        }

        /// <summary>
        /// The length of the LineSegment
        /// </summary>
        public override float Length
        {
            get
            {
                var xLength = StartPoint.X - EndPoint.X;
                var yLength = StartPoint.Y - EndPoint.Y;

                return (float)Math.Sqrt(xLength * xLength + yLength * yLength);
            }
        }
        /// <summary>
        /// Draws the LineSegment to the given Graphics context.
        /// </summary>
        /// <param name="field">The Graphics context used to render the route.</param>
        public override void Draw(Graphics field)
        {
            field.DrawLine(DrawingPen, StartPoint, EndPoint);
        }
        /// <summary>
        /// Scales the two points on the route.
        /// </summary>
        /// <param name="scale">The number to divide the points by</param>
        /// Scales StartPoint and EndPoint, usually used to shift between coordinate systems.
        public override void Scale(SizeF scale)
        {
            StartPoint = StartPoint.Scale(scale);
            EndPoint = EndPoint.Scale(scale);
        }
        /// <summary>
        /// Returns a string that represents the current object
        /// </summary>
        /// <returns>A string that represents the current object</returns>
        /// Returns a string of the format "{StartPoint} => {EndPoint}"
        public override string ToString()
        {
            return string.Format("{0} => {1}", StartPoint, EndPoint);
        }
    }
}
