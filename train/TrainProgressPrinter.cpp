/*
 * TrainProgressPrinter.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: louis
 */

#include "TrainProgressPrinter.h"

#include "Logging.h"

namespace SLM {

TrainProgressPrinter::TrainProgressPrinter() {
	// TODO Auto-generated constructor stub

}

TrainProgressPrinter::~TrainProgressPrinter() {
	// TODO Auto-generated destructor stub
}

void TrainProgressPrinter::start()
{
	startTimePoint = std::chrono::system_clock::now();
}

void TrainProgressPrinter::next()
{
	++counter;

	print();
}

void TrainProgressPrinter::nextSample()
{
	reset(true, sample+1, 0);
	L_I << "\n";
}

void TrainProgressPrinter::reset(bool alsoTimer, int newSample , unsigned long long newValue)
{
	counter = newValue;
	sample = newSample;
	if(alsoTimer)
	{
		startTimePoint = std::chrono::system_clock::now();
	}
}

void TrainProgressPrinter::print()
{
	if(counter < 20000 || counter % 20000 == 0)
	{
		currentTimePoint = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedSeconds = currentTimePoint-startTimePoint;

		double avgPerSecond = counter*1.0/elapsedSeconds.count();

		L_I << std::fixed << "\r"
				  << "Sample [" << sample << "]"
				  << "\tPattern: " << std::setw(10) << counter
				  //<<  " (" << std::setw(4) << (counter*1.0/total*100) << "%) "
				  << "\t" << std::setw(8) << ((int) avgPerSecond) << "P/s"
				  //<< " seconds remaining:" << (total-counter)*1.0/avgPerSecond
				  << std::flush;
	}
}

} /* namespace SLM */
