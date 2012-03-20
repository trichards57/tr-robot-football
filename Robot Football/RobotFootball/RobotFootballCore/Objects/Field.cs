using System.Collections.Generic;
using System.Drawing;

namespace RobotFootballCore.Objects
{
    /// <summary>
    /// Represents the game playing field.
    /// </summary>
    public class Field
    {
        /// <summary>
        /// The width of the field border when rendered
        /// </summary>
        private int BorderWidth { get; set; }
        /// <summary>
        /// The current players from both teams
        /// </summary>
        public List<Player> Players { get; private set; }
        /// <summary>
        /// The Ball used by the game
        /// </summary>
        public Ball Ball { get; private set; }
        /// <summary>
        /// The size of the field
        /// </summary>
        public Size Size { get; private set; }
        /// <summary>
        /// The width of the field
        /// </summary>
        /// <see cref="Size"/>
        /// <remarks>Taken from the Size property</remarks>
        public int Width { get { return Size.Width; } }
        /// <summary>
        /// The height of the field
        /// </summary>
        /// <see cref="Size"/>
        /// <remarks>Taken from the Size property</remarks>
        public int Height { get { return Size.Height; } }
        /// <summary>
        /// Initializes a new instance of the Field class
        /// </summary>
        /// <param name="width">The width of the new field</param>
        /// <param name="height">The height of the new field</param>
        /// 
        /// Also places the ball at the center of the field.
        public Field(int width, int height)
        {
            Size = new Size(width, height);
            
            Players = new List<Player>();
            Ball = new Ball(new Point(width / 2, height / 2));
            
            BorderWidth = 5;
        }
        /// <summary>
        /// Renders the field onto a Bitmap.
        /// </summary>
        /// <returns>A Bitmap containing a representation of the Field</returns>
        /// 
        /// Renders each player in turn, as well as the ball, based on the current information.
        /// 
        /// <seealso cref="Player::DrawToField"/>
        /// <seealso cref="Ball::DrawToField"/>
        public Bitmap ToBitmap()
        {
            var outputSize = Size + new Size(2 * BorderWidth, 2 * BorderWidth);
            var output = new Bitmap(outputSize.Width, outputSize.Height);

            using (var graph = Graphics.FromImage(output))
            {
                graph.Clear(Color.Black);
                graph.DrawRectangle(new Pen(Color.Blue, BorderWidth), BorderWidth / 2, BorderWidth / 2, Width + BorderWidth, Height + BorderWidth);

                foreach (var p in Players)
                    p.DrawToField(graph);
                Ball.DrawToField(graph);
            }

            return output;
        }

        
    }
}
