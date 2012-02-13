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
    public class HashSetAStarRouteFinder : RouteFinder
    {
        public int ObjectClearance { get; set; }

        public HashSetAStarRouteFinder()
        {
            Resolution = new Size(10, 10);
            ObjectClearance = 20;
        }

        private GridSquare[] InitGrid(PointF startPoint, PointF endPoint, Field field, Size gridSize, IPositionedObject movingObject)
        {
            var playersize = (movingObject.Size + new Size(ObjectClearance, ObjectClearance)).Scale(Resolution).Scale(2.0f).Ceiling();
            var clearance = Math.Max(playersize.Width, playersize.Height); // The amount to increase an obstacle's size by to allow for the player's size

            var grid = new GridSquare[gridSize.Height * gridSize.Width];
            // Initialize the grid
            Parallel.For(0, grid.Length, i => { 
                grid[i] = new GridSquare();
                grid[i].Location = PointExtensions.FromIndex(i, gridSize.Width); 
            });

            Parallel.ForEach(from p in field.Players where p.Team == Team.Opposition select p, player =>
            {
                var centerGridPoint = player.Position.Scale(Resolution).Floor();
                
                var minX = Math.Max(0, centerGridPoint.X - playersize.Width - clearance);
                var maxX = Math.Min(centerGridPoint.X + playersize.Width + clearance, gridSize.Width);
                var minY = Math.Max(0, centerGridPoint.Y - playersize.Height - clearance);
                var maxY = Math.Min(centerGridPoint.Y + playersize.Height + clearance, gridSize.Height);

                for (var i = minX; i < maxX; i++)
                {
                    for (var j = minY; j < maxY; j++)
                    {
                        if (i < 0 || j < 0)
                            continue;

                        var gridPoint = new Point(i, j);
                        grid[gridPoint.ToIndex(gridSize.Width)].Type = SquareType.Obstacle;
                    }
                }
            });

            var gridEndPoint = endPoint.Scale(Resolution).Floor();

            grid[gridEndPoint.ToIndex(gridSize.Width)].Type = SquareType.Destination;

            var gridStartPoint = startPoint.Scale(Resolution).Floor();
            grid[gridStartPoint.ToIndex(gridSize.Width)].Type = SquareType.Origin;

            return grid;
        }

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

        private Route ReconstructPath(Dictionary<GridSquare, GridSquare> cameFrom, GridSquare currentSquare)
        {
            if (cameFrom.ContainsKey(currentSquare))
            {
                var path = ReconstructPath(cameFrom, cameFrom[currentSquare]);
                path.Path.Add(new LineSegment(cameFrom[currentSquare].Location, currentSquare.Location));
                return path;
            }
            return new Route();
        }

        private float CalculateHeuristic(GridSquare square, PointF endPoint)
        {
            if (square.Type == SquareType.Obstacle)
                return float.PositiveInfinity;
            else
                return CalculateLength(square.Location, endPoint);
        }

        
    }
}
