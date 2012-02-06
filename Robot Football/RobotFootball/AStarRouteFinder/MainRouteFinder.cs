using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotFootballCore.Interfaces;
using System.Drawing;
using RobotFootballCore.Objects;
using RobotFootballCore.RouteObjects;

namespace AStarRouteFinder
{
    public class MainRouteFinder : IRouteFinder
    {
        public MainRouteFinder()
        {
            Resolution = new SizeF(10, 10);
        }

        public Route FindPath(PointF startPoint, PointF endPoint, Field field)
        {
            if (Resolution.Height < 1 && Resolution.Width < 1)
            {
                throw new InvalidOperationException("Resolution must be greater than or equal to 1 pixel by 1 pixel");
            }

            var route = new Route();

            return route;
        }

        public SizeF Resolution
        {
            get;
            set;
        }
    }
}
