using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;

namespace RobotFootballCore.RouteObjects
{
    public class Route
    {
        public Route()
        {
            Path = new Collection<RouteSegment>();
        }

        public Collection<RouteSegment> Path { get; private set; }
    }
}
