using System.Runtime.InteropServices;

namespace FieldRenderer.Classes
{
    /// <summary>
    /// An opposing robot
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    struct OpponentRobot
    {
        /// <summary>
        /// The position of the robot
        /// </summary>
        public Vector3D Position;
        /// <summary>
        /// The rotation of the robot
        /// </summary>
        public double Rotation;
    }
}