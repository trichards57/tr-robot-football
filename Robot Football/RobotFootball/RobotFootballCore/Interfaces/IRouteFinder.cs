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
        Route FindPath(PointF startPoint, PointF endPoint, Field field);

        SizeF Resolution { get; set; }
    }
}
