using System.Drawing;

namespace RouteFinders
{
    /// <summary>
    /// A square in a discretized environment
    /// </summary>
    /// Represents a discrete square in an environment, as required by some algorithms such as A*.
    public class GridSquare
    {
        /// <summary>
        /// Initializes a new instance of the GridSquare class.
        /// </summary>
        /// Sets KnownScore to it's default value of float.MaxValue.
        public GridSquare()
        {
            KnownScore = float.MaxValue;
        }

        /// <summary>
        /// The known score for the square
        /// </summary>
        /// Shows how far the square is from the route.
        public float KnownScore { get; set; }
        /// <summary>
        /// The heuristic score for the square
        /// </summary>
        /// The score for the square determined by the current heuristic method.
        public float HeuristicScore { get; set; }
        /// <summary>
        /// The total score for the square.
        /// </summary>
        /// The sum of the KnownScore and HeuristicScore
        /// <value>
        /// A single-precision floating point number
        /// </value>
        public float TotalScore { get { return KnownScore + HeuristicScore; } }
        /// <summary>
        /// The type of square
        /// </summary>
        /// Used by the route finding algorithms to work out the KnownScore.
        public SquareType Type { get; set; }
        /// <summary>
        /// The location of the square
        /// </summary>
        /// The (x,y) coordinates of the square in the field. Used to save recalculating the position of the square every time it is needed.
        public Point Location { get; set; }
    }

    /// <summary>
    /// Represents the different types of GridSquare that can exist in an environment.
    /// </summary>
    public enum SquareType
    {
        /// <summary>
        /// A square containing nothing.
        /// </summary>
        Empty = 0,
        /// <summary>
        /// A square to start a route from.
        /// </summary>
        Origin,
        /// <summary>
        /// A square that cannot be passed.
        /// </summary>
        Obstacle,
        /// <summary>
        /// A square to end a route on.
        /// </summary>
        Destination
    }
}
