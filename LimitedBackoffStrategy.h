/*
 * LimitedBackoffStrategy.h
 *
 *  Created on: Dec 16, 2016
 *      Author: onrust
 */

#ifndef LIMITEDBACKOFFSTRATEGY_H_
#define LIMITEDBACKOFFSTRATEGY_H_

#include "BackoffStrategy.h"

namespace SLM {

class InterpolationStrategy;

class LimitedBackoffStrategy: public BackoffStrategy
{
public:
	LimitedBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName, SLM::InterpolationStrategy* interpolationStrategy);
	virtual ~LimitedBackoffStrategy();

	std::string name() const;
	double prob(const Pattern& context, const Pattern& focus);
private:
	SLM::InterpolationStrategy* interpolationStrategy;

	std::map<Pattern, double> cache;
};

} /* namespace SLM */

#endif /* LIMITEDBACKOFFSTRATEGY_H_ */
