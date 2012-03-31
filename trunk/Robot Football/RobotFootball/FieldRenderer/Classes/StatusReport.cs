using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Windows;
using System.Linq;
using System.Text;

namespace FieldRenderer.Classes
{
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    struct StatusReport
    {
        public Environment Environment;
        public Vector3D FieldVector;
    }
}
