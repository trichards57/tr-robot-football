using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;

namespace RobotFootballCore.Objects
{
    public struct NativeField
    {

    }

    public class Field
    {
        public int BorderWidth { get; set; }

        public List<Player> Players { get; set; }
        public Ball Ball { get; set; }

        public Size Size { get; private set; }

        public int Width { get { return Size.Width; } }
        public int Height { get { return Size.Height; } }

        public Field(int width, int height)
        {
            Size = new Size(width, height);
            
            Players = new List<Player>();
            Ball = new Ball(new Point(width / 2, height / 2));
            
            BorderWidth = 5;
        }

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
