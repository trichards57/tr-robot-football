using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;

namespace FieldRenderer.Classes
{
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    internal struct StatusReport
    {
        public Environment Environment;
        public Vector3D FieldVector;
        public Vector3D BallVelocity;
        public int FieldType;
    }
}