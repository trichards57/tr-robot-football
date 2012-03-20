using System;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using RobotFootballCore.Objects;
using RouteFinders;

namespace TestBench
{
    /// <summary>
    /// The main TestBench program.
    /// </summary>
    class Program
    {
        static void Main()
        {
            var field = new Field(5000, 1000);

            field.Players.AddRange(new[] { new Player(new Point(100, 100), Team.Current), new Player(new Point(200, 200), Team.Opposition) });

            Console.WriteLine("Route Finder Test Bench");

            var timer = new Stopwatch();

            var opponents = field.Players.Where(p => p.Team == Team.Opposition).Select(t => (PositionedObject.FromIPositionedObject(t))).ToArray();
            var route4 = new Point[1024]; for (var i = 0; i < 1024; i++) route4[i] = Point.Empty;

            timer.Restart();
            NativeRouteFinder.AStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route4, (uint)route4.Length, true);
            timer.Stop();

            Console.WriteLine("Series Route Finder (Native, Non-Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            timer.Restart();
            NativeRouteFinder.HashMapAStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route4, (uint)route4.Length, true);
            timer.Stop();

            Console.WriteLine("Series Route Finder (Native, Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            Console.ReadLine();
        }
    }
}
