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
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.IO;

namespace FieldImager
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private const int fieldWidth = 1001;
        private const int fieldHeight = 1001;
        private const int fieldCenterX = 500;
        private const int fieldCenterY = 500;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void CircularRepelFieldClick(object sender, RoutedEventArgs e)
        {
            const int distance = 200;
            const int level = 20;

            GetField((x, y, field) =>
            {
                var xDist = x - fieldCenterX;
                var yDist = y - fieldCenterY;

                var dist = Math.Sqrt(xDist * xDist + yDist * yDist);

                double val;

                if (dist < distance)
                    val = level;
                else
                    val = 0;

                field[x + y * fieldWidth] = val;
            });
        }

        private byte[] GetImage(double[] data)
        {
            var buffer = new byte[fieldWidth * fieldHeight * 3];

            var max = data.Max();
            var min = data.Min();

            Parallel.For(0, data.Length, i =>
                {
                    var val = (byte)Math.Floor((data[i] - min) * 255 / max);
                    buffer[3 * i] = val;
                    buffer[3 * i + 1] = val;
                    buffer[3 * i + 2] = val;
                });

            return buffer;
        }

        private double[] GetGradient(double[] field)
        {
            var gradientField = new double[fieldWidth * fieldHeight];
            gradientField.Initialize();

            Parallel.For(1, fieldWidth - 1, x =>
                {
                    for (int y = 1; y < fieldHeight - 1; y++)
                    {
                        var xGrad = (field[x - 1 + y * fieldWidth] - field[x + 1 + y * fieldWidth]);
                        var yGrad = (field[x + (y - 1) * fieldWidth] - field[x + (y + 1) * fieldWidth]);
                        var grad = Math.Sqrt(xGrad * xGrad + yGrad * yGrad);
                        gradientField[x + y * fieldWidth] = grad;
                    }
                });

            return gradientField;
        }

        private void SaveImageClick(object sender, RoutedEventArgs e)
        {
            var dlg = new SaveFileDialog
            {
                CheckPathExists = true,
                DefaultExt = "png",
                Filter = "PNG File|*.png|All Files|*.*",
                OverwritePrompt = true,
                ValidateNames = true,
                Title = "Save Field Image"
            };
            var res = dlg.ShowDialog(this);

            if (res == true)
            {
                using (var file = File.OpenWrite(dlg.FileName))
                {
                    var encoder = new PngBitmapEncoder();
                    encoder.Frames.Add(BitmapFrame.Create(MagnitudeImage.Source as BitmapSource));
                    encoder.Save(file);
                }
            }

            dlg.Title = "Save Gradient Image";
            res = dlg.ShowDialog(this);

            if (res == true)
            {
                using (var file = File.OpenWrite(dlg.FileName))
                {
                    var encoder = new PngBitmapEncoder();
                    encoder.Frames.Add(BitmapFrame.Create(GradientImage.Source as BitmapSource));
                    encoder.Save(file);
                }
            }
        }

        private void GetField(Action<int, int, double[]> renderCode)
        {
            var field = new double[fieldWidth * fieldHeight];

            Parallel.For(0, fieldWidth, x =>
                {
                    for (int y = 0; y < fieldHeight; y++)
                    {
                        renderCode(x, y, field);
                    }
                });

            var gradField = GetGradient(field);

            var mainImage = GetImage(field);
            var gradImage = GetImage(gradField);


            Dispatcher.Invoke(new Action(() =>
            {
                MagnitudeImage.Source = BitmapSource.Create(fieldWidth, fieldHeight, 20, 20, PixelFormats.Rgb24, null, mainImage, fieldWidth * 3);
                GradientImage.Source = BitmapSource.Create(fieldWidth, fieldHeight, 20, 20, PixelFormats.Rgb24, null, gradImage, fieldWidth * 3);
            }));
        }

        private void ConicRepelFieldClick(object sender, RoutedEventArgs e)
        {
            const int distance = 200;
            const int level = 20;

            GetField((x, y, field) =>
            {
                var xDist = x - fieldCenterX;
                var yDist = y - fieldCenterY;

                var dist = Math.Sqrt(xDist * xDist + yDist * yDist);

                double val;

                if (dist < distance)
                    val = (distance - dist) * level;
                else
                    val = 0;

                field[x + y * fieldWidth] = val;
            });
        }

        private void GaussianFieldClick(object sender, RoutedEventArgs e)
        {
            GetField((x, y, field) =>
            {
                var xDist = x - fieldCenterX;
                var yDist = y - fieldCenterY;

                var dist = xDist * xDist + yDist * yDist;
                var val = Math.Exp(-dist / 50000.0);

                field[x + y * fieldWidth] = val;
            });
        }

        private void StretchedGaussianFieldClick(object sender, RoutedEventArgs e)
        {
            GetField((x, y, field) =>
            {
                var xDist = x - fieldCenterX;
                var yDist = y - fieldCenterY;

                var dist = xDist * xDist/60000.0 + yDist * yDist/20000.0;
                var val = Math.Exp(-dist );

                field[x + y * fieldWidth] = val;
            });
        }

        private void ShapedFieldClick(object sender, RoutedEventArgs e)
        {
            GetField((x, y, field) =>
            {
                var xDist = x - fieldCenterX;
                var yDist = y - fieldCenterY;

                var dist = xDist * xDist + yDist * yDist;

                var angle = Math.Atan2(yDist, xDist);

                var val = Math.Exp(-dist / 70000.0) * Math.Abs((angle) / Math.PI);

                field[x + y * fieldWidth] = val;
            });
        }

        private void PairedFieldClick(object sender, RoutedEventArgs e)
        {
            const int level = 20;
            const int offset = 300;

            GetField((x, y, field) =>
            {
                var xDist = x - (fieldCenterX - offset);
                var yDist = y - (fieldCenterY);

                var dist = Math.Sqrt(xDist * xDist + yDist * yDist);

                var val = dist * level;

                field[x + y * fieldWidth] = val;

                xDist = x - (fieldCenterX + offset);
                yDist = y - (fieldCenterY);

                dist = xDist * xDist + yDist * yDist;
                val = Math.Exp(-dist / 10000.0);

                field[x + y * fieldWidth] += val*3000;
            });
        }

    }
}
