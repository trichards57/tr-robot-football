using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace DataDumpProcessor
{
    class DataEntries
    {
        public DateTime EntryDate { get; set; }
        public int Velocity { get; set; }
    }

    class RawPointDataEntries : DataEntries
    {
        public List<Point> Points { get; private set; }

        public RawPointDataEntries()
        {
            Points = new List<Point>();
        }
    }

    struct TimedDataEntry
    {
        public double Time { get; set; }
        public Point Point { get; set; }
    }

    class TimedDataEntries : DataEntries
    {
        public List<TimedDataEntry> Points { get; set; }

        public TimedDataEntries()
        {
            Points = new List<TimedDataEntry>();
        }
    }
}
