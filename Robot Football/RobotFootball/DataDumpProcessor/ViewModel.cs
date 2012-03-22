using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using Visiblox.Charts;

namespace DataDumpProcessor
{
    /// <summary>
    /// The view model used to display data series
    /// </summary>
    class SeriesViewModel : INotifyPropertyChanged
    {
        /// <summary>
        /// The chart series to display
        /// </summary>
        private readonly ChartSeriesCommonBase series;

        /// <summary>
        /// Initializes a new instance of the SeriesViewModel class
        /// </summary>
        /// <param name="series">The series to display</param>
        public SeriesViewModel(ChartSeriesCommonBase series)
        {
            this.series = series;
        }

        /// <summary>
        /// Gets or sets the title of the data series.
        /// </summary>
        public string Title
        {
            get
            {
                return series.DataSeries.Title;
            }
        }

        /// <summary>
        /// Gets or sets the visibility of the data series.
        /// </summary>
        public bool Visible
        {
            get
            {
                return series.Visibility == Visibility.Visible;
            }
            set
            {
                series.Visibility = value ? Visibility.Visible : Visibility.Hidden;

                OnPropertyChanged("Visible");
            }
        }

        /// <summary>
        /// Invoked whenever a property on this view model changes.
        /// </summary>
        /// <param name="property">The name of the property that has changed.</param>
        private void OnPropertyChanged(string property)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property));
        }

        /// <summary>
        /// Occurs when a property value changes.
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;
    }

    /// <summary>
    /// Represents the range the secondary scale should use
    /// </summary>
    public enum ScaleRange
    {
        /// <summary>
        /// Represents a range large enough to show both sets of data
        /// </summary>
        Both = 0,
        /// <summary>
        /// Represents a range large enough to show the velocity data
        /// </summary>
        Velocity,
        /// <summary>
        /// Represents a range large enough to show the acceleration data
        /// </summary>
        Acceleration
    }

    /// <summary>
    /// The view model used to display data in the main window
    /// </summary>
    class ViewModel : INotifyPropertyChanged
    {
        /// <summary>
        /// Initializes a new instance of the ViewModel class
        /// </summary>
        public ViewModel()
        {
            DataFilePath = @"H:\DataDump.csv";
            DataEntries = new ObservableCollection<DataEntries>();
            SeriesSettings = new ObservableCollection<SeriesViewModel>();
        }

        /// <summary>
        /// The main x-axis used by the main graph
        /// </summary>
        private IAxis mainXAxis = new LinearAxis();
        /// <summary>
        /// The main y-axis used by the main graph
        /// </summary>
        private IAxis mainYAxis = new LinearAxis();
        /// <summary>
        /// The secondary y-axis used by the main graph
        /// </summary>
        private IAxis secondYAxis = new LinearAxis();

        /// <summary>
        /// Gets or sets the main x-axis used by the main graph
        /// </summary>
        public IAxis MainXAxis
        {
            get { return mainXAxis; }
            set
            {
                mainXAxis = value;
                OnPropertyChanged("MainXAxis");
            }
        }
        /// <summary>
        /// Gets or sets the main y-axis used by the main graph
        /// </summary>
        public IAxis MainYAxis
        {
            get { return mainYAxis; }
            set
            {
                mainYAxis = value;
                OnPropertyChanged("MainYAxis");
            }
        }
        /// <summary>
        /// Gets or sets the secondary y-axis used by the main graph
        /// </summary>
        public IAxis SecondYAxis
        {
            get { return secondYAxis; }
            set
            {
                secondYAxis = value;
                OnPropertyChanged("SecondYAxis");
            }
        }
        /// <summary>
        /// The maximum velocity found
        /// </summary>
        private double maxVel;
        /// <summary>
        /// The minimum velocity found
        /// </summary>
        private double minVel;
        /// <summary>
        /// The maximum acceleration found
        /// </summary>
        private double maxAccel; 
        /// <summary>
        /// The minimum acceleration found
        /// </summary>
        private double minAccel;
        /// <summary>
        /// The path to the current data file
        /// </summary>
        private string dataFilePath;
        /// <summary>
        /// The selected data entries
        /// </summary>
        private DataEntries selectedDataEntries;
        /// <summary>
        /// The range to use for the secondary y-axis
        /// </summary>
        private ScaleRange scaleRange;
        /// <summary>
        /// Gets or sets if the graph should use a secondary axis large enough for both the acceleration and the velocity readings
        /// </summary>
        public bool BothScaleRange
        {
            get
            {
                return scaleRange == ScaleRange.Both;
            }
            set
            {
                if (!value || scaleRange == ScaleRange.Both) return;
                scaleRange = ScaleRange.Both;
                OnScaleRangeChanged();
            }
        }
        /// <summary>
        /// Invoked whenever the BothScaleRange, VelocityScaleRange or AccelerationScaleRange changes
        /// </summary>
        /// <remarks>
        /// Invokes OnPropertyChanged for all three properties, regardless of which one changed.
        /// </remarks>
        private void OnScaleRangeChanged()
        {
            switch (scaleRange)
            {
                case ScaleRange.Acceleration:
                    SecondYAxis.Range = new DoubleRange(minAccel, maxAccel);
                    break;
                case ScaleRange.Velocity:
                    SecondYAxis.Range = new DoubleRange(minVel, maxVel);
                    break;
                case ScaleRange.Both:
                    SecondYAxis.Range = new DoubleRange(Math.Min(minAccel, minVel), Math.Max(maxAccel, maxVel));
                    break;
            }
            OnPropertyChanged("BothScaleRange");
            OnPropertyChanged("VelocityScaleRange");
            OnPropertyChanged("AccelerationScaleRange");
        }
        /// <summary>
        /// Gets or sets if the graph should use a secondary axis large enough for the velocity readings
        /// </summary>
        public bool VelocityScaleRange
        {
            get
            {
                return scaleRange == ScaleRange.Velocity;
            }
            set
            {
                if (!value || scaleRange == ScaleRange.Velocity) return;
                scaleRange = ScaleRange.Velocity;
                OnScaleRangeChanged();
            }
        }
        /// <summary>
        /// Gets or sets if the graph should use a secondary axis large enough for the acceleration readings
        /// </summary>
        public bool AccelerationScaleRange
        {
            get
            {
                return scaleRange == ScaleRange.Acceleration;
            }
            set
            {
                if (!value || scaleRange == ScaleRange.Acceleration) return;
                scaleRange = ScaleRange.Acceleration;
                OnScaleRangeChanged();
            }
        }
        /// <summary>
        /// Gets or sets the path to the current data file
        /// </summary>
        public string DataFilePath
        {
            get { return dataFilePath; }
            set 
            {
                dataFilePath = value;
                OnPropertyChanged("DataFilePath");
            }
        }
        /// <summary>
        /// Invoked whenever a property in the view model changes.
        /// </summary>
        /// <param name="property">The property that changed</param>
        private void OnPropertyChanged(string property)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property));
        }
        /// <summary>
        /// Invoked whenever the SelectedDataEntries property changes
        /// </summary>
        /// 
        /// Invoked whenever the SelectedDataEntries property is changed.  Clears away the current SeriesSettings, as well as all the current data series.
        /// Recalculates velocity and acceleration graphs based on the current data.  Recalculates the filtered data for data
        /// series that contain time information. Adds all the newly created graphs to the available graph collections, defaulting all but the basic
        /// data to not visible. Triggers the secondary axis scale to be reset to match the current data, based on the selected scale range. Determines
        /// the distribution of timesteps recorded.
        private void OnSelectedDataEntriesChanged()
        {
            OnPropertyChanged("SelectedDataEntries");

            var timesteps = new Dictionary<int, long>();

            var series = new SeriesCollection<IChartSeries>();
            // Identify if the selected series has time information attached.
            var rawPointEntries = SelectedDataEntries as DataEntries<Point>;
            var timedPointEntries = SelectedDataEntries as DataEntries<TimedDataEntry>;
            // Clear the current series settings
            SeriesSettings.Clear();
            if (timedPointEntries != null)
            {
                // IIR filter coefficients, produced using MATLAB
                const double a21 = -0.41403357712344141;
                const double a22 = -0.34184464084699628;
                const double a31 = 0.6223866646485583;
                const double a32 = 0.30662550664896931;
                const double b11 = 0.64308220852957476;
                const double b12 = 0.61792290676387107;
                const double b21 = 0.53696045764116318;
                const double b22 = 0.61773371771257013;
                const double b31 = 0.64307849607020995;
                const double b32 = 0.41066147232228034;
                const double s1 = -0.36438081475160222;

                // Create the new data series
                var xSeries = new DataSeries<double, double>("X Coordinates");
                var ySeries = new DataSeries<double, double>("Y Coordinates");
                var xVelSeries = new DataSeries<double, double>("X Velocity");
                var fVelSeries = new DataSeries<double, double>("Filtered X Velocity");
                var yVelSeries = new DataSeries<double, double>("Y Velocity");
                var totalVelSeries = new DataSeries<double, double>("Total Velocity");
                var xAccelSeries = new DataSeries<double, double>("X Acceleration");
                var yAccelSeries = new DataSeries<double, double>("Y Acceleration");
                var totalAccelSeries = new DataSeries<double, double>("Total Acceleration");

                var filterMidValue = new List<double>();

                // Set up the variables to identify the data series range
                double lastY, lastXVel, lastYVel, lastTotalVel;
                maxVel = maxAccel = double.MinValue;
                minVel = minAccel = double.MaxValue;
                var lastTime = 0.0;
                var firstVel = true;
                var firstPlace = true;
                var lastX = lastY = lastXVel = lastYVel = lastTotalVel = 0;

                foreach (var p in timedPointEntries.Points.OrderBy(t => t.Time))
                {
                    var timeStep = double.NaN;
                    if (firstPlace)
                    {
                        // First entry. Cannot calculate the current velocity
                        lastX = p.Point.X;
                        lastY = p.Point.Y;
                        lastTime = p.Time;
                        firstPlace = false;
                    }
                    else if (firstVel)
                    {
                        // Second entry. Can calculate the current velocity, but not the acceleration

                        // Calculate the velocity
                        timeStep = (p.Time - lastTime);
                        var xVel = (p.Point.X - lastX) / timeStep;
                        var yVel = (p.Point.Y - lastY) / timeStep;
                        var totalVel = Math.Sqrt(xVel * xVel + yVel * yVel);

                        // Begin the IIR filter
                        var fVel = xVel * s1 * b11;
                        filterMidValue.Add(fVel);
                        fVel *= b12;

                        // Check the velocity range
                        if (maxVel < Math.Max(Math.Max(xVel, yVel), totalVel))
                            maxVel = Math.Max(Math.Max(xVel, yVel), totalVel);
                        if (minVel > Math.Min(Math.Min(xVel, yVel), totalVel))
                            minVel = Math.Min(Math.Min(xVel, yVel), totalVel);

                        xVelSeries.Add(new DataPoint<double, double>(p.Time, xVel));
                        yVelSeries.Add(new DataPoint<double, double>(p.Time, yVel));
                        fVelSeries.Add(new DataPoint<double, double>(p.Time, fVel));
                        totalVelSeries.Add(new DataPoint<double, double>(p.Time, totalVel));

                        lastXVel = xVel;
                        lastYVel = yVel;
                        lastTotalVel = totalVel;
                        lastX = p.Point.X;
                        lastY = p.Point.Y;
                        lastTime = p.Time;
                        firstVel = false;
                    }
                    else
                    {
                        // Every other entry. Can calculate both velocity and acceleration.

                        // Calculate the velocity
                        timeStep = (p.Time - lastTime);
                        var xVel = (p.Point.X - lastX) / timeStep;
                        var yVel = (p.Point.Y - lastY) / timeStep;
                        var totalVel = Math.Sqrt(xVel * xVel + yVel * yVel);

                        // Continue the IIR filter
                        var fVel = xVel * s1;
                        fVel -= a21 * xVelSeries[xVelSeries.Count - 1].Y;
                        if (xVelSeries.Count > 2)
                            fVel -= a31 * xVelSeries[xVelSeries.Count - 2].Y;
                        fVel *= b11;
                        fVel += b21 * xVelSeries[xVelSeries.Count - 1].Y;
                        if (xVelSeries.Count > 2)
                            fVel += b31 * xVelSeries[xVelSeries.Count - 2].Y;
                        var lastFilterMidVal = filterMidValue[filterMidValue.Count - 1];
                        var secondLastFilterMidVal = 0.0;
                        if (filterMidValue.Count > 2)
                            secondLastFilterMidVal = filterMidValue[filterMidValue.Count - 2];
                        fVel -= a22 * lastFilterMidVal;
                        fVel -= a32 * secondLastFilterMidVal;
                        filterMidValue.Add(fVel);
                        fVel *= b12;
                        fVel += b22 * lastFilterMidVal;
                        fVel += b32 * secondLastFilterMidVal;
                        fVel /= 1.4;
                        fVelSeries.Add(new DataPoint<double,double>(p.Time, fVel));

                        // Calculate the acceleration
                        var xAccel = (xVel - lastXVel) / timeStep;
                        var yAccel = (yVel - lastYVel) / timeStep;
                        var totalAccel = (totalVel - lastTotalVel) / timeStep;

                        // Check the velocity and acceleration ranges
                        if (maxVel < Math.Max(Math.Max(xVel, yVel), Math.Max(totalVel, fVel)))
                            maxVel = Math.Max(Math.Max(xVel, yVel), Math.Max(totalVel, fVel));
                        if (minVel > Math.Min(Math.Min(xVel, yVel), Math.Min(totalVel, fVel)))
                            minVel = Math.Min(Math.Min(xVel, yVel), Math.Min(totalVel, fVel));
                        if (maxAccel < Math.Max(Math.Max(xAccel, yAccel), totalAccel))
                            maxAccel = Math.Max(Math.Max(xAccel, yAccel), totalAccel);
                        if (minAccel > Math.Min(Math.Min(xAccel, yAccel), totalAccel))
                            minAccel = Math.Min(Math.Min(xAccel, yAccel), totalAccel);

                        xVelSeries.Add(new DataPoint<double, double>(p.Time, xVel));
                        yVelSeries.Add(new DataPoint<double, double>(p.Time, yVel));
                        totalVelSeries.Add(new DataPoint<double, double>(p.Time, totalVel));
                        xAccelSeries.Add(new DataPoint<double, double>(p.Time, xAccel));
                        yAccelSeries.Add(new DataPoint<double, double>(p.Time, yAccel));
                        totalAccelSeries.Add(new DataPoint<double, double>(p.Time, totalAccel));

                        lastXVel = xVel;
                        lastYVel = yVel;
                        lastTotalVel = totalVel;
                        lastTime = p.Time;
                        lastX = p.Point.X;
                        lastY = p.Point.Y;
                    }
                    xSeries.Add(new DataPoint<double, double>(p.Time, p.Point.X));
                    ySeries.Add(new DataPoint<double, double>(p.Time, p.Point.Y));

                    if (double.IsNaN(timeStep)) continue;
                    if (timeStep < 5) // If an apparently large time step, it's probably already in ms
                        timeStep *= 1000;
                    if (timeStep > 40000) // If a VERY large time step, it's probably in us
                        timeStep /= 1000;
                    var mSec = (int)(Math.Round(timeStep, 1)*10); // Round of the timestep to an integer

                    // Record the timestep distribution
                    if (timesteps.ContainsKey(mSec))
                        timesteps[mSec] += 1;
                    else
                        timesteps[mSec] = 1;
                }

                // Set up all the LineSeries for the calculated data sets
                var xLineSeries = new LineSeries { DataSeries = xSeries, XAxis = mainXAxis, YAxis = mainYAxis };
                var yLineSeries = new LineSeries { DataSeries = ySeries, XAxis = mainXAxis, YAxis = mainYAxis };
                // Set all the non-basic line series to hidden
                var xVelLineSeries = new LineSeries { DataSeries = xVelSeries, Visibility = Visibility.Hidden, XAxis = mainXAxis, YAxis = secondYAxis };
                var fVelLineSeries = new LineSeries { DataSeries = fVelSeries, Visibility = Visibility.Hidden, XAxis = mainXAxis, YAxis = secondYAxis };
                var yVelLineSeries = new LineSeries { DataSeries = yVelSeries, Visibility = Visibility.Hidden, XAxis = mainXAxis, YAxis = secondYAxis };
                var totalVelLineSeries = new LineSeries { DataSeries = totalVelSeries, Visibility = Visibility.Hidden, XAxis = mainXAxis, YAxis = secondYAxis };
                var xAccelLineSeries = new LineSeries { DataSeries = xAccelSeries, Visibility = Visibility.Hidden, XAxis = mainXAxis, YAxis = secondYAxis };
                var yAccelLineSeries = new LineSeries { DataSeries = yAccelSeries, Visibility = Visibility.Hidden, XAxis = mainXAxis, YAxis = secondYAxis };
                var totalAccelLineSeries = new LineSeries { DataSeries = totalAccelSeries, Visibility = Visibility.Hidden, XAxis = mainXAxis, YAxis = secondYAxis };

                series.Add(xLineSeries);
                series.Add(yLineSeries);
                series.Add(xVelLineSeries);
                series.Add(fVelLineSeries);
                series.Add(yVelLineSeries);
                series.Add(totalVelLineSeries);
                series.Add(xAccelLineSeries);
                series.Add(yAccelLineSeries);
                series.Add(totalAccelLineSeries);

                // Add the series to the SeriesSettings list to allow the visibility to be controlled with data binding
                SeriesSettings.Add(new SeriesViewModel(xLineSeries));
                SeriesSettings.Add(new SeriesViewModel(yLineSeries));
                SeriesSettings.Add(new SeriesViewModel(xVelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(fVelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(yVelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(totalVelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(xAccelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(yAccelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(totalAccelLineSeries));

                // Set up the secondary axis range
                SecondYAxis.Range = new DoubleRange(Math.Min(minVel, minAccel), Math.Max(maxVel, maxAccel));

                // Create the timestep distribution graph
                var timeDistribution = new DataSeries<double, long>("Time Steps");
                foreach (var t in timesteps.OrderBy(u => u.Key))
                {
                    timeDistribution.Add(new DataPoint<double, long>(((double)(t.Key)/10), t.Value));
                }
                TimeStepDistribution = new SeriesCollection<IChartSeries>
                                           {new ColumnSeries {DataSeries = timeDistribution}};

                OnPropertyChanged("TimeStepDistribution");
            }
            else if (rawPointEntries != null)
            {
                // Data entries with no time data. Can't do anything beyond plot the points
                var xSeries = new DataSeries<long, double>("X Coordinates");
                var ySeries = new DataSeries<long, double>("Y Coordinates");
                var i = 0;
                foreach (var p in rawPointEntries.Points)
                {
                    xSeries.Add(new DataPoint<long, double>(i, p.X));
                    ySeries.Add(new DataPoint<long, double>(i, p.Y));
                    i++;
                }

                var xLineSeries = new LineSeries { DataSeries = xSeries };
                var yLineSeries = new LineSeries { DataSeries = ySeries };

                series.Add(xLineSeries);
                series.Add(yLineSeries);
            }

            CurrentDataSeries = series;
            OnPropertyChanged("CurrentDataSeries");
            OnScaleRangeChanged();
        }

        /// <summary>
        /// Gets the data entries available to be viewed.
        /// </summary>
        public ObservableCollection<DataEntries> DataEntries { get; private set; }

        /// <summary>
        /// Gets or sets the currently selected data entry
        /// </summary>
        public DataEntries SelectedDataEntries
        {
            get
            {
                return selectedDataEntries;
            }
            set
            {
                selectedDataEntries = value;
                OnSelectedDataEntriesChanged();
            }
        }

        /// <summary>
        /// Gets the current set of data series
        /// </summary>
        public SeriesCollection<IChartSeries> CurrentDataSeries { get; private set; }
        /// <summary>
        /// Gets the timestep distribution data series
        /// </summary>
        public SeriesCollection<IChartSeries> TimeStepDistribution { get; private set; }
        /// <summary>
        /// Gets the settings for the data series
        /// </summary>
        public ObservableCollection<SeriesViewModel> SeriesSettings { get; private set; }
        /// <summary>
        /// Occurs when a property value changes.
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;
    }
}
