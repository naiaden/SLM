/*
 * TrainProgressPrinter.h
 *
 *  Created on: Mar 29, 2017
 *      Author: louis
 */

#ifndef TRAIN_TRAINPROGRESSPRINTER_H_
#define TRAIN_TRAINPROGRESSPRINTER_H_

#include <ctime>
#include <chrono>

#include <iomanip> // put_time
#include <string>  // string

namespace SLM {

class TrainProgressPrinter {
public:
	TrainProgressPrinter();
	virtual ~TrainProgressPrinter();

	void print();

	void start();
	void next();
	void nextSample();
	void reset(bool alsoTimer = true, int newSample = 0, unsigned long long newValue = 0);
protected:
	unsigned long long counter = 0;
	int sample = 0;
	unsigned long long total = 0;

	std::chrono::time_point<std::chrono::system_clock> startTimePoint, currentTimePoint;
};

} /* namespace SLM */

#endif /* TRAIN_TRAINPROGRESSPRINTER_H_ */
