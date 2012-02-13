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
            var hashRoutePlotter = new HashSetAStarRouteFinder();
            var parallelHashRoutePlotter = new ParallelHashSetAStarRouteFinder();

            //var span = Markers.EnterSpan("Parallel Route Finding");

            //var route = parRoutePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));
            //span.Leave();

            var span1 = Markers.EnterSpan("Series Route Finding");

            var route1 = routePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));

            span1.Leave();

            span1 = Markers.EnterSpan("Hash Set Series Route Finding");

            var route1a = hashRoutePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));

            span1.Leave();

            span1 = Markers.EnterSpan("Parallel Hash Set Series Route Finding");

            var route1b = parallelHashRoutePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));

            span1.Leave();

            //var opponents = field.Players.Where(p => p.Team == Team.Opposition).Select(t => (PositionedObject)t).ToArray();
            //var route2 = new Point[1024]; for (var i = 0; i < 1024; i++) route2[i] = Point.Empty;
            //var span2 = Markers.EnterSpan("Native Route Finding (Trial Run)");
            
            //NativeRouteFinder.AStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route2, (uint)route2.Length, false);

            //span2.Leave();
            
            //var route3 = new Point[1024]; for (var i = 0; i < 1024; i++) route3[i] = Point.Empty;
            
            //var span3 = Markers.EnterSpan("Native Route Finding (Without SQRT)");
            
            //NativeRouteFinder.AStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route3, (uint)route2.Length, false);
            
            //span3.Leave();

            //var route4 = new Point[1024]; for (var i = 0; i < 1024; i++) route4[i] = Point.Empty;

            //var span4 = Markers.EnterSpan("Native Route Finding (With SQRT)");

            //NativeRouteFinder.AStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route4, (uint)route2.Length, true);

            //span4.Leave();
        }
    }
}
