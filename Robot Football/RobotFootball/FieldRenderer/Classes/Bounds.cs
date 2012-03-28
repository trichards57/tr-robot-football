using System.Runtime.InteropServices;

namespace FieldRenderer.Classes
{
    /// <summary>
    /// Represents the bounds of the pitch used by the simulator
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    struct Bounds
    {
        /// <summary>
        /// The position of the left edge
        /// </summary>
        public int Left;
        /// <summary>
        /// The position of the right edge
        /// </summary>
        public int Right;
        /// <summary>
        /// The position of the top edge
        /// </summary>
        public int Top;
        /// <summary>
        /// The position of the bottom edge.
        /// </summary>
        public int Bottom;
    }
}
