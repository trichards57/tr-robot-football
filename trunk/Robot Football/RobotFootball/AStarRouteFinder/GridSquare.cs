using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AStarRouteFinder
{
    class GridSquare
    {
        private bool obstacle = false;

        public int? Weight { get; set; }
        public SquareType Type { get; set; }

    }

    public enum SquareType
    {
        Origin,
        Obstacle,
        Destination
    }
}
