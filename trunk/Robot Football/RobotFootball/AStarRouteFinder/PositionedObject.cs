using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using RobotFootballCore.Interfaces;
using RobotFootballCore.Objects;

namespace RouteFinders
{
    [StructLayout(LayoutKind.Sequential, Pack=8)]
    public struct PositionedObject
    {
        public System.Drawing.Size Size;
        public System.Drawing.Point Position;
        

        public static implicit operator PositionedObject(Player obj)
        {
            return new PositionedObject { Position = obj.Position, Size = obj.Size };
        }
    }
}
