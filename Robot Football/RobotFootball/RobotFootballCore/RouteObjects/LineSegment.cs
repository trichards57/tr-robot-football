using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using RobotFootballCore.Utilities;

namespace RobotFootballCore.RouteObjects
{
    public class LineSegment : RouteSegment
    {
        public LineSegment(PointF startPoint, PointF endPoint)
        {
            StartPoint = startPoint;
            EndPoint = endPoint;
        }

        public override float Length
        {
            get
            {
                var xLength = StartPoint.X - EndPoint.X;
                var yLength = StartPoint.Y - EndPoint.Y;

                return (float)Math.Sqrt(xLength * xLength + yLength * yLength);
            }
        }

        public override void Draw(Graphics field)
        {
            field.DrawLine(RouteSegment.DrawingPen, StartPoint, EndPoint);
        }

        public override void Scale(SizeF scale)
        {
            StartPoint = StartPoint.Scale(scale);
            EndPoint = EndPoint.Scale(scale);
        }

        public override string ToString()
        {
            return string.Format("{0} => {1}", StartPoint, EndPoint);
        }
    }
}
