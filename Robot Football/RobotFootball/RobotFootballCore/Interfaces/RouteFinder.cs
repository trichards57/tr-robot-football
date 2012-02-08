using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotFootballCore.RouteObjects;
using System.Drawing;
using RobotFootballCore.Objects;

namespace RobotFootballCore.Interfaces
{
    public abstract class RouteFinder
    {
        public abstract Route FindPath(PointF startPoint, PointF endPoint, Field field, IPositionedObject movingObject);

        public Size Resolution { get; set; }
    }
}
