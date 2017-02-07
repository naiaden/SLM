/*
 * MLEInterpolationStrategy.h
 *
 *  Created on: Dec 19, 2016
 *      Author: onrust
 */

#ifndef MLEINTERPOLATIONSTRATEGY_H_
#define MLEINTERPOLATIONSTRATEGY_H_

#include "InterpolationStrategy.h"
#include "LanguageModel.h"

namespace SLM {

class MLEInterpolationStrategy: public InterpolationStrategy {
public:
	MLEInterpolationStrategy(SLM::LanguageModel& lm);
	virtual ~MLEInterpolationStrategy();

	double get(const Pattern& context);
	std::string name() const;
protected:
	std::map<Pattern, double> weights;
	SLM::LanguageModel* lm;
};

} /* namespace SLM */

#endif /* MLEINTERPOLATIONSTRATEGY_H_ */
