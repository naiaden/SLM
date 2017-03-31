/*
 * PerplexityInterpolationStrategy.h
 *
 *  Created on: Mar 31, 2017
 *      Author: louis
 */

#ifndef PERPLEXITYINTERPOLATIONSTRATEGY_H_
#define PERPLEXITYINTERPOLATIONSTRATEGY_H_

#include "InterpolationStrategy.h"
#include "LanguageModel.h"

#include <unordered_map>

namespace SLM {

class PerplexityInterpolationStrategy: public InterpolationStrategy {
public:
	PerplexityInterpolationStrategy(SLM::LanguageModel& lm);
	virtual ~PerplexityInterpolationStrategy();

	double get(const Pattern& context);
	std::string name() const;
protected:
	std::unordered_map<Pattern, double> weights;
	SLM::LanguageModel* lm;
};

} /* namespace SLM */

#endif /* PERPLEXITYINTERPOLATIONSTRATEGY_H_ */
