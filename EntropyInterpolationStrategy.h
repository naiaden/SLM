/*
 * EntropyInterpolationStrategy.h
 *
 *  Created on: Feb 7, 2017
 *      Author: louis
 */

#ifndef ENTROPYINTERPOLATIONSTRATEGY_H_
#define ENTROPYINTERPOLATIONSTRATEGY_H_

#include "InterpolationStrategy.h"
#include "LanguageModel.h"

namespace SLM {

class EntropyInterpolationStrategy: public InterpolationStrategy {
public:
	EntropyInterpolationStrategy(SLM::LanguageModel& lm);
	virtual ~EntropyInterpolationStrategy();

	double get(const Pattern& context);
	std::string name() const;
protected:
	std::map<Pattern, double> weights;
	SLM::LanguageModel* lm;
};

} /* namespace SLM */

#endif /* ENTROPYINTERPOLATIONSTRATEGY_H_ */
