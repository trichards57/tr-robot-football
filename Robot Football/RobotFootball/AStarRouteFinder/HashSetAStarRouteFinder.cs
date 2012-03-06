using System;
using System.Drawing;
using System.Linq;
using RobotFootballCore.Interfaces;
using RobotFootballCore.Objects;
using RobotFootballCore.RouteObjects;
using RobotFootballCore.Utilities;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace RouteFinders
{
    /// <summary>
    /// A route finder that determines a route by using the A* algorithm, using a HashSet to speed up searches in the open and closed sets.
    /// </summary>
    public class HashSetAStarRouteFinder : AStarRouteFinder
    {
        public override Route FindPath(PointF startPoint, PointF endPoint, Field field, IPositionedObject movingObject)
        {
            if (Resolution.Height < 1 || Resolution.Width < 1)
                throw new InvalidOperationException("Resolution must be greater than or equal to 1 pixel by 1 pixel");

            var gridSize = field.Size.Scale(Resolution).Ceiling();

            var grid = InitGrid(startPoint, endPoint, field, gridSize, movingObject);

            var startPoints = from g in grid where g.Type == SquareType.Origin select g;

            var closedSet = new HashSet<GridSquare>(); // The already checked points
            var openSet = new HashSet<GridSquare>(from g in grid where g.Type == SquareType.Origin select g); // The points to check
            var cameFrom = new Dictionary<GridSquare, GridSquare>(); // A list of route data already calculated

            // Initialise the origin points
            Parallel.ForEach(startPoints, g => { g.KnownScore = 0; g.HeuristicScore = CalculateHeuristic(g, endPoint); });

            // While there are points available to check
            while (openSet.Any())
            {
                var square = openSet.OrderBy(p => p.TotalScore).First();
                if (square.Type == SquareType.Destination)
                {
                    var path = ReconstructPath(cameFrom, cameFrom[square]);
                    path.Path.Add(new LineSegment(cameFrom[square].Location, square.Location));
                    path.Scale(Resolution.Invert()); // Convert the path back to world coordinates from grid coordinates
                    return path;
                }

                openSet.Remove(square);
                closedSet.Add(square);

                var index = square.Location.ToIndex(gridSize.Width);

                // Calculate the neighbouring indexes and discard the ones out of range.
                var neighbourIndexes = new[] { index + 1, 
                                               index - 1, 
                                               index + gridSize.Width, 
                                               index - gridSize.Width,
                                               index + gridSize.Width + 1, 
                                               index - gridSize.Width + 1,
                                               index + gridSize.Width - 1, 
                                               index - gridSize.Width - 1 }.Where(i => (i >= 0) && (i < grid.Length));

                foreach (var i in neighbourIndexes)
                {
                    var neighbour = grid[i];

                    if (closedSet.Contains(neighbour))
                        continue; // Already checked this square. Skip it

                    // Work out the distance to the origin
                    var tentativeKnownScore = square.KnownScore + CalculateLength(square.Location, neighbour.Location);

                    bool tentativeIsBetter = false;

                    if (!openSet.Contains(neighbour))
                    {
                        // First time this point has been considered. The current distance is the best guess.
                        openSet.Add(neighbour);
                        neighbour.HeuristicScore = CalculateHeuristic(neighbour, endPoint);
                        tentativeIsBetter = true;
                    }
                    else if (tentativeKnownScore < neighbour.KnownScore)
                        // Point has been considered before and the last consideration was given a higher score, so use the knew one.
                        tentativeIsBetter = true;
                    else
                        // Point has been considered before and the last consideration was given a lower score, so use that one.
                        tentativeIsBetter = false;

                    if (tentativeIsBetter)
                    {
                        // If necessary, update the square's known score and note the path to that square.
                        cameFrom[neighbour] = square;
                        neighbour.KnownScore = tentativeKnownScore;
                    }
                }
            }

            return null;
        }        
    }
}
