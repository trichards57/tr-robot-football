using System;
using System.Collections.Generic;
using System.Windows;

namespace DataDumpProcessor
{
    /// <summary>
    /// Base class to allow all DataEntries(of T) to be stored in a single collection, regardless of the type of T
    /// </summary>
    abstract class DataEntries
    {
    }

    /// <summary>
    /// A generic set of data entries
    /// </summary>
    /// <typeparam name="T">The data type of the entries</typeparam>
    class DataEntries<T> : DataEntries
    {
        /// <summary>
        /// The data points recorded
        /// </summary>
        public List<T> Points { get; private set; }

        /// <summary>
        /// The date and time the entry was made
        /// </summary>
        public DateTime EntryDate { get; set; }
        /// <summary>
        /// The demand velocity of the system
        /// </summary>
        public int Velocity { get; set; }

        /// <summary>
        /// Initializes a new instance of the DataEntries class
        /// </summary>
        public DataEntries()
        {
            Points = new List<T>();
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
}
