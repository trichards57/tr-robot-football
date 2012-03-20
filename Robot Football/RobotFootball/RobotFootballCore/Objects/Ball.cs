using System.Drawing;
using RobotFootballCore.Interfaces;

namespace RobotFootballCore.Objects
{
    /// <summary>
    /// Represents the ball in the game
    /// </summary>
    /// 
    /// An object that represents the game ball, recording it's size and position, and rendering it when required.
    public class Ball : IPositionedObject
    {
        /// <summary>
        /// The size of the ball
        /// </summary>
        private static readonly Size BallSize = new Size(40, 40);
        /// <summary>
        /// The current position of the ball
        /// </summary>
        public Point Position { get; private set; }
        /// <summary>
        /// Renders the ball to the given Graphics object.
        /// </summary>
        /// <param name="field">The graphics object being used to create the field</param>
        /// Renders the ball as a yellow circle centered over the current Position of the Ball.
        public void DrawToField(Graphics field)
        {
            var startPoint = (PointF)Position - new SizeF(BallSize.Width / 2.0f, BallSize.Height / 2.0f);
            var rect = new RectangleF(startPoint, BallSize);

            field.FillEllipse(Brushes.Yellow, rect);
            field.DrawEllipse(Pens.Yellow, Rectangle.Round(rect));
        }
        /// <summary>
        /// Initializes a new instance of the Ball class
        /// </summary>
        /// <param name="position">The position the ball starts at</param>
        public Ball(Point position)
        {
            Position = position;
        }
        /// <summary>
        /// Gets the size of the ball
        /// </summary>
        public Size Size
        {
            get { return BallSize; }
        }
    }
}
