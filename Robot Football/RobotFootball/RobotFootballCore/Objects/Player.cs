using System.Drawing;
using RobotFootballCore.Interfaces;

namespace RobotFootballCore.Objects
{
    /// <summary>
    /// Represents a Player in the Game
    /// </summary>
    public class Player : IPositionedObject
    {
        /// <summary>
        /// The size of every Player
        /// </summary>
        public static readonly Size PlayerSize = new Size(50, 50);
        /// <summary>
        /// The position of the Player
        /// </summary>
        public Point Position
        {
            get;
            private set;
        }
        /// <summary>
        /// Initializes a new instance of the Player class
        /// </summary>
        /// <param name="startPosition">The position the player will start at</param>
        /// <param name="team">The team the player is on</param>
        public Player(Point startPosition, Team team)
        {
            Position = startPosition;

            Team = team;
        }
        /// <summary>
        /// Renders the player to the provided Graphics context
        /// </summary>
        /// <param name="field">The Graphics context used to draw the field</param>
        public void DrawToField(Graphics field)
        {
            var brush = Team == Team.Current ? Brushes.Green : Brushes.Red;

            var startPoint = (PointF)Position - new SizeF(PlayerSize.Width / 2.0f, PlayerSize.Height / 2.0f);
            var rect = new RectangleF(startPoint, PlayerSize);

            field.FillRectangle(brush, rect);
            field.DrawRectangle(Pens.Yellow, Rectangle.Round(rect));
        }
        /// <summary>
        /// The team the player is on.
        /// </summary>
        public Team Team { get; private set; }

        /// <summary>
        /// The size of the player
        /// </summary>
        /// <remarks>Taken from PlayerSize</remarks>
        public Size Size
        {
            get { return PlayerSize; }
        }
    }
}
