using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows;
using System.IO.Pipes;
using System.Windows.Media.Imaging;
using Cloo;
using OpenTK;

namespace FieldRenderer
{
    [StructLayout(LayoutKind.Sequential)]
    struct float2
    {
        public float X;
        public float Y;

        public float2(float x, float y)
        {
            X = x;
            Y = y;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    struct float4
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public float4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public sealed partial class MainWindow
    {
        private const float FieldHeight = 70.8661f;
        private const float FieldWidth = 86.6141f;
        private const float GridResolution = 0.1f;
        private const int WorkGroupSize = 16;
        private Classes.Environment latestEnvironment;
        private Classes.Environment currentEnvironment;


        private readonly BackgroundWorker worker = new BackgroundWorker();

        private readonly object latestEnvironmentLocker = new object();

        private NamedPipeServerStream pipeServer;
        private ComputeContext context;
        private ComputeKernel kernel;
        private ComputeKernel colourKernel;
        private float[] points;
        private byte[] pixels;
        private ComputeBuffer<float> outCl;
        private int gridWidth;
        private int gridHeight;
        private ReadOnlyCollection<ComputePlatform> platformList;
        private ComputeBuffer<byte> outPix;
        private ComputeKernel pointsKernel;

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

            var ball =
                new float2((float) currentEnvironment.CurrentBall.Position.X - currentEnvironment.FieldBounds.Left,
                            (float) currentEnvironment.CurrentBall.Position.Y - currentEnvironment.FieldBounds.Bottom
                    );

            var repulsers =
                currentEnvironment.Opponents.Select(o => o.Position).Concat(
                    currentEnvironment.Home.Select(h => h.Position)).Select(p => new Vector2((float)p.X - currentEnvironment.FieldBounds.Left, (float)p.Y - currentEnvironment.FieldBounds.Bottom)).ToArray();

            var inRepulsers = new ComputeBuffer<Vector2>(context,
                                                         ComputeMemoryFlags.ReadOnly |
                                                         ComputeMemoryFlags.CopyHostPointer, repulsers);
            kernel.SetValueArgument(0, ball);
            kernel.SetValueArgument(1, GridResolution);
            kernel.SetMemoryArgument(2, inRepulsers);
            kernel.SetMemoryArgument(3, outCl);

            var queue = new ComputeCommandQueue(context, context.Devices[0], ComputeCommandQueueFlags.None);

            queue.Execute(kernel, new long[] {0}, new long[] {gridWidth, gridHeight},
                          new long[] {WorkGroupSize, WorkGroupSize}, null);

            queue.Finish();

            queue.ReadFromBuffer(outCl, ref points, true, null);

            var inPoints = new[] {new Vector2(1, 1), new Vector2(2, 2), new Vector2(3, 3), new Vector2(4, 4)};
            var inPointsBuffer = new ComputeBuffer<Vector2>(context,
                                                    ComputeMemoryFlags.ReadOnly | ComputeMemoryFlags.CopyHostPointer,
                                                    inPoints);

            var outPoints = new float[4];
            var outPointsBuffer = new ComputeBuffer<float>(context,
                                                           ComputeMemoryFlags.WriteOnly |
                                                           ComputeMemoryFlags.CopyHostPointer, outPoints);


            pointsKernel.SetValueArgument(0, ball);
            pointsKernel.SetMemoryArgument(1, inPointsBuffer);
            pointsKernel.SetMemoryArgument(2, inRepulsers);
            pointsKernel.SetMemoryArgument(3, outPointsBuffer);

            queue.Execute(pointsKernel, new long[] { 0 }, new long[] { 4 },
                          new long[] { 1 }, null);
            queue.ReadFromBuffer(outPointsBuffer, ref outPoints, true, null);

            var p1 = points[10 + 10 * gridWidth];
            var p2 = points[20 + 20 * gridWidth];
            var p3 = points[30 + 30 * gridWidth];
            var p4 = points[40 + 40 * gridWidth];

            var max = new[] {points.AsParallel().Max()};
            var min = new[] {points.AsParallel().Min()};

            var inMax = new ComputeBuffer<float>(context,
                                                   ComputeMemoryFlags.ReadOnly | ComputeMemoryFlags.CopyHostPointer, max);
            var inMin = new ComputeBuffer<float>(context,
                                                    ComputeMemoryFlags.ReadOnly | ComputeMemoryFlags.CopyHostPointer, min);


            colourKernel.SetMemoryArgument(0, inMax);
            colourKernel.SetMemoryArgument(1, inMin);
            colourKernel.SetMemoryArgument(2, outCl);
            colourKernel.SetMemoryArgument(3, outPix); 
                
            queue.Execute(colourKernel, new long[] { 0 }, new long[] { gridWidth, gridHeight },
                          new long[] { WorkGroupSize, WorkGroupSize }, null);
            queue.ReadFromBuffer(outPix, ref pixels, true, null);


            var bitmap = new Bitmap(gridWidth, gridHeight, PixelFormat.Format24bppRgb);

            var info = bitmap.LockBits(new Rectangle(0, 0, gridWidth, gridHeight), ImageLockMode.ReadWrite,
                                       PixelFormat.Format24bppRgb);



            Marshal.Copy(pixels, 0, info.Scan0, pixels.Length);

            bitmap.UnlockBits(info);

            bitmap.RotateFlip(RotateFlipType.RotateNoneFlipY);

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
            };

            Dispatcher.Invoke(refresh);
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
            pointsKernel = program.CreateKernel("fieldAtPoints");

            gridWidth = (int)Math.Ceiling(FieldWidth / GridResolution);
            var remainder = gridWidth % WorkGroupSize;
            gridWidth += WorkGroupSize - remainder;

            gridHeight = (int)Math.Ceiling(FieldHeight / GridResolution);
            remainder = gridHeight % WorkGroupSize;
            gridHeight += WorkGroupSize - remainder;

            var length = gridWidth * gridHeight;

            points = new float[length];
            pixels = new byte[length*3];

            outCl = new ComputeBuffer<float>(context,
                                               ComputeMemoryFlags.ReadWrite, points.Length);
            outPix = new ComputeBuffer<byte>(context, ComputeMemoryFlags.WriteOnly, pixels.Length);



            pipeServer = new NamedPipeServerStream("fieldRendererPipe", PipeDirection.InOut, 5,
                                                   PipeTransmissionMode.Message, PipeOptions.Asynchronous);
            pipeServer.BeginWaitForConnection(EnvironmentReceived, null);
        }

        // Code from http://stackoverflow.com/questions/3278827/how-to-convert-structure-to-byte-array-in-c
        private void EnvironmentReceived(IAsyncResult ar)
        {
            var environment = new Classes.Environment();
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
                latestEnvironment = (Classes.Environment)Marshal.PtrToStructure(ptr, environment.GetType());
            }

            if (!worker.IsBusy)
                worker.RunWorkerAsync();

            Marshal.FreeHGlobal(ptr);
            pipeServer.Disconnect();
            pipeServer.BeginWaitForConnection(EnvironmentReceived, null);
        }
    }
}
