/*
 * LimitedBackoffStrategy.h
 *
 *  Created on: Dec 16, 2016
 *      Author: onrust
 */

#ifndef LIMITEDBACKOFFSTRATEGY_H_
#define LIMITEDBACKOFFSTRATEGY_H_

#include "FullBackoffStrategy.h"

namespace SLM {

class InterpolationStrategy;

class LimitedBackoffStrategy: public FullBackoffStrategy
{
public:
	LimitedBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName, SLM::InterpolationStrategy* interpolationStrategy);
	virtual ~LimitedBackoffStrategy();

	std::string name() const;
	double prob(const Pattern& context, const Pattern& focus);

	double getNormalisationFactor(const Pattern& context);
private:

};

} /* namespace SLM */

#endif /* LIMITEDBACKOFFSTRATEGY_H_ */
