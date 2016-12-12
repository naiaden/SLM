/*
 * BackoffStrategies.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#include "BackoffStrategies.h"
#include "NgramBackoffStrategy.h"



namespace SLM {

enum BackoffLevel { LIMITED, FULL };
enum InterpolationStrategy { UNIFORM, MLE, ENTROPY };



BackoffStrategies::BackoffStrategies(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm)
{
	// TODO Auto-generated constructor stub
	backoffStrategies.push_back(new NgramBackoffStrategy(lm, programOptions.getTestModelName()));

}

BackoffStrategies::~BackoffStrategies()
{
	// TODO Auto-generated destructor stub
}


void BackoffStrategies::prob(const Pattern& context, const Pattern& focus)
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->prob(context, focus);
	}
}

void BackoffStrategies::addBackoffStrategy(BackoffStrategy* strategy)
{
	backoffStrategies.push_back(strategy);
}

void BackoffStrategies::nextFile()
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->nextFile();
	}
}

void BackoffStrategies::nextLine()
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->nextLine();
	}
}

void BackoffStrategies::done()
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->done();
	}
}

} /* namespace SLM */

