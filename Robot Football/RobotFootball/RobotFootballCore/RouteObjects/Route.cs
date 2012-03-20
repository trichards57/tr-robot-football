using System.Collections.ObjectModel;
using System.Drawing;
using System.Linq;

namespace RobotFootballCore.RouteObjects
{
    /// <summary>
    /// Represents a Route through a Field
    /// </summary>
    public class Route
    {
        /// <summary>
        /// Initializes a new instance of the Route class.
        /// </summary>
        public Route()
        {
            Path = new Collection<RouteSegment>();
        }
        /// <summary>
        /// Initializes a new instance of the Route class.
        /// </summary>
        /// <param name="points">The points to add into the route, each joined together by a LineSegment</param>
        public Route(Point[] points)
        {
            Path = new Collection<RouteSegment>();
            for (var i = 0; i < points.Length - 1; i++)
            {
                if ((points[i] == Point.Empty || points[i + 1] == Point.Empty))
                    break;
                Path.Add(new LineSegment(points[i], points[i + 1]));
            }
        }
        /// <summary>
        /// The path the route follows.
        /// </summary>
        public Collection<RouteSegment> Path { get; private set; }
        /// <summary>
        /// Draws the route to a graphics context
        /// </summary>
        /// <param name="field">The graphics context used to render the route</param>
        /// Renders each segment of the route, segment by segment.
        /// 
        /// If the route is only made up of LineSegment, renders the line all in one go with Graphics::DrawLines
        public void Draw(Graphics field)
        {
            if (Path.All(s => s is LineSegment))
            {
                // Special case, as most paths will be all line segments, which produces a nicer looking joined up path.
                var points = new[] { Path.First().StartPoint }.Concat(Path.Select(s => s.EndPoint)).ToArray();
                field.DrawLines(RouteSegment.DrawingPen, points);
            }
            else
            {
                foreach (var segment in Path)
                {
                    segment.Draw(field);
                }
            }
        }
        /// <summary>
        /// Scales the route by scaling each segment in turn.
        /// </summary>
        /// <param name="scale">The number to divide each of the points by.</param>
        /// <remarks>Usually used to shift between coordinate systems.</remarks>
        public void Scale(SizeF scale)
        {
            foreach (var s in Path)
                s.Scale(scale);
        }
    }
}
