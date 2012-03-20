using System.Runtime.InteropServices;
using RobotFootballCore.Interfaces;

namespace RouteFinders
{
    /// <summary>
    /// An object with position and size
    /// </summary>
    /// Represents an object with position and size that is used for a route finding algorithm.
    /// 
    /// Maps to NativeRouteFinders::PositionedObject for native interop purposes.
    /// 
    /// Uses sequential layout, with packing alignment set to 8.
    /// <see cref="NativeRouteFinders::PositionedObject"/>
    [StructLayout(LayoutKind.Sequential, Pack=8)]
    public struct PositionedObject
    {
        /// <summary>
        /// The size of the object
        /// </summary>
        public System.Drawing.Size Size;
        /// <summary>
        /// The position of the object
        /// </summary>
        public System.Drawing.Point Position;

        /// <summary>
        /// Converts an IPositionedObject to a PositionedObject for interop purposes.
        /// </summary>
        /// <param name="obj">The object to convert</param>
        /// <returns>A PositionedObject that represents (but is not linked to) the IPositionedObject</returns>
        public static PositionedObject FromIPositionedObject(IPositionedObject obj)
        {
            return new PositionedObject { Position = obj.Position, Size = obj.Size };
        }
    }
}
