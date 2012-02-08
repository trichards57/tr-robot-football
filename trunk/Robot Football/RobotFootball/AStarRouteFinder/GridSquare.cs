using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace RouteFinders
{
    class GridSquare
    {
        public GridSquare()
        {
            KnownScore = float.MaxValue;
        }

        public float KnownScore { get; set; }
        public float HeuristicScore { get; set; }
        public float TotalScore { get { return KnownScore + HeuristicScore; } }
        public SquareType Type { get; set; }
        public Point Location { get; set; }
    }

    public enum SquareType
    {
        Empty = 0,
        Origin,
        Obstacle,
        Destination
    }
}
