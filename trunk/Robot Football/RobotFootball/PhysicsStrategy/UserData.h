#pragma once

/// @brief Class used to maintain state between simulator calls
class UserData
{
public:
	/// @brief The output stream used to store output data
	std::fstream outStream;

	/// @brief Initialize a new instance of the UserData class
	UserData()
		: outStream("D:\\Users\\Tony\\Documents\\DataDump.csv", std::fstream::out | std::fstream::app)
	{ }

	/// @brief Finalizes an instance of the UserData class.
	///
	/// Flushes and closes outStream.
	~UserData()
	{
		outStream.flush();
		outStream.close();
	}
};