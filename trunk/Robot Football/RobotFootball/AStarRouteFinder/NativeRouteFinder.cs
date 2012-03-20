using System.Drawing;
using System.Runtime.InteropServices;

namespace RouteFinders
{
    /// <summary>
    /// Interop class used to access the NativeRouteFinder functions.
    /// </summary>
    public static class NativeRouteFinder
    {
        /// <summary>
        /// Imports the NativeRouteFinder::AStarFindRoute function.
        /// </summary>
        /// <param name="startPoint">The point to start the algorithm from.</param>
        /// <param name="endPoint">The point to find a route to</param>
        /// <param name="fieldSize">The size of the field that will be searched.</param>
        /// <param name="resolution">The size of each square in the field, once it has been discretised</param>
        /// <param name="movingObjectSize">The size of the object that will follow the route</param>
        /// <param name="objectClearance">The distance that obstacles must be cleared by</param>
        /// <param name="opponents">The opponents that act as obstacles</param>
        /// <param name="opponentsCount">The length of the opponents array</param>
        /// <param name="routeResult">The route that has been produced by the algorithm (an output parameter)</param>
        /// <param name="routeResultLength">The length of the routeResult array</param>
        /// <param name="useSqrt">If false, all distance algorithms skip the square root step of the calculation</param>
        /// <see cref="NativeRouteFinders::AStarFindRoute"/>
        [DllImport("NativeRouteFinders.dll", CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern void AStarFindRoute(Point startPoint, Point endPoint,
                                                 Size fieldSize, Size resolution,
                                                 Size movingObjectSize, int objectClearance,
                                                 [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 7)]PositionedObject[] opponents, int opponentsCount,
                                                 [In, Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 9)]Point[] routeResult, uint routeResultLength,
                                                 bool useSqrt);
        [DllImport("NativeRouteFinders.dll", CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern void HashMapAStarFindRoute(Point startPoint, Point endPoint,
                                                 Size fieldSize, Size resolution,
                                                 Size movingObjectSize, int objectClearance,
                                                 [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 7)]PositionedObject[] opponents, int opponentsCount,
                                                 [In, Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 9)]Point[] routeResult, uint routeResultLength,
                                                 bool useSqrt);

        [DllImport("NativeRouteFinders.dll", CallingConvention = CallingConvention.Cdecl, PreserveSig = false)]
        public static extern void Init();
    }
}
