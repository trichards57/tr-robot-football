using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace RobotFootballCore.RouteObjects
{
    public abstract class RouteSegment
    {
        public PointF StartPoint { get; set; }
        public PointF EndPoint { get; set; }

        public abstract float Length { get; }
    }
}
