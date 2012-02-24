using System.IO;
using System.Windows;
using System;
using System.Linq;
using System.Globalization;
using System.Threading;

namespace DataDumpProcessor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        private ViewModel viewModel = new ViewModel();

        public MainWindow()
        {
            DataContext = viewModel;

            InitializeComponent();
        }

        private void ReloadFile(object sender, RoutedEventArgs e)
        {
            if (!File.Exists(viewModel.DataFilePath))
            {
                MessageBox.Show("Data file does not exist.", "Data File Not Found", MessageBoxButton.OK, MessageBoxImage.Stop);
                return;
            }

            var lines = File.ReadAllLines(viewModel.DataFilePath);
            viewModel.DataEntries.Clear();
            DataEntries dataEntries = null;
            DateTime date;
            DateTime entryDate = DateTime.Now;
            foreach (var l in lines.Where(m => !string.IsNullOrEmpty(m)))
            {
                if (DateTime.TryParseExact(l, "ddd MMM dd HH:mm:ss yyyy" , Thread.CurrentThread.CurrentCulture.DateTimeFormat, DateTimeStyles.AllowWhiteSpaces, out date))
                {
                    entryDate = date;
                    dataEntries = null;
                    continue;
                }
                else
                {
                    var parts = l.Split(new[] { "," }, StringSplitOptions.RemoveEmptyEntries);
                    long time;
                    double x, y;
                    switch (parts.Length)
                    {
                        case 2:
                            if (!(double.TryParse(parts[0], out x) && (double.TryParse(parts[1], out y))))
                                throw new InvalidOperationException();
                            if (dataEntries == null || !(dataEntries is RawPointDataEntries))
                            {
                                dataEntries = new RawPointDataEntries();
                                dataEntries.EntryDate = entryDate;
                                viewModel.DataEntries.Add(dataEntries);
                            }
                            
                            ((RawPointDataEntries)dataEntries).Points.Add(new Point(x, y));
                            break;
                        case 3:
                            if (!(long.TryParse(parts[0], out time) && double.TryParse(parts[1], out x) && double.TryParse(parts[2], out y)))
                                throw new InvalidOperationException();
                            if (dataEntries == null || !(dataEntries is TimedDataEntries))
                            {
                                dataEntries = new TimedDataEntries();
                                dataEntries.EntryDate = entryDate;
                                viewModel.DataEntries.Add(dataEntries);
                            }

                            ((TimedDataEntries)dataEntries).Points.Add(new TimedDataEntry { Point = new Point(x, y), Time = time });
                            break;
                        default:
                            continue;
                    }
                }
            }
        }
    }
}
