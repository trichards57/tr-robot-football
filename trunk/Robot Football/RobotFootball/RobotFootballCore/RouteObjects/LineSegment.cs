using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace RobotFootballCore.RouteObjects
{
    public class LineSegment : RouteSegment
    {
        public override double Length
        {
            get
            {
                var xLength = StartPoint.X - EndPoint.X;
                var yLength = StartPoint.Y - EndPoint.Y;

                return Math.Sqrt(xLength * xLength + yLength * yLength);
            }
        }
    }
}
