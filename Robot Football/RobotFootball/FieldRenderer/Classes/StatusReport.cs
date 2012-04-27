using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;

namespace FieldRenderer.Classes
{
    /// <summary>
    /// Represents a report of the status of the simulator, received from InterceptStrategy
    /// </summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    internal struct StatusReport
    {
        /// <summary>
        /// The environment produced by the simulator
        /// </summary>
        public Environment Environment;
        /// <summary>
        /// The direction of the field at the robot's current location
        /// </summary>
        public Vector3D FieldVector;
        /// <summary>
        /// The current velocity of the ball
        /// </summary>
        public Vector3D BallVelocity;
        /// <summary>
        /// The field configuration that is currently being used
        /// </summary>
        /// @todo Replace this with an enumerated variable for clarity
        public int FieldType;
    }
}