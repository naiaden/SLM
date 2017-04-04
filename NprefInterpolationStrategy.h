/*
 * NprefInterpolationStrategy.h
 *
 *  Created on: Feb 14, 2017
 *      Author: louis
 */

#ifndef NPREFINTERPOLATIONSTRATEGY_H_
#define NPREFINTERPOLATIONSTRATEGY_H_

#include "InterpolationStrategy.h"

namespace SLM {

class NprefInterpolationStrategy: public InterpolationStrategy {
public:
	NprefInterpolationStrategy(double npref = 2.0);
	virtual ~NprefInterpolationStrategy();

	double get(const Pattern& context);
	std::string name() const;
protected:
	double npref = 2.0;
};

} /* namespace SLM */

#endif /* NPREFINTERPOLATIONSTRATEGY_H_ */
