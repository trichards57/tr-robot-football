using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Threading.Tasks;
using RobotFootballCore.Interfaces;
using RobotFootballCore.Objects;
using RobotFootballCore.RouteObjects;
using RobotFootballCore.Utilities;

namespace RouteFinders
{
    /// <summary>
    /// A route finder that determines a route using the A* algorithm using a basic List to hold the squares and parallelization for optimization
    /// </summary>
    public class ParallelListAStarRouteFinder : ParallelAStarRouteFinder<List<GridSquare>>
    {
    }

    /// <summary>
    /// A route finder that determines a route by using the A* algorithm, using a HashSet to speed up searches in the open and closed sets, and parallelization for optimization.
    /// </summary>
    public class ParallelHashSetAStarRouteFinder : ParallelAStarRouteFinder<HashSet<GridSquare>>
    {
    }

    /// <summary>
    /// A route finder that determines a route using the A* algorithm, and parallelization to optimize the search
    /// </summary>
    public class ParallelAStarRouteFinder<TStorageClass> : AStarRouteFinder<TStorageClass> where TStorageClass : ICollection<GridSquare>, new()
    {
        /// <summary>
        /// Finds a route using a parallelized A* algorithm
        /// </summary>
        /// <param name="startPoint">The point to start the algorithm from.</param>
        /// <param name="endPoint">The point to find a route to.</param>
        /// <param name="field">The field in which the route must be found.</param>
        /// <param name="movingObject">The object that will move around the <paramref name="field"/>.</param>
        /// <returns>
        /// A Route if a route can be found.
        /// 
        /// null otherwise
        /// </returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown if the specified resolution is 0 in one or more directions.
        /// </exception>
        /// 
        /// Determines a route from <paramref name="startPoint" /> to <paramref name="endPoint" /> using a parallelized A* algorithm.
        ///
        /// Algorithm taken from @cite aiModernApproach
        public override Route FindPath(PointF startPoint, PointF endPoint, Field field, IPositionedObject movingObject)
        {
            if (Resolution.Height < 1 || Resolution.Width < 1)
                throw new InvalidOperationException("Resolution must be greater than or equal to 1 pixel by 1 pixel");

            var gridSize = field.Size.Scale(Resolution).Ceiling();

            var grid = InitGrid(startPoint, endPoint, field, gridSize, movingObject);

            var startPoints = from g in grid where g.Type == SquareType.Origin select g;

            var closedSet = new TStorageClass(); // The already checked points
            var openSet = new TStorageClass();
            foreach (var f in from g in grid where g.Type == SquareType.Origin select g) // The points to check
                openSet.Add(f);
            var cameFrom = new Dictionary<GridSquare, GridSquare>(); // A list of route data already calculated

            // Initialize the origin points
            Parallel.ForEach(startPoints, g => { g.KnownScore = 0; g.HeuristicScore = CalculateLength(g.Location, endPoint); });

            var tasks = new List<Task>();

            // While there are points available to check
            while (openSet.Any() || tasks.Any(t => !t.IsCompleted))
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

                // Calculate the neighboring indexes and discard the ones out of range.
                var neighbourIndexes = new[] { index + 1, 
                                               index - 1, 
                                               index + gridSize.Width, 
                                               index - gridSize.Width,
                                               index + gridSize.Width + 1, 
                                               index - gridSize.Width + 1,
                                               index + gridSize.Width - 1, 
                                               index - gridSize.Width - 1 }.Where(i => (i >= 0) && (i < grid.Length) && !closedSet.Contains(grid[i])).ToList();

                foreach (var neighbour in neighbourIndexes.Select(i => grid[i]).Where(neighbour => !openSet.Contains(neighbour)))
                {
                    openSet.Add(neighbour);
                    cameFrom[neighbour] = null;
                    neighbour.HeuristicScore = CalculateHeuristic(neighbour, endPoint);
                }

                Parallel.ForEach(neighbourIndexes, (i, state) =>
                {
                    var neighbour = grid[i];

                    // Work out the distance to the origin
                    var tentativeKnownScore = square.KnownScore + CalculateLength(square.Location, neighbour.Location);
                    
                    if (tentativeKnownScore < neighbour.KnownScore)
                    {
                        // If necessary, update the square's known score and note the path to that square.
                        cameFrom[neighbour] = square;
                        neighbour.KnownScore = tentativeKnownScore;
                    }
                });
            }

            return null;
        }
    }
}
