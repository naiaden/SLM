/*
 * RandomInterpolationStrategy.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: louis
 */

#include "RandomInterpolationStrategy.h"
#include <stdlib.h>

namespace SLM {

RandomInterpolationStrategy::RandomInterpolationStrategy() {
	// TODO Auto-generated constructor stub

}

RandomInterpolationStrategy::~RandomInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double RandomInterpolationStrategy::get(const Pattern& context)
{
	return ((double) rand() / (RAND_MAX));
}

std::string RandomInterpolationStrategy::name() const
{
	return "random";
}

} /* namespace SLM */
