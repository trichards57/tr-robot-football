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
using MathNet.Numerics.LinearAlgebra;

namespace RouteFinders
{
    public class MassedPotentialFieldRouteFinder : RouteFinder
    {
        public override Route FindPath(PointF startPoint, PointF endPoint, Field field, IPositionedObject movingObject)
        {
            var attractiveConstant = 1;
            var repulsiveConstant = -5000000;
            var repulsiveDistance = 150;
            var timestep = 0.1;
            var mass = 40;
            var flowResistance = 0.1;

            var currentVelocity = new Vector(2, 0);
            var currentPosition = new Vector(new double[] { startPoint.X, startPoint.Y });
            var endVector = new Vector(new double[] { endPoint.X, endPoint.Y });

            var route = new Route();
            var distance = endVector - currentPosition;

            while (distance.Norm() > 10)
            {
                var attractForce = attractiveConstant * distance;
                var repulseForce = new Vector(new[] { 0.0, 0.0 });

                //var distance = new SizeF(currentPosition.X - endPoint.X, currentPosition.Y - endPoint.Y).Scale(-1);
                
                //var attractForce = distance.Scale(attractiveConstant);

                //SizeF repulsiveForce = SizeF.Empty;

                foreach (var p in field.Players.Where(p => p.Team == Team.Opposition))
                {
                    var playerPos = new Vector(new double[] { p.Position.X, p.Position.Y });
                    var opDistance = playerPos - currentPosition;
                    var distMag = opDistance.Norm();
                    if (distMag > repulsiveDistance)
                        continue;
                    var directionVector = opDistance.Normalize();
                    var force = repulsiveConstant * directionVector / (distMag * distMag);
                    repulseForce += force;
                }

                var totalForce = attractForce + repulseForce;

                var angle = Math.Abs(Math.Acos(Vector.ScalarProduct(currentVelocity, attractForce) / (currentVelocity.Norm() * attractForce.Norm())));
                if (angle < Math.PI/2)
                {
                    var rotMat = new Matrix(new[] { new[] { Math.Cos(145), -Math.Sin(145) }, new[] { Math.Sin(145), Math.Cos(145) } });
                    attractForce = (rotMat * repulseForce.ToColumnMatrix()).GetColumnVector(0);
                    totalForce = attractForce + repulseForce;
                }

                currentVelocity += totalForce * timestep / mass;
                currentVelocity -= flowResistance * currentVelocity;
                var oldPosition = currentPosition;
                currentPosition += currentVelocity * timestep;
                route.Path.Add(new LineSegment(new PointF((float)oldPosition[0], (float)oldPosition[1]), new PointF((float)currentPosition[0], (float)currentPosition[1])));
                if (route.Path.Count > 80000)
                    break;
                distance = endVector - currentPosition;
            }

            return route;
        }
    }
}
