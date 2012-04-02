using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Media.Imaging;
using Cloo;
using FieldRenderer.Classes;
using OpenTK;

/// @brief All the field rendering classes
/// 
/// Intended to contain all the classes related to the field renderer
namespace FieldRenderer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public sealed partial class MainWindow
    {
        /// <summary>
        /// The current environment being rendered
        /// </summary>
        private Classes.Environment currentEnvironment;
        /// <summary>
        /// The last environment to be received.
        /// </summary>
        /// <remarks>
        /// Must lock <see cref="latestEnvironmentLocker"/> before use.
        /// </remarks>
        private Classes.Environment latestEnvironment;
        /// <summary>
        /// Buffer to store the pixels output by <see cref="colourKernel"/>
        /// </summary>
        private ComputeBuffer<byte> outPix;
        /// <summary>
        /// Buffer to store the field data produced by <see cref="kernel"/>
        /// </summary>
        private ComputeBuffer<float> outCl;
        /// <summary>
        /// Buffer to store the gradient data produced by <see cref="gradientKernel"/>
        /// </summary>
        private ComputeBuffer<float> outGradient;
        /// <summary>
        /// The ComputeContext used for rendering
        /// </summary>
        private ComputeContext context;
        /// <summary>
        /// The kernel used to produce an image based on field data
        /// </summary>
        private ComputeKernel colourKernel;
        /// <summary>
        /// The kernel used to produce calculate the field gradient magnitude from field data
        /// </summary>
        private ComputeKernel gradientKernel;
        /// <summary>
        /// The kernel used to calculate the field
        /// </summary>
        private ComputeKernel kernel;
        /// <summary>
        /// The named pipe server used to receive the incoming <see cref="Classes.Environment"/>
        /// </summary>
        private NamedPipeServerStream pipeServer;
        /// <summary>
        /// The list of ComputePlatforms available
        /// </summary>
        private ReadOnlyCollection<ComputePlatform> platformList;
        /// <summary>
        /// The pixels produced by <see cref="colourKernel"/>
        /// </summary>
        /// <seealso cref="outPix"/>
        private byte[] pixels;
        /// <summary>
        /// The height of the playing field
        /// </summary>
        private const float FieldHeight = 70.8661f;
        /// <summary>
        /// The width of the playing field
        /// </summary>
        private const float FieldWidth = 86.6141f;
        /// <summary>
        /// The resolution of the calculated field
        /// </summary>
        private const float GridResolution = 0.1f;
        /// <summary>
        /// The side size the local workgroup used to run the kernels
        /// </summary>
        private const int WorkGroupSize = 16;
        /// <summary>
        /// The field produced by <see cref="kernel"/>
        /// </summary>
        /// <seealso cref="outCl"/>
        private float[] points;
        /// <summary>
        /// The gradient points produced by <see cref="gradientKernel"/>
        /// </summary>
        private float[] gradientPoints;
        /// <summary>
        /// The height of the calculation grid
        /// </summary>
        private int gridHeight;
        /// <summary>
        /// The width of the calculation grid
        /// </summary>
        private int gridWidth;
        /// <summary>
        /// Used to render a received image
        /// </summary>
        private readonly BackgroundWorker worker = new BackgroundWorker();
        /// <summary>
        /// Object used to synchronise access to <see cref="latestEnvironment"/>
        /// </summary>
        private readonly object latestEnvironmentLocker = new object();

        /// <summary>
        /// Initializes a new instance of the <see cref="MainWindow"/> class.
        /// </summary>
        public MainWindow()
        {
            worker.WorkerSupportsCancellation = true;
            worker.DoWork += RenderImage;
            InitializeComponent();
        }

        /// <summary>
        /// Renders the image based on the <see cref="latestEnvironment"/>.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="doWorkEventArgs">The <see cref="System.ComponentModel.DoWorkEventArgs"/> instance containing the event data.</param>
        /// <remarks>
        /// Buffers <see cref="latestEnvironment"/> into <see cref="currentEnvironment"/> before rendering, to ensure new incoming
        /// data doesn't interfere, even if it comes in while still rendering.
        /// </remarks>
        private void RenderImage(object sender, DoWorkEventArgs doWorkEventArgs)
        {
            // Buffer the environment data
            lock (latestEnvironmentLocker)
            {
                currentEnvironment = latestEnvironment;
            }

            // Create the computation queue (this can't be saved and reused, not sure why)
            var queue = new ComputeCommandQueue(context, context.Devices[0], ComputeCommandQueueFlags.None);

            // Calculate the potential field
            ComputeField(queue, points);
            // Draw a picture of it
            var bitmap = RenderPoints(queue, points, outCl);
            // Calculate the field gradients
            ComputeGradient(queue, gradientPoints);
            // Draw a picture of them
            var bitmap2 = RenderPoints(queue, gradientPoints, outGradient);

            // Anonymous function which refreshes the images shown on the UI.  Has to be called on the UI thread to allow access
            // to the UI controls.
            Action refresh = () =>
            {
                using (var stream = new MemoryStream())
                {
                    bitmap.Save(stream, ImageFormat.Png);
                    stream.Seek(0, SeekOrigin.Begin);

                    var decoder = BitmapDecoder.Create(stream, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);

                    var writeable = new WriteableBitmap(decoder.Frames.Single());
                    writeable.Freeze();

                    FieldImage.Source = writeable;
                }
                using (var stream = new MemoryStream())
                {
                    bitmap2.Save(stream, ImageFormat.Png);
                    stream.Seek(0, SeekOrigin.Begin);

                    var decoder = BitmapDecoder.Create(stream, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);

                    var writeable = new WriteableBitmap(decoder.Frames.Single());
                    writeable.Freeze();

                    GradientImage.Source = writeable;
                }
            };

            Dispatcher.Invoke(refresh);
        }

        /// <summary>
        /// Renders the provided potential force field or gradient map.
        /// </summary>
        /// <param name="queue">The queue.</param>
        /// <param name="pointsToRender">The points to render.</param>
        /// <param name="computeBuffer">The compute buffer to use.</param>
        /// <returns>A bitmap representing <paramref name="pointsToRender"/></returns>
        private Bitmap RenderPoints(ComputeCommandQueue queue, float[] pointsToRender, ComputeBuffer<float> computeBuffer)
        {
            // Calculate the max and min values here, as it's easier than on the GPU
            // and there's no real time cost
            var max = pointsToRender.AsParallel().Max();
            var min = pointsToRender.AsParallel().Min();

            // Set up the colourKernel's arguments
            colourKernel.SetValueArgument(0, max);
            colourKernel.SetValueArgument(1, min);
            colourKernel.SetMemoryArgument(2, computeBuffer);
            colourKernel.SetMemoryArgument(3, outPix);

            // Run the colourKernl
            queue.Execute(colourKernel, new long[] {0}, new long[] {gridWidth, gridHeight},
                          new long[] {WorkGroupSize, WorkGroupSize}, null);
            // and read out the data produced
            queue.ReadFromBuffer(outPix, ref pixels, true, null);

            // Copy the data into a new Bitmap
            var bitmap = new Bitmap(gridWidth, gridHeight, PixelFormat.Format24bppRgb);

            var info = bitmap.LockBits(new Rectangle(0, 0, gridWidth, gridHeight), ImageLockMode.ReadWrite,
                                       PixelFormat.Format24bppRgb);


            Marshal.Copy(pixels, 0, info.Scan0, pixels.Length);

            bitmap.UnlockBits(info);
            // Flip the bitmap to make the coordinate axis match those in the simulator
            bitmap.RotateFlip(RotateFlipType.RotateNoneFlipY);
            return bitmap;
        }

        /// <summary>
        /// Computes the gradient of the field at all points.
        /// </summary>
        /// <param name="queue">The queue to execute the kernel on.</param>
        /// <param name="gradPoints">The calculated gradient points.</param>
        private void ComputeGradient(ComputeCommandQueue queue, float[] gradPoints)
        {
            gradientKernel.SetMemoryArgument(0, outCl);
            gradientKernel.SetMemoryArgument(1, outGradient);

            queue.Execute(gradientKernel, new long[] { 1,1 }, new long[] { gridWidth - 2, gridHeight - 2 },
                          null, null);

            queue.ReadFromBuffer(outGradient, ref gradPoints, true, null);
        }

        /// <summary>
        /// Computes the potential field at all points.
        /// </summary>
        /// <param name="queue">The queue to execute the kernel on.</param>
        /// <param name="field">The calculated field points.</param>
        /// 
        /// @todo Re-include the skipped robot once the Strategy does the same
        private void ComputeField(ComputeCommandQueue queue, float[] field)
        {
            var ball =
                new Vector2((float) currentEnvironment.CurrentBall.Position.X - currentEnvironment.FieldBounds.Left,
                            (float) currentEnvironment.CurrentBall.Position.Y - currentEnvironment.FieldBounds.Bottom
                    );

            // Collect together all the points that will repel the robot
            var repulsers =
                currentEnvironment.Opponents.Select(o => o.Position).Concat(
                    currentEnvironment.Home.Select(h => h.Position)).Select(
                        p =>
                        new Vector2((float) p.X - currentEnvironment.FieldBounds.Left,
                                    (float) p.Y - currentEnvironment.FieldBounds.Bottom)).ToArray();

            var inRepulsers = new ComputeBuffer<Vector2>(context,
                                                         ComputeMemoryFlags.ReadOnly |
                                                         ComputeMemoryFlags.CopyHostPointer, repulsers);
            kernel.SetValueArgument(0, ball);
            kernel.SetValueArgument(1, GridResolution);
            kernel.SetMemoryArgument(2, inRepulsers);
            kernel.SetMemoryArgument(3, outCl);

            queue.Execute(kernel, new long[] {0}, new long[] {gridWidth, gridHeight},
                          new long[] {WorkGroupSize, WorkGroupSize}, null);

            queue.ReadFromBuffer(outCl, ref field, true, null);
        }

        /// <summary>
        /// Called when the window has finished loading.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="System.Windows.RoutedEventArgs"/> instance containing the event data.</param>
        /// <remarks>
        /// Initialises the OpenCL parts that can be cached and re-used. Compiles the kernels and sets up the larger data storage
        /// properties.
        /// </remarks>
        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            platformList = ComputePlatform.Platforms;
            context = new ComputeContext(ComputeDeviceTypes.Gpu, new ComputeContextPropertyList(platformList[0]),
                                         null, IntPtr.Zero);

            var code = File.ReadAllText(@"D:\Users\Tony\Documents\tr-robot-football\Simulator\field_kernel.cl");
            var program = new ComputeProgram(context, code);
            try
            {
                program.Build(context.Devices, null, null, IntPtr.Zero);
            }
            catch(BuildProgramFailureComputeException)
            {
                var s = program.GetBuildLog(context.Devices[0]);
                MessageBox.Show(s);
                Close();
            }

            kernel = program.CreateKernel("main");
            colourKernel = program.CreateKernel("colorize");
            gradientKernel = program.CreateKernel("calculateGradient");

            gridWidth = (int)Math.Ceiling(FieldWidth / GridResolution);
            var remainder = gridWidth % WorkGroupSize;
            gridWidth += WorkGroupSize - remainder;

            gridHeight = (int)Math.Ceiling(FieldHeight / GridResolution);
            remainder = gridHeight % WorkGroupSize;
            gridHeight += WorkGroupSize - remainder;

            var length = gridWidth * gridHeight;

            points = new float[length];
            pixels = new byte[length*3];
            gradientPoints = new float[length];

            outCl = new ComputeBuffer<float>(context, ComputeMemoryFlags.ReadWrite, points.Length);
            outPix = new ComputeBuffer<byte>(context, ComputeMemoryFlags.WriteOnly, pixels.Length);
            outGradient = new ComputeBuffer<float>(context, ComputeMemoryFlags.ReadWrite, gradientPoints.Length);


            pipeServer = new NamedPipeServerStream("fieldRendererPipe", PipeDirection.InOut, 5,
                                                   PipeTransmissionMode.Message, PipeOptions.Asynchronous);
            pipeServer.BeginWaitForConnection(EnvironmentReceived, null);
        }

        /// <summary>
        /// Called whenever a new environment has been received.
        /// </summary>
        /// <param name="ar">The results of the aysnchronous operation.</param>
        /// <remarks>
        /// Byte Array to Structure code taken from @cite stackOverflowByte (http://stackoverflow.com/questions/3278827/how-to-convert-structure-to-byte-array-in-c)
        /// </remarks>
        private void EnvironmentReceived(IAsyncResult ar)
        {
            var environment = new StatusReport();
            var size = Marshal.SizeOf(environment);
	
            pipeServer.EndWaitForConnection(ar);
	
            var bytes = new List<byte>();
	
            var buffer = new byte[size];
	
            pipeServer.Read(buffer, 0, size);
	
            bytes.AddRange(buffer);
	
            var array = bytes.ToArray();
	
            var ptr = Marshal.AllocHGlobal(size);
	
            Marshal.Copy(array, 0, ptr, size);
	
            lock (latestEnvironmentLocker)
            {
                var report = (StatusReport) Marshal.PtrToStructure(ptr, typeof (StatusReport));
                latestEnvironment = report.Environment;
                CurrentVector = report.FieldVector;
            }
	
            if (!worker.IsBusy)
                worker.RunWorkerAsync();
	
            Marshal.FreeHGlobal(ptr);
            pipeServer.Disconnect();
            pipeServer.BeginWaitForConnection(EnvironmentReceived, null);
        }

        /// <summary>
        /// The field vector reported by the simulator
        /// </summary>
        private Vector3D currentVector;

        /// <summary>
        /// Gets or sets the current vector reported by the simulator.
        /// </summary>
        /// <value>
        /// The current vector.
        /// </value>
        internal Vector3D CurrentVector
        {
            get { return currentVector; }
            set
            {
                currentVector = value;
                Dispatcher.Invoke(new Action(() =>
                                             FieldVector.Text =
                                             string.Format("{0},{1}", currentVector.X, currentVector.Y)));
            }
        }
    }
}
