using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace RobotFootballCore.RouteObjects
{
    public abstract class RouteSegment
    {
        public PointF StartPoint { get; set; }
        public PointF EndPoint { get; set; }

        public abstract float Length { get; }
        public abstract void Draw(Graphics field);
        public abstract void Scale(SizeF scale);

        private static readonly Pen drawingPen = new Pen(Color.White, 2.5f);

        public static Pen DrawingPen { get { return drawingPen; } }
    }
}
