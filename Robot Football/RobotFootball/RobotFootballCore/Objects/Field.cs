using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;

namespace RobotFootballCore.Objects
{
    public class Field
    {
        

        public int BorderWidth { get; set; }

        public List<Player> Players { get; set; }
        public Ball Ball { get; set; }

        public int Width { get; private set; }
        public int Height { get; private set; }

        public Field(int width, int height)
        {
            Width = width;
            Height = height;
            
            Players = new List<Player>();
            Ball = new Ball(new PointF(((float)width) / 2, ((float)height) / 2));
            
            BorderWidth = 5;
        }

        public Bitmap ToBitmap()
        {
            var output = new Bitmap(Width + 2 * BorderWidth, Height + 2 * BorderWidth);

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
