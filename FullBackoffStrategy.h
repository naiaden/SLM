/*
 * FullBackoffStrategy.h
 *
 *  Created on: Dec 13, 2016
 *      Author: onrust
 */

#ifndef FULLBACKOFFSTRATEGY_H_
#define FULLBACKOFFSTRATEGY_H_

#include "BackoffStrategy.h"

namespace SLM {

class FullBackoffStrategy: public BackoffStrategy {
public:
	FullBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName);
	virtual ~FullBackoffStrategy();

	std::string name() const;
	double prob(const Pattern& context, const Pattern& focus);
};

} /* namespace SLM */

#endif /* FULLBACKOFFSTRATEGY_H_ */
