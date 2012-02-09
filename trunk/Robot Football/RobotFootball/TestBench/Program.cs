using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using Microsoft.ConcurrencyVisualizer.Instrumentation;
using RobotFootballCore.Objects;
using RouteFinders;

namespace TestBench
{
    class Program
    {
        static void Main(string[] args)
        {
            var field = new Field(5000, 1000);

            field.Players.AddRange(new[] { new Player(new Point(100, 100), Team.Current), new Player(new Point(200, 200), Team.Opposition) });

            var parRoutePlotter = new ParallelAStarRouteFinder();
            var routePlotter = new AStarRouteFinder();
            

            var span = Markers.EnterSpan("Parallel Route Finding");

            var route = parRoutePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));
            span.Leave();

            var span1 = Markers.EnterSpan("Series Route Finding");

            var route1 = routePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));

            span1.Leave();

        }
    }
}
