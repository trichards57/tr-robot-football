using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.ComponentModel;
using Visiblox.Charts;

namespace DataDumpProcessor
{
    class ViewModel : INotifyPropertyChanged
    {
        public ViewModel()
        {
            DataFilePath = @"H:\DataDump.csv";
            DataEntries = new ObservableCollection<DataEntries>();
        }

        private string dataFilePath;
        private DataEntries selectedDataEntries;

        public string DataFilePath
        {
            get { return dataFilePath; }
            set { dataFilePath = value; }
        }

        private void OnPropertyChanged(string property)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property));
        }

        private void OnSelectedDataEntriesChanged()
        {
            OnPropertyChanged("SelectedDataEntries");

            var series = new SeriesCollection<IChartSeries>();
            var rawPointEntries = SelectedDataEntries as RawPointDataEntries;
            if (rawPointEntries != null)
            {
                var xSeries = new DataSeries<long, double>("X Coordinates");
                var ySeries = new DataSeries<long, double>("Y Coordinates");
                int i = 0;
                foreach (var p in rawPointEntries.Points)
                {
                    xSeries.Add(new DataPoint<long, double>(i, p.X));
                    ySeries.Add(new DataPoint<long, double>(i, p.Y));
                    i++;
                }

                var xLineSeries = new LineSeries();
                var yLineSeries = new LineSeries();
                xLineSeries.DataSeries = xSeries;
                yLineSeries.DataSeries = ySeries;

                series.Add(xLineSeries);
                series.Add(yLineSeries);
            }

            CurrentDataSeries = series;
            OnPropertyChanged("CurrentDataSeries");
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

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
