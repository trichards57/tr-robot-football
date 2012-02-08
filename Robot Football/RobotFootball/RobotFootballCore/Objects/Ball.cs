using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using RobotFootballCore.Interfaces;

namespace RobotFootballCore.Objects
{
    public class Ball : IPositionedObject
    {
        public static readonly Size BallSize = new Size(40, 40);

        public Point Position { get; private set; }

        public void DrawToField(Graphics field)
        {
            var startPoint = (PointF)Position - new SizeF(BallSize.Width / 2, BallSize.Height / 2);
            var rect = new RectangleF(startPoint, BallSize);

            field.FillEllipse(Brushes.Yellow, rect);
            field.DrawEllipse(Pens.Yellow, Rectangle.Round(rect));
        }

        public Ball(Point position)
        {
            Position = position;
        }
        
        public Size Size
        {
            get { return BallSize; }
        }
    }
}
