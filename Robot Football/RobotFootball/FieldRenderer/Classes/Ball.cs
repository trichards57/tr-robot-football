using System.Runtime.InteropServices;

/// @brief The field renderer's data model classes
/// 
/// Intended to contain all the classes used to represent data for the field renderer.
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
