using System;
using System.Collections.Generic;
using System.Windows;

namespace DataDumpProcessor
{
    /// <summary>
    /// A basic set of data entry
    /// </summary>
    class DataEntries
    {
        /// <summary>
        /// The date and time the entry was made
        /// </summary>
        public DateTime EntryDate { get; set; }
        /// <summary>
        /// The demand velocity of the system
        /// </summary>
        public int Velocity { get; set; }
    }
    /// <summary>
    /// A set of data points
    /// </summary>
    class RawPointDataEntries : DataEntries
    {
        /// <summary>
        /// The data points
        /// </summary>
        public List<Point> Points { get; private set; }
        /// <summary>
        /// Initializes a new instance of the RawPointDataEntries class
        /// </summary>
        public RawPointDataEntries()
        {
            Points = new List<Point>();
        }
    }

    /// <summary>
    /// A data entry that includes a recorded time
    /// </summary>
    struct TimedDataEntry
    {
        /// <summary>
        /// The time the point was recorded at
        /// </summary>
        public double Time { get; set; }
        /// <summary>
        /// The data point
        /// </summary>
        public Point Point { get; set; }
    }

    /// <summary>
    /// A set of data points that includes the time of recording
    /// </summary>
    class TimedDataEntries : DataEntries
    {
        public List<TimedDataEntry> Points { get; private set; }

        public TimedDataEntries()
        {
            Points = new List<TimedDataEntry>();
        }
    }
}
