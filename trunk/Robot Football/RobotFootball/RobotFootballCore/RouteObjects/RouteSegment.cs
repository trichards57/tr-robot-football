using System.Drawing;

namespace RobotFootballCore.RouteObjects
{
    /// <summary>
    /// Represents an abstract section of a route
    /// </summary>
    public abstract class RouteSegment
    {
        /// <summary>
        /// The start point of the route
        /// </summary>
        public PointF StartPoint { get; protected set; }
        /// <summary>
        /// The end point of the route
        /// </summary>
        public PointF EndPoint { get; protected set; }

        /// <summary>
        /// The length of the RouteSegment
        /// </summary>
        public abstract float Length { get; }
        /// <summary>
        /// Draws the RouteSegment to a Graphics context
        /// </summary>
        /// <param name="field">The Graphics context that is rendering the RouteSegment </param>
        public abstract void Draw(Graphics field);
        /// <summary>
        /// Scales the route segment
        /// </summary>
        /// <param name="scale">The number to divide the segment by</param>
        /// Normally used to shift between coordinate systems.
        public abstract void Scale(SizeF scale);

        /// <summary>
        /// Gets the pen used to draw the RouteSegment
        /// </summary>
        public static Pen DrawingPen { get { return new Pen(Color.White, 2.5f); } }
    }
}
