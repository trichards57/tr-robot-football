using System;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using RobotFootballCore.Objects;
using RouteFinders;
using System.Collections.Generic;
using RobotFootballCore.Interfaces;

/// @brief The testing classes
/// 
/// Intended to contain any classes used purely to test other classes.
namespace TestBench
{
    /// <summary>
    /// The main TestBench program.
    /// </summary>
    class Program
    {
        /// <summary>
        /// The initial function run by the runtime.
        /// </summary>
        static void Main()
        {
            var field = new Field(5000, 1000);

            field.Players.AddRange(new[] { new Player(new Point(100, 100), Team.Current), new Player(new Point(200, 200), Team.Opposition) });

            Console.WriteLine("Route Finder Test Bench");

            var timer = new Stopwatch();

            var opponents = field.Players.Where(p => p.Team == Team.Opposition).Select(t => (PositionedObject.FromIPositionedObject(t))).ToArray();
            var route4 = new Point[1024]; for (var i = 0; i < 1024; i++) route4[i] = Point.Empty;

            var finder1 = new ListAStarRouteFinder();
            var finder2 = new HashSetAStarRouteFinder();
            var finder3 = new ParallelListAStarRouteFinder();
            var finder4 = new ParallelHashSetAStarRouteFinder();

            timer.Restart();

            finder1.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));

            timer.Stop();

            Console.WriteLine("Series Route Finder (Native, Non-Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            timer.Restart();
            finder2.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));
            timer.Stop();

            Console.WriteLine("Series Route Finder (Native, Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            timer.Restart();
            finder3.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));
            timer.Stop();

            Console.WriteLine("Parallel Route Finder (Native, Non-Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            timer.Restart();
            finder4.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));
            timer.Stop();

            Console.WriteLine("Parallel Route Finder (Native, Hashmap, Square-Root) : {0}s", timer.Elapsed.TotalSeconds);

            Console.ReadLine();
        }
    }
}
