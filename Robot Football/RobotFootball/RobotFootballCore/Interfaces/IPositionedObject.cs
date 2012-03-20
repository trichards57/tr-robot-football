using System.Drawing;

namespace RobotFootballCore.Interfaces
{
    /// <summary>
    /// An interface that represents a object with position and size
    /// </summary>
    public interface IPositionedObject
    {
        /// <summary>
        /// The size of the object
        /// </summary>
        Point Position { get; }
        /// <summary>
        /// The position of the object
        /// </summary>
        Size Size { get; }
    }
}
