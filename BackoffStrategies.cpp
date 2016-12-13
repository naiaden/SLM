/*
 * BackoffStrategies.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#include "BackoffStrategies.h"
#include "NgramBackoffStrategy.h"



namespace SLM {

enum BackoffLevel { NGRAM, LIMITED, FULL };
enum InterpolationStrategy { UNIFORM, MLE, ENTROPY };



BackoffStrategies::BackoffStrategies(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm)
{
	backoffStrategies = BackoffStrategiesFactory::fromProgramOptions(programOptions, lm);
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

std::vector<BackoffStrategy*> BackoffStrategiesFactory::fromProgramOptions(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm)
{
	std::vector<BackoffStrategy*> backoffStrategies;

	for(const std::string token: delimiterTokeniser(programOptions.getBackoffOptions(), '-'))
	{
		if(token == "ngram" || token == "all")
		{
			BackoffStrategy* bos = createNgramBackoffStrategy(programOptions, lm);
			if(bos) backoffStrategies.push_back(bos);
		}
	}

	return backoffStrategies;
}

BackoffStrategy* BackoffStrategiesFactory::createNgramBackoffStrategy(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm)
{
	// ngram cache option in program options?
	return new NgramBackoffStrategy(lm, programOptions.getTestModelName());
}

BackoffStrategy* BackoffStrategiesFactory::createLimitedBackoffStrategy(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm)
{
	// limited cache option in program options?
	return nullptr;
}

} /* namespace SLM */

