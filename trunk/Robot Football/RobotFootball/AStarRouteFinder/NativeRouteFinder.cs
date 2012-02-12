using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using RobotFootballCore.Interfaces;
using RobotFootballCore.Objects;

namespace RouteFinders
{
    public static class NativeRouteFinder
    {
        [DllImport("NativeRouteFinders.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern void AStarFindRoute(Point startPoint, Point endPoint,
                                                 Size fieldSize, Size resolution,
                                                 Size movingObjectSize, int objectClearance,
                                                 [MarshalAs(UnmanagedType.LPArray, SizeParamIndex=7)]PositionedObject[] opponents, int opponentsCount,
                                                 [In, Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=9)]Point[] routeResult, uint routeResultLength);
                                                 
    }
}
