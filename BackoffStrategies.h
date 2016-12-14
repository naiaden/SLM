/*
 * BackoffStrategies.h
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#ifndef BACKOFFSTRATEGIES_H_
#define BACKOFFSTRATEGIES_H_

#include "BackoffStrategy.h"

#include "LanguageModel.h"
#include "ProgramOptions.h"
#include "Utils.h"

namespace SLM {

class InterpolationStrategy;

class BackoffStrategies
{
public:
	BackoffStrategies(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm);
	virtual ~BackoffStrategies();

	void prob(const Pattern& context, const Pattern& focus);
	void addBackoffStrategy(BackoffStrategy* strategy);

	void nextFile();
	void nextLine();
	void done();

private:
	std::vector<BackoffStrategy*> backoffStrategies;
};

class BackoffStrategiesFactory
{
public:
	static std::vector<BackoffStrategy*> fromProgramOptions(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm);
	static BackoffStrategy* createNgramBackoffStrategy(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm);
	static BackoffStrategy* createLimitedBackoffStrategy(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm);
	static BackoffStrategy* createFullBackoffStrategy(const SLM::ProgramOptions& programOptions, SLM::LanguageModel& lm, SLM::InterpolationStrategy* interpolationStrategy);
};

} /* namespace SLM */

#endif /* BACKOFFSTRATEGIES_H_ */
