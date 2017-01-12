/*
 * Timer.cpp
 *
 *  Created on: Dec 15, 2016
 *      Author: onrust
 */

#include "Timer.h"

#include <iostream>
#include <iomanip>

namespace SLM {

Timer::Timer()
{
	reset();
}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

std::chrono::duration<double> Timer::reset()
{
	startTime = std::chrono::system_clock::now();
	std::chrono::duration<double> oldInterval = interval;

	interval = std::chrono::duration<double>::zero();
	return oldInterval;
}
std::chrono::duration<double> Timer::getSplit()
{
	return std::chrono::system_clock::now()-startTime;
}
std::chrono::duration<double> Timer::stop()
{
	interval += std::chrono::system_clock::now()-startTime;
	return interval;
}
void Timer::start()
{
	startTime = std::chrono::system_clock::now();
}

ProgressTimer::ProgressTimer()
{

}

ProgressTimer::~ProgressTimer()
{

}

std::string ProgressTimer::toString()
{
//	 *mout << std::fixed << "\r"
//	              << "\tPattern: " << std::setw(10) << counter
//	              << " (" << std::setw(4) << (oov*1.0/counter*100) << "% OOV) "
//	              << std::setw(8) << ((int) avgPerSecond) << "P/s"
//	              << " took " << elapsedSeconds.count() << " seconds";
//	        *mout << std::endl;

	if(patterns < 10000 || patterns % 20000 == 0)
	{
		std::cout << "Pattern: " << std::setw(10) << patterns
				  << "\tF" << files << ":S" << lines
				  << "\tAvg: " << (patterns*1.0)/getSplit().count()
				  << "P/s\tTook: " << getSplit().count()
				  << " seconds"
//				  << std::flush;
				  << std::endl;
	}

	return "hoi";
}

void ProgressTimer::nextPattern()
{
	++patterns;
}

void ProgressTimer::nextLine()
{
	++lines;
}

void ProgressTimer::nextFile()
{
	++files;
}

} /* namespace SLM */
