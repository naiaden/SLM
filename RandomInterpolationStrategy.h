/*
 * RandomInterpolationStrategy.h
 *
 *  Created on: Feb 14, 2017
 *      Author: louis
 */

#ifndef RANDOMINTERPOLATIONSTRATEGY_H_
#define RANDOMINTERPOLATIONSTRATEGY_H_

#include "InterpolationStrategy.h"

namespace SLM {

class RandomInterpolationStrategy: public InterpolationStrategy {
public:
	RandomInterpolationStrategy();
	virtual ~RandomInterpolationStrategy();

	double get(const Pattern& context);
	std::string name() const;
};

} /* namespace SLM */

#endif /* RANDOMINTERPOLATIONSTRATEGY_H_ */
