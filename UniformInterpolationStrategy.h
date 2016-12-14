/*
 * UniformInterpolationStrategy.h
 *
 *  Created on: Dec 14, 2016
 *      Author: onrust
 */

#ifndef UNIFORMINTERPOLATIONSTRATEGY_H_
#define UNIFORMINTERPOLATIONSTRATEGY_H_

#include "InterpolationStrategy.h"

namespace SLM {

class UniformInterpolationStrategy: public InterpolationStrategy {
public:
	UniformInterpolationStrategy();
	virtual ~UniformInterpolationStrategy();

	double get(const Pattern& context) const;
	std::string name() const;
};

} /* namespace SLM */

#endif /* UNIFORMINTERPOLATIONSTRATEGY_H_ */
