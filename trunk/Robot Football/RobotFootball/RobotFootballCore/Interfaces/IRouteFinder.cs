using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotFootballCore.RouteObjects;
using System.Drawing;
using RobotFootballCore.Objects;

namespace RobotFootballCore.Interfaces
{
    public interface IRouteFinder
    {
        public Route FindPath(PointF startPoint, PointF endPoint, Field field);

        public SizeF Resolution { get; set; }
    }
}
