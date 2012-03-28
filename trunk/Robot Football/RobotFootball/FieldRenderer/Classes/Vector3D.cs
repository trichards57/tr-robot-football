using System.Runtime.InteropServices;

namespace FieldRenderer.Classes
{
    /// <summary>
    /// A 3D vector used by the simulator
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    struct Vector3D
    {
        /// <summary>
        /// The X component of the vector
        /// </summary>
        public double X;
        /// <summary>
        /// The Y component of the vector
        /// </summary>
        public double Y;
        /// <summary>
        /// The Z component of the vector
        /// </summary>
        public double Z;
    }
}