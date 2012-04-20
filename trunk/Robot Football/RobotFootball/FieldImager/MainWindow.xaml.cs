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

        private void CircularFieldClick(object sender, RoutedEventArgs e)
        {
            const int distance = 200;
            const int level = 20;

            var field = new double[fieldWidth * fieldHeight];

            Parallel.For(0, fieldWidth, x =>
                {
                    for (int y = 0 ; y < fieldHeight; y++)
                    {
                        var xDist = x - fieldCenterX;
                        var yDist = y - fieldCenterY;

                        var dist = Math.Sqrt(xDist * xDist + yDist * yDist);

                        float val;

                        if (dist < distance)
                            val = level;
                        else
                            val = 0;

                        field[x + y * fieldWidth] = val;
                    }
                });

            var gradField = GetGradient(field);

            var mainImage = GetImage(field);
            var gradImage = GetImage(gradField);


            Dispatcher.Invoke(new Action(() => { 
                MagnitudeImage.Source = BitmapSource.Create(fieldWidth, fieldHeight, 20, 20, PixelFormats.Rgb24, null, mainImage, fieldWidth * 3); 
                GradientImage.Source = BitmapSource.Create(fieldWidth, fieldHeight, 20, 20, PixelFormats.Rgb24, null, gradImage, fieldWidth * 3); 
            }));
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



    }
}
