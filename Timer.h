/*
 * Timer.h
 *
 *  Created on: Dec 15, 2016
 *      Author: onrust
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>
#include <string>

namespace SLM {

class Timer
{
public:
	Timer();
	virtual ~Timer();

	std::chrono::duration<double> reset();
	std::chrono::duration<double> getSplit();
	std::chrono::duration<double> stop();
	void start();
protected:
	std::chrono::time_point<std::chrono::system_clock> startTime;
	 std::chrono::duration<double> interval;
};

class ProgressTimer: public Timer
{
public:
	ProgressTimer();
	virtual ~ProgressTimer();

	std::string toString();

	void nextPattern();
	void nextLine();
	void nextFile();

        void disablePrinting();
private:
	unsigned long long patterns = 0;
	unsigned long long lines = 0;
	unsigned int files = 0;

        bool doPrint = true;
};

} /* namespace SLM */

#endif /* TIMER_H_ */
