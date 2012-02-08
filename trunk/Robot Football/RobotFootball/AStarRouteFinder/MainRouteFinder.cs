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
            Resolution = new Size(10, 10);
        }

        public Route FindPath(PointF startPoint, PointF endPoint, Field field)
        {
            if (Resolution.Height < 1 && Resolution.Width < 1)
            {
                throw new InvalidOperationException("Resolution must be greater than or equal to 1 pixel by 1 pixel");
            }

            var route = new Route();

            var gridSize = new Size(field.Width / Resolution.Width, field.Height / Resolution.Height);

            var grid = new GridSquare[gridSize.Height * gridSize.Width];

            foreach (var player in from p in field.Players where p.Team == Team.Opposition select p)
            {
                var x = (int)Math.Floor(player.Position.X / Resolution.Width);
                var y = (int)Math.Floor(player.Position.Y / Resolution.Height);

                grid[x + y * gridSize.Width].Type = SquareType.Obstacle;
            }

            return route;
        }

        public Size Resolution
        {
            get;
            set;
        }


        SizeF IRouteFinder.Resolution
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }
    }
}
