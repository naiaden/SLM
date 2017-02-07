/*
 * InterpolationStrategy.h
 *
 *  Created on: Dec 14, 2016
 *      Author: onrust
 */

#ifndef INTERPOLATIONSTRATEGY_H_
#define INTERPOLATIONSTRATEGY_H_

#include <pattern.h>

namespace SLM {

class InterpolationStrategy {
public:
	InterpolationStrategy();
	virtual ~InterpolationStrategy();

	virtual double get(const Pattern& context) = 0;
	virtual std::string name() const = 0;
protected:
//	std::map<Pattern, double> weights;
};

} /* namespace SLM */

#endif /* INTERPOLATIONSTRATEGY_H_ */
