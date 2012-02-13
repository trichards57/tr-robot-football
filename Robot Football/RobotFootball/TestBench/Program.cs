using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
//using Microsoft.ConcurrencyVisualizer.Instrumentation;
using RobotFootballCore.Objects;
using RouteFinders;
using System.Diagnostics;

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

            Console.WriteLine("Route Finder Test Bench");

            var timer = new Stopwatch();

            //timer.Start();
            //var route1 = routePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));
            //timer.Stop();

            //Console.WriteLine("Serial Route Finder (Managed, Non-Hashmap) : {0}s", timer.Elapsed.TotalSeconds);

            //timer.Restart();
            //var route2 = hashRoutePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));
            //timer.Stop();

            //Console.WriteLine("Serial Route Finder (Managed, Hashmap) : {0}s", timer.Elapsed.TotalSeconds);

            //timer.Restart();
            //var route3 = parallelHashRoutePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));
            //timer.Stop();

            //Console.WriteLine("Parallel Route Finder (Managed, Hashmap) : {0}s", timer.Elapsed.TotalSeconds);

            //NativeRouteFinder.Init();

            var opponents = field.Players.Where(p => p.Team == Team.Opposition).Select(t => (PositionedObject)t).ToArray();
            var route4 = new System.Drawing.Point[1024]; for (var i = 0; i < 1024; i++) route4[i] = System.Drawing.Point.Empty;

            //timer.Restart();
            //NativeRouteFinder.AStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new System.Drawing.Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route4, (uint)route4.Length, true);
            //timer.Stop();

            //Console.WriteLine("Series Route Finder (Native, Non-Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            timer.Restart();
            NativeRouteFinder.AStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new System.Drawing.Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route4, (uint)route4.Length, true);
            timer.Stop();

            Console.WriteLine("Series Route Finder (Native, Non-Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            timer.Restart();
            NativeRouteFinder.HashMapAStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new System.Drawing.Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route4, (uint)route4.Length, true);
            timer.Stop();

            Console.WriteLine("Series Route Finder (Native, Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            Console.ReadLine();
        }
    }
}
