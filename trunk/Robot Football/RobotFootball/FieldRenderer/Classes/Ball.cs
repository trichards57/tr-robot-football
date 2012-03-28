using System.Runtime.InteropServices;

namespace FieldRenderer.Classes
{
    /// <summary>
    /// Represents the Ball structure used by the simulator.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    struct Ball
    {
        /// <summary>
        /// The position of the ball.
        /// </summary>
        public Vector3D Position;
    }
}
