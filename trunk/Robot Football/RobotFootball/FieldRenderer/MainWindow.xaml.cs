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

namespace FieldRenderer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public sealed partial class MainWindow
    {
        private Classes.Environment currentEnvironment;
        private Classes.Environment latestEnvironment;
        private ComputeBuffer<byte> outPix;
        private ComputeBuffer<float> outCl;
        private ComputeBuffer<float> outGradient; 
        private ComputeContext context;
        private ComputeKernel colourKernel;
        private ComputeKernel gradientKernel;
        private ComputeKernel kernel;
        private NamedPipeServerStream pipeServer;
        private ReadOnlyCollection<ComputePlatform> platformList;
        private byte[] pixels;
        private const float FieldHeight = 70.8661f;
        private const float FieldWidth = 86.6141f;
        private const float GridResolution = 0.1f;
        private const int WorkGroupSize = 16;
        private float[] points;
        private float[] gradientPoints;
        private int gridHeight;
        private int gridWidth;
        private readonly BackgroundWorker worker = new BackgroundWorker();
        private readonly object latestEnvironmentLocker = new object();

        public MainWindow()
        {
            worker.WorkerSupportsCancellation = true;
            worker.DoWork += RenderImage;
            InitializeComponent();
        }

        private void RenderImage(object sender, DoWorkEventArgs doWorkEventArgs)
        {
            lock (latestEnvironmentLocker)
            {
                currentEnvironment = latestEnvironment;
            }

            var queue = new ComputeCommandQueue(context, context.Devices[0], ComputeCommandQueueFlags.None);

            ComputeField(queue, points);
            var bitmap = RenderPoints(queue, points, outCl);
            ComputeGradient(queue, gradientPoints);
            var bitmap2 = RenderPoints(queue, gradientPoints, outGradient);

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

        private Bitmap RenderPoints(ComputeCommandQueue queue, float[] pointsToRender, ComputeBuffer<float> computeBuffer)
        {
            var max = pointsToRender.AsParallel().Max();
            var min = pointsToRender.AsParallel().Min();

            colourKernel.SetValueArgument(0, max);
            colourKernel.SetValueArgument(1, min);
            colourKernel.SetMemoryArgument(2, computeBuffer);
            colourKernel.SetMemoryArgument(3, outPix);

            queue.Execute(colourKernel, new long[] {0}, new long[] {gridWidth, gridHeight},
                          new long[] {WorkGroupSize, WorkGroupSize}, null);
            queue.ReadFromBuffer(outPix, ref pixels, true, null);


            var bitmap = new Bitmap(gridWidth, gridHeight, PixelFormat.Format24bppRgb);

            var info = bitmap.LockBits(new Rectangle(0, 0, gridWidth, gridHeight), ImageLockMode.ReadWrite,
                                       PixelFormat.Format24bppRgb);


            Marshal.Copy(pixels, 0, info.Scan0, pixels.Length);

            bitmap.UnlockBits(info);

            bitmap.RotateFlip(RotateFlipType.RotateNoneFlipY);
            return bitmap;
        }

        private void ComputeGradient(ComputeCommandQueue queue, float[] gradPoints)
        {
            gradientKernel.SetMemoryArgument(0, outCl);
            gradientKernel.SetMemoryArgument(1, outGradient);

            queue.Execute(gradientKernel, new long[] { 0 }, new long[] { gridWidth, gridHeight },
                          new long[] { WorkGroupSize, WorkGroupSize }, null);

            queue.ReadFromBuffer(outGradient, ref gradPoints, true, null);
        }

        private void ComputeField(ComputeCommandQueue queue, float[] field)
        {
            var ball =
                new Vector2((float) currentEnvironment.CurrentBall.Position.X - currentEnvironment.FieldBounds.Left,
                            (float) currentEnvironment.CurrentBall.Position.Y - currentEnvironment.FieldBounds.Bottom
                    );

            var repulsers =
                currentEnvironment.Opponents.Select(o => o.Position).Concat(
                    currentEnvironment.Home.Select(h => h.Position).Where((v, i) => i != 1)).Select(
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

        // Code from http://stackoverflow.com/questions/3278827/how-to-convert-structure-to-byte-array-in-c
        private void EnvironmentReceived(IAsyncResult ar)
        {
            try
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
catch (System.Exception ex)
{
    throw;
}
        }

        private Vector3D currentVector;

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
