Getting Started
===============

For an overview of Robot Soccer the place to start is the FIRA web site:- www.FIRA.net

This lists all the rules and regulations for playing robot soccer.

You can download the simulator from the site and install it on your computer.

To create and build strategies you need a MS Visual Studio V8.0 Application.

See TSG for this.

Directories and Files
=====================

Having downloaded the simulator you MUST copy the following file to the directory where the Robot Soccer .exe file is located, otherwise your strategy will not run.

		.\Lib\Release\Geometry.dll 	13K	18-DEC-2006 13:50

The following directories MUST exist.

		C:\strategy\blue
		C:\strategy\yellow

They are usually created when the simulator is installed. This is where you put your strategies to be executed.

Create diretory C:\strategy\misc and copy the contents of blue and yellow plus Geometry.dll into it, in case of file corruption.

Setting the Strategy
====================

Copy the following file:-

	.\Strategy\Release\InitStrategy.dll 
to 
	C:\strategy\blue

Running the Simulator
=====================

Run the simulator by double clicking on the Robot Soccer icon.

When loaded you will see the home screen. At the bottom there is the Help option.
Click on this and read the manual. This describes how to load and play the game.


Loading the Strategy
====================

Click on the STRATEGIES option.
For Blue team:-
Click on Lingo, switches to C++  (Must always be C++ for our strategies)
Click on Team1 and delete it
Enter the name of the strategy dll file without the .dll extension - case sensitive
e.g. InitStrategy

Repeat for Yellow team

If you want this strategy to play the machine (Yellow) then leave it alone, 
it must be Lingo and Team2

Click on Send option.

Click on Start option








