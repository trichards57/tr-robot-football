using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace RobotFootballCore.Interfaces
{
    public interface IPositionedObject
    {
        Point Position { get; }
        Size Size { get; }
    }
}
