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

namespace SLM {

class BackoffStrategies {
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

} /* namespace SLM */

#endif /* BACKOFFSTRATEGIES_H_ */
