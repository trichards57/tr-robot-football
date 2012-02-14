using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using RobotFootballCore;
using System.IO;
using System.Drawing.Imaging;
using RobotFootballCore.Objects;
using System.Drawing;
using RouteFinders;
using System.Threading.Tasks;
using RobotFootballCore.RouteObjects;

namespace RobotFootballUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            field = new Field(1000, 1200);

            field.Players.AddRange(new[] { new Player(new System.Drawing.Point(100, 100), Team.Current), new Player(new System.Drawing.Point(200, 200), Team.Opposition), new Player(new System.Drawing.Point(200, 400), Team.Opposition), new Player(new System.Drawing.Point(400, 400), Team.Opposition) });

            var t = new Task(() => UpdateFieldDisplay());

            t.Start();
        }

        private Field field;

        private void UpdateFieldDisplay()
        {
            var image = field.ToBitmap();

            //var routePlotter = new AStarRouteFinder();
            //var route = routePlotter.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));

            //using (var graph = Graphics.FromImage(image))
            //{
            //    route.Draw(graph);
            //}

            var opponents = field.Players.Where(p => p.Team == Team.Opposition).Select(t => (PositionedObject)t).ToArray();

            var r = new MassedPotentialFieldRouteFinder();

            var route = r.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field, field.Players.First(p => p.Team == Team.Current));

            //var route2 = new System.Drawing.Point[1024]; for (var i = 0; i < 1024; i++) route2[i] = System.Drawing.Point.Empty;
            //NativeRouteFinder.HashMapAStarFindRoute(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field.Size, new System.Drawing.Size(10, 10), Player.PlayerSize, 20, opponents, opponents.Length, route2, (uint)route2.Length, true);
            //var route = new Route(route2);
            
            using (var graph = Graphics.FromImage(image))
            {
                route.Draw(graph);
            }

            Action refresh = () =>
            {
                using (var stream = new MemoryStream())
                {
                    image.Save(stream, ImageFormat.Png);
                    stream.Seek(0, SeekOrigin.Begin);

                    var decoder = BitmapDecoder.Create(stream, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);

                    var writeable = new WriteableBitmap(decoder.Frames.Single());
                    writeable.Freeze();

                    FieldImage.Source = writeable;
                }
            };

            Dispatcher.Invoke(refresh, null);
        }
    }
}
