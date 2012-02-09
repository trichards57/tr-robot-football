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

        protected float CalculateLength(PointF startPoint, PointF endPoint)
        {
            var xLength = startPoint.X - endPoint.X;
            var yLength = startPoint.Y - endPoint.Y;

            return (float)Math.Sqrt(xLength * xLength + yLength * yLength);
        }
    }
}
