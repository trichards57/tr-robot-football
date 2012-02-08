using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.Drawing;

namespace RobotFootballCore.RouteObjects
{
    public class Route
    {
        public Route()
        {
            Path = new Collection<RouteSegment>();
        }

        public Collection<RouteSegment> Path { get; private set; }

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

        public void Scale(SizeF scale)
        {
            foreach (var s in Path)
                s.Scale(scale);
        }
    }
}
