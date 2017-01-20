/*
 * BackoffStrategies.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#include "BackoffStrategies.h"
#include "FullBackoffStrategy.h"
#include "LimitedBackoffStrategy.h"
#include "NgramBackoffStrategy.h"

#include "InterpolationStrategy.h"
#include "UniformInterpolationStrategy.h"



namespace SLM {

enum BackoffLevel { NGRAM, LIMITED, FULL };
enum InterpolationFactor { UNIFORM, MLE, ENTROPY };



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

std::string toString(InterpolationFactor is)
{
	switch (is) {
		case SLM::InterpolationFactor::UNIFORM:
			return "uni";
		case SLM::InterpolationFactor::MLE:
			return "mle";
		case SLM::InterpolationFactor::ENTROPY:
			return "ent";
	}
}

std::string toString(BackoffLevel bl)
{
	switch (bl) {
		case SLM::BackoffLevel::NGRAM:
			return "ngram";
		case SLM::BackoffLevel::LIMITED:
			return "lim";
		case SLM::BackoffLevel::FULL:
			return "full";
	}
}

InterpolationFactor stringToInterpolationStrategy(const std::string& is)
{
	if(is == "uni")	return SLM::InterpolationFactor::UNIFORM;
	if(is == "mle")	return SLM::InterpolationFactor::MLE;
	if(is == "ent")	return SLM::InterpolationFactor::ENTROPY;

	return SLM::InterpolationFactor::UNIFORM;
}

BackoffLevel stringToBackoffLevel(const std::string& bl)
{
	if(bl == "uni")	return SLM::BackoffLevel::NGRAM;
	if(bl == "mle")	return SLM::BackoffLevel::LIMITED;
	if(bl == "ent")	return SLM::BackoffLevel::FULL;

	return SLM::BackoffLevel::NGRAM;
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

		if(startsWith(token, "full"))
		{
			InterpolationStrategy* is = new UniformInterpolationStrategy();

			BackoffStrategy* bos = createFullBackoffStrategy(programOptions, lm, is);
			if(bos) backoffStrategies.push_back(bos);
		}

		if(startsWith(token, "lim"))
		{
			InterpolationStrategy* is = new UniformInterpolationStrategy();

			BackoffStrategy* bos = createLimitedBackoffStrategy(programOptions, lm, is);
			if(bos) backoffStrategies.push_back(bos);
		}

	}

	return backoffStrategies;
}

BackoffStrategy* BackoffStrategiesFactory::createNgramBackoffStrategy(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm)
{
	// ngram cache option in program options?
	NgramBackoffStrategy* nbs = new NgramBackoffStrategy(lm, programOptions.getTestModelName());
	nbs->init(lm, programOptions.getTestModelName());
	return nbs;
}

BackoffStrategy* BackoffStrategiesFactory::createLimitedBackoffStrategy(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm, SLM::InterpolationStrategy* interpolationStrategy)
{
	// limited cache option in program options?
	LimitedBackoffStrategy* lbs = new LimitedBackoffStrategy(lm, programOptions.getTestModelName(), interpolationStrategy);
	lbs->setIgnoreCache(programOptions.isIgnoreCache());
	lbs->init(lm, programOptions.getTestModelName());
	return lbs;
}

BackoffStrategy* BackoffStrategiesFactory::createFullBackoffStrategy(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm, SLM::InterpolationStrategy* interpolationStrategy)
{
	// full cache option in program options?
	FullBackoffStrategy* fbs = new FullBackoffStrategy(lm, programOptions.getTestModelName(), interpolationStrategy);
	fbs->setIgnoreCache(programOptions.isIgnoreCache());
	fbs->init(lm, programOptions.getTestModelName());
	return fbs;
}

} /* namespace SLM */

