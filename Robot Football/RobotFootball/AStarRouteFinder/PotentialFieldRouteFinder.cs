using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotFootballCore.Interfaces;
using RobotFootballCore.RouteObjects;
using System.Drawing;
using RobotFootballCore.Objects;
using RobotFootballCore.Utilities;
using System.Threading.Tasks;

namespace RouteFinders
{
    public class PotentialFieldRouteFinder : RouteFinder
    {
        public override Route FindPath(PointF startPoint, PointF endPoint, Field field, IPositionedObject movingObject)
        {
            const int attractiveConstant = 1/10;
            const int repulsiveConstant = 10;
            const int repulsiveDistance = 20;

            var currentVelocity = Size.Empty;
            var currentPosition = startPoint;

            var route = new Route();

            while (CalculateLength(currentPosition, endPoint) > 10)
            {
                var distance = new SizeF(currentPosition.X - endPoint.X, currentPosition.Y - endPoint.Y);
                var attractForce = distance.Scale(attractiveConstant);

                var repulseForce = 0;

                Parallel.ForEach(field.Players.Where(p => p.Team == Team.Opposition), p =>
                {

                });
            }

            return route;
        }
    }
}
