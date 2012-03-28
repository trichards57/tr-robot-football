using System.Runtime.InteropServices;

namespace FieldRenderer.Classes
{
    /// <summary>
    /// The environment provided by the simulator for rendering purposes.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    struct Environment
    {
        /// <summary>
        /// The current team's robots
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
        public Robot[] Home;
        /// <summary>
        /// The opposing team's robots
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
        public OpponentRobot[] Opponents;

        /// <summary>
        /// The current position of the ball
        /// </summary>
        public Ball CurrentBall;
        /// <summary>
        /// The previous position of the ball
        /// </summary>
        public Ball LastBall;
        /// <summary>
        /// The predicted position of the ball (not used)
        /// </summary>
        public Ball PredictedBall;

        /// <summary>
        /// The bounds of the field in use
        /// </summary>
        public Bounds FieldBounds;
        /// <summary>
        /// The bounds of the goal in use
        /// </summary>
        public Bounds GoalBounds;

        /// <summary>
        /// The state of the game
        /// </summary>
        public int GameState;
        /// <summary>
        /// Represents who owns the ball
        /// </summary>
        public int WhosBall;

        /// <summary>
        /// The user data object
        /// </summary>
        /// <remarks>This is a 32-bit pointer</remarks>
        public int UserData;
    }
}
