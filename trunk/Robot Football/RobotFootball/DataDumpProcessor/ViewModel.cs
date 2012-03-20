using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using Visiblox.Charts;

namespace DataDumpProcessor
{
    class SeriesViewModel : INotifyPropertyChanged
    {
        private readonly ChartSeriesCommonBase series;

        public SeriesViewModel(ChartSeriesCommonBase series)
        {
            this.series = series;
        }
        public string Title
        {
            get
            {
                return series.DataSeries.Title;
            }
        }

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

        private void OnPropertyChanged(string property)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property));
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }

    public enum ScaleRange
    {
        Both = 0,
        Velocity,
        Acceleration
    }

    class ViewModel : INotifyPropertyChanged
    {
        public ViewModel()
        {
            DataFilePath = @"H:\DataDump.csv";
            DataEntries = new ObservableCollection<DataEntries>();
            SeriesSettings = new ObservableCollection<SeriesViewModel>();
        }

        private IAxis mainXAxis = new LinearAxis();
        private IAxis mainYAxis = new LinearAxis();
        private IAxis secondYAxis = new LinearAxis();

        public IAxis MainXAxis
        {
            get { return mainXAxis; }
            set
            {
                mainXAxis = value;
                OnPropertyChanged("MainXAxis");
            }
        }
        public IAxis MainYAxis
        {
            get { return mainYAxis; }
            set
            {
                mainYAxis = value;
                OnPropertyChanged("MainYAxis");
            }
        }
        public IAxis SecondYAxis
        {
            get { return secondYAxis; }
            set
            {
                secondYAxis = value;
                OnPropertyChanged("SecondYAxis");
            }
        }

        private double maxVel, minVel, maxAccel, minAccel;

        private string dataFilePath;
        private DataEntries selectedDataEntries;

        private ScaleRange scaleRange;

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

        public string DataFilePath
        {
            get { return dataFilePath; }
            set 
            {
                dataFilePath = value;
                OnPropertyChanged("DataFilePath");
            }
        }

        private void OnPropertyChanged(string property)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property));
        }

        private void OnSelectedDataEntriesChanged()
        {
            OnPropertyChanged("SelectedDataEntries");

            var timesteps = new Dictionary<int, long>();

            var series = new SeriesCollection<IChartSeries>();
            var rawPointEntries = SelectedDataEntries as RawPointDataEntries;
            var timedPointEntries = SelectedDataEntries as TimedDataEntries;
            SeriesSettings.Clear();
            if (timedPointEntries != null)
            {
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
                        lastX = p.Point.X;
                        lastY = p.Point.Y;
                        lastTime = p.Time;
                        firstPlace = false;
                    }
                    else if (firstVel)
                    {
                        timeStep = (p.Time - lastTime);
                        var xVel = (p.Point.X - lastX) / timeStep;
                        var yVel = (p.Point.Y - lastY) / timeStep;
                        var totalVel = Math.Sqrt(xVel * xVel + yVel * yVel);

                        var fVel = xVel * s1 * b11;
                        filterMidValue.Add(fVel);
                        fVel *= b12;

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
                        timeStep = (p.Time - lastTime);
                        var xVel = (p.Point.X - lastX) / timeStep;
                        var yVel = (p.Point.Y - lastY) / timeStep;
                        var totalVel = Math.Sqrt(xVel * xVel + yVel * yVel);

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

                        var xAccel = (xVel - lastXVel) / timeStep;
                        var yAccel = (yVel - lastYVel) / timeStep;
                        var totalAccel = (totalVel - lastTotalVel) / timeStep;

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
                    var mSec = (int)(Math.Round(timeStep, 1)*10);
                    if (timesteps.ContainsKey(mSec))
                        timesteps[mSec] += 1;
                    else
                        timesteps[mSec] = 1;
                }

                var xLineSeries = new LineSeries { DataSeries = xSeries, XAxis = mainXAxis, YAxis = mainYAxis };
                var yLineSeries = new LineSeries { DataSeries = ySeries, XAxis = mainXAxis, YAxis = mainYAxis };
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

                SeriesSettings.Add(new SeriesViewModel(xLineSeries));
                SeriesSettings.Add(new SeriesViewModel(yLineSeries));
                SeriesSettings.Add(new SeriesViewModel(xVelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(fVelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(yVelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(totalVelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(xAccelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(yAccelLineSeries));
                SeriesSettings.Add(new SeriesViewModel(totalAccelLineSeries));

                SecondYAxis.Range = new DoubleRange(Math.Min(minVel, minAccel), Math.Max(maxVel, maxAccel));

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

        public ObservableCollection<DataEntries> DataEntries { get; private set; }

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

        public SeriesCollection<IChartSeries> CurrentDataSeries { get; private set; }
        public SeriesCollection<IChartSeries> TimeStepDistribution { get; private set; }

        public ObservableCollection<SeriesViewModel> SeriesSettings { get; private set; }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
