using System;
using System.Drawing;
using System.Linq;
using MathNet.Numerics.LinearAlgebra;
using RobotFootballCore.Interfaces;
using RobotFootballCore.Objects;
using RobotFootballCore.RouteObjects;

namespace RouteFinders
{
    /// <summary>
    /// A route finder that uses a Potential Field algorithm to find a route
    /// </summary>
    public sealed class PotentialFieldRouteFinder : RouteFinder
    {
        /// <summary>
        /// Finds a route using a Potential Field algorithm
        /// </summary>
        /// <param name="startPoint">The point to start the algorithm from.</param>
        /// <param name="endPoint">The point to find a route to.</param>
        /// <param name="field">The field in which the route must be found.</param>
        /// <returns>
        /// A Route if a route can be found.
        /// 
        /// Otherwise, the route that has been found so far.
        /// </returns>
        /// Determines a route from <paramref name="startPoint" /> to <paramref name="endPoint" /> using a Potential Field algorithm.
        /// 
        /// Algorithm taken from @cite intelligentAlgorithmPathPlanning
        public static Route FindPath(PointF startPoint, PointF endPoint, Field field)
        {
            const int attractiveConstant = 1;
            const int repulsiveConstant = -50000000;
            const int repulsiveDistance = 150;
            const double timestep = 0.05;

            var currentVelocity = new Vector(2, 0);
            var currentPosition = new Vector(new double[] { startPoint.X, startPoint.Y });
            var endVector = new Vector(new double[] { endPoint.X, endPoint.Y });

            var route = new Route();
            var distance = endVector - currentPosition;

            while (distance.Norm() > 10)
            {
                var attractForce = attractiveConstant * distance;
                var repulseForce = new Vector(new[] { 0.0, 0.0 });

                var position = currentPosition;
                repulseForce = (from p in field.Players.Where(p => p.Team == Team.Opposition) 
                                select new Vector(new double[] {p.Position.X, p.Position.Y}) 
                                into playerPos select playerPos - position 
                                into opDistance let distMag = opDistance.Norm() 
                                where distMag <= repulsiveDistance 
                                let directionVector = opDistance.Normalize() 
                                select repulsiveConstant*directionVector/(distMag*distMag)).Aggregate(repulseForce, (current, force) => current + force);

                var totalForce = attractForce + repulseForce;

                var angle = Math.Abs(Math.Acos(Vector.ScalarProduct(currentVelocity, attractForce) / (currentVelocity.Norm() * attractForce.Norm())));
                if (angle < Math.PI/2)
                {
                    var rotMat = new Matrix(new[] { new[] { Math.Cos(145), -Math.Sin(145) }, new[] { Math.Sin(145), Math.Cos(145) } });
                    attractForce = (rotMat * repulseForce.ToColumnMatrix()).GetColumnVector(0);
                    totalForce = attractForce + repulseForce;
                }

                currentVelocity = totalForce * timestep;
                var oldPosition = currentPosition;
                currentPosition += currentVelocity * timestep;
                route.Path.Add(new LineSegment(new PointF((float)oldPosition[0], (float)oldPosition[1]), new PointF((float)currentPosition[0], (float)currentPosition[1])));
                if (route.Path.Count > 4056)
                    break;
                distance = endVector - currentPosition;
            }

            return route;
        }
    }
}
