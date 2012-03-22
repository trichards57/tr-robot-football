using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media.Imaging;
using RobotFootballCore.Objects;
using RouteFinders;

namespace RobotFootballUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        /// <summary>
        /// Initializes a new instance of the MainWindow class
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Called when the window is loaded. Sets up the main field.
        /// </summary>
        /// <param name="sender">The object calling the event handler</param>
        /// <param name="e">The event arguments</param>
        /// <remarks>Calls the UpdateFieldDisplay method asyncrhonously to speed up window loading.</remarks>
        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            field = new Field(1000, 1200);

            field.Players.AddRange(new[] { new Player(new System.Drawing.Point(100, 100), Team.Current), new Player(new System.Drawing.Point(200, 200), Team.Opposition), new Player(new System.Drawing.Point(200, 400), Team.Opposition), new Player(new System.Drawing.Point(400, 400), Team.Opposition), new Player(new System.Drawing.Point(400, 200), Team.Opposition) });
            
            // Call UpdateFieldDisplay, but don't wait for the result before continuing the window load
            var t = new Task(UpdateFieldDisplay);
            t.Start();
        }

        /// <summary>
        /// The field to display
        /// </summary>
        private Field field;

        /// <summary>
        /// Updates the field display by rendering the field as a bitmap.
        /// </summary>
        /// Renders the field, as well as the result of a called route finder, and displays the results.
        private void UpdateFieldDisplay()
        {
            var image = field.ToBitmap();

            var r = new PotentialFieldRouteFinder();

            var route = PotentialFieldRouteFinder.FindPath(field.Players.First(p => p.Team == Team.Current).Position, field.Ball.Position, field);
            
            using (var graph = Graphics.FromImage(image))
            {
                route.Draw(graph);
            }

            // Anonymous function to convert the bitmap to a WriteableBitmap and display it in FieldImage.
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
            // Hand over the image display routine to the UI thread
            Dispatcher.Invoke(refresh, null);
        }
    }
}
