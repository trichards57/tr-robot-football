using System.Runtime.InteropServices;

namespace FieldRenderer.Classes
{
    /// <summary>
    /// A friendly robot
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    struct Robot
    {
        /// <summary>
        /// The position of the robot
        /// </summary>
        public Vector3D Position;
        /// <summary>
        /// The orientation of the robot
        /// </summary>
        public double Rotation;
        /// <summary>
        /// The robot's left motor speed
        /// </summary>
        public double VelocityLeft;
        /// <summary>
        /// The robot's right motor speed
        /// </summary>
        public double VelocityRight;
    }
}
