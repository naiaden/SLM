/*
 * CountInterpolationStrategy.h
 *
 *  Created on: Feb 14, 2017
 *      Author: louis
 */

#ifndef COUNTINTERPOLATIONSTRATEGY_H_
#define COUNTINTERPOLATIONSTRATEGY_H_

#include "InterpolationStrategy.h"
#include "LanguageModel.h"

namespace SLM {

class CountInterpolationStrategy: public InterpolationStrategy {
public:
	CountInterpolationStrategy(SLM::LanguageModel& lm);
	virtual ~CountInterpolationStrategy();

	double get(const Pattern& context);
	std::string name() const;
protected:
	std::map<Pattern, double> weights;
	SLM::LanguageModel* lm;
};

} /* namespace SLM */

#endif /* COUNTINTERPOLATIONSTRATEGY_H_ */
