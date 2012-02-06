using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using RobotFootballCore.Interfaces;

namespace RobotFootballCore.Objects
{
    public class Player : IPositionedObject
    {
        public static readonly SizeF PlayerSize = new SizeF(50, 50);

        public PointF Position
        {
            get;
            private set;
        }

        public Player(PointF startPosition, Team team)
        {
            Position = startPosition;

            Team = team;
        }

        public void DrawToField(Graphics field)
        {
            var brush = Team == Team.Current ? Brushes.Green : Brushes.Red;

            var startPoint = Position - new SizeF(PlayerSize.Width / 2, PlayerSize.Height / 2);
            var rect = new RectangleF(startPoint, PlayerSize);

            field.FillRectangle(brush, rect);
            field.DrawRectangle(Pens.Yellow, Rectangle.Round(rect));
        }

        public Team Team { get; private set; }
    }
}
