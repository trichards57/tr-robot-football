using System.IO;
using System.Windows;
using System;
using System.Linq;
using System.Globalization;
using System.Threading;
using Microsoft.Win32;

namespace DataDumpProcessor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        /// <summary>
        /// The view model used to store data for the window.
        /// </summary>
        private readonly ViewModel viewModel = new ViewModel();

        /// <summary>
        /// Initializes a new instance of the MainWindow class.
        /// </summary>
        public MainWindow()
        {
            DataContext = viewModel;

            InitializeComponent();
        }

        /// <summary>
        /// Prepares the view model data for use in MATLAB
        /// </summary>
        /// <param name="sender">The object where the event handler is attached.</param>
        /// <param name="e">The event data</param>
        /// 
        /// Prepares all the view model data for use in MATLAB.  Normalises the time entries (so that each run starts at time=0) and calculates the
        /// instantaneous velocity (relative to the input command to the motors).
        /// <remarks>Stores the prepared data file in My Documents, with the name data.csv</remarks>
        /// @bug Will overwrite any existing file called data.csv in My Documents
        private void MatlabPrepare(object sender, RoutedEventArgs e)
        {
            // Store the file in My Documents
            using (var dataFile = File.OpenWrite(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "data.csv")))
            {
                var writer = new StreamWriter(dataFile);

                // Create the file headers
                writer.WriteLine("time, x position, x velocity");

                foreach (var dat in viewModel.DataEntries.OfType<DataEntries<TimedDataEntry>>())
                {
                    var baseLine = dat.Points[0];
                    for (var i = 1; i < dat.Points.Count; i++)
                    {
                        var p = dat.Points[i];
                        writer.WriteLine("{0}, {1}, {2}", 
                            p.Time - baseLine.Time, // Normalise the time
                            p.Point.X, 
                            ((p.Point.X - dat.Points[i - 1].Point.X) / (p.Time - dat.Points[i - 1].Time)) / dat.Velocity); // Normalise the velocity 
                    }
                }
            }
        }

        /// <summary>
        /// Allows the user to select a data dump file.
        /// </summary>
        /// <param name="sender">The object where the event handler is attached.</param>
        /// <param name="e">The event data</param>
        /// 
        /// Allows the user to use any csv file as an input file to the data processing.
        private void BrowseClick(object sender, RoutedEventArgs e)
        {
            var dialog = new OpenFileDialog { Filter = "CSV File|*.csv" };
            var result = dialog.ShowDialog(this);
            if (result == true)
                viewModel.DataFilePath = dialog.FileName;
        }

        /// <summary>
        /// Reloads the selected data file.
        /// </summary>
        /// <param name="sender">The object where the event handler is attached.</param>
        /// <param name="e">The event data</param>
        /// 
        /// Reloads the selected data file, parsing every entry in the file into a collection of DataEntries.
        /// Data entries are identified based on the number of columns in the entry.  
        /// 
        /// * The first line that matches the required date and time format identifies the data entry.
        /// 
        /// * A line with two columns is taken as an entry with only point data
        /// 
        /// * A line with three columns is taken as an entry with a time marker and point data
        /// 
        /// * A line with four columns is taken as an entry with a high-resolution time marker and point data
        /// 
        /// * A line with six columns is taken as an entry with a high-resolution time marker, the high-resolution timer frequency and point data
        /// 
        /// * All other lines are ignored
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
            var entryDate = DateTime.Now;
            var vel = 0;
            foreach (var parts in lines.Where(m => !string.IsNullOrEmpty(m)).Select(l => l.Split(new[] { "," }, StringSplitOptions.RemoveEmptyEntries)))
            {
                DateTime date;
                if (DateTime.TryParseExact(parts[0], "ddd MMM dd HH:mm:ss yyyy" , Thread.CurrentThread.CurrentCulture.DateTimeFormat, DateTimeStyles.AllowWhiteSpaces, out date))
                {
                    entryDate = date;
                    dataEntries = null;
                }
                else
                {
                    double time;
                    double x, y;
                    long t1, t2;
                    switch (parts.Length)
                    {
                        case 1:
                            int.TryParse(parts[0], out vel);
                            break;
                        case 2:
                            if (!(double.TryParse(parts[0], out x) && (double.TryParse(parts[1], out y))))
                                throw new InvalidOperationException();
                            if (dataEntries == null || !(dataEntries is DataEntries<Point>))
                            {
                                dataEntries = new DataEntries<Point> { Velocity = vel, EntryDate = entryDate };
                                viewModel.DataEntries.Add(dataEntries);
                            }

                            ((DataEntries<Point>)dataEntries).Points.Add(new Point(x, y));
                            break;
                        case 3:
                            if (!(double.TryParse(parts[0], out time) && double.TryParse(parts[1], out x) && double.TryParse(parts[2], out y)))
                                throw new InvalidOperationException();
                            if (dataEntries == null || !(dataEntries is DataEntries<TimedDataEntry>))
                            {
                                dataEntries = new DataEntries<TimedDataEntry> { Velocity = vel, EntryDate = entryDate };
                                viewModel.DataEntries.Add(dataEntries);
                            }

                            ((DataEntries<TimedDataEntry>)dataEntries).Points.Add(new TimedDataEntry { Point = new Point(x, y), Time = time });
                            break;
                        case 4:
                            if (!(long.TryParse(parts[0], out t1) && long.TryParse(parts[1], out t2) && double.TryParse(parts[2], out x) && double.TryParse(parts[3], out y)))
                                throw new InvalidOperationException();
                            if (dataEntries == null || !(dataEntries is DataEntries<TimedDataEntry>))
                            {
                                dataEntries = new DataEntries<TimedDataEntry> { Velocity = vel, EntryDate = entryDate };
                                viewModel.DataEntries.Add(dataEntries);
                            }
                            time = (t1 << 32) + t2;
                            ((DataEntries<TimedDataEntry>)dataEntries).Points.Add(new TimedDataEntry { Point = new Point(x, y), Time = time });
                            break;
                        case 6:
                            long f1;
                            long f2;
                            if (!(long.TryParse(parts[0], out f1) && long.TryParse(parts[1], out f2) && long.TryParse(parts[2], out t1) && long.TryParse(parts[3], out t2) && double.TryParse(parts[4], out x) && double.TryParse(parts[5], out y)))
                                throw new InvalidOperationException();
                            if (dataEntries == null || !(dataEntries is DataEntries<TimedDataEntry>))
                            {
                                dataEntries = new DataEntries<TimedDataEntry> { Velocity = vel, EntryDate = entryDate };
                                viewModel.DataEntries.Add(dataEntries);
                            }
                            var freq = (f1 << 32) + f2;
                            time = (double)((t1 << 32) + t2) / freq;
                            ((DataEntries<TimedDataEntry>)dataEntries).Points.Add(new TimedDataEntry { Point = new Point(x, y), Time = time });
                            break;
                        default:
                            continue;
                    }
                }
            }
        }
    }
}
