/*
 * UniformInterpolationStrategy.cpp
 *
 *  Created on: Dec 14, 2016
 *      Author: onrust
 */

#include "UniformInterpolationStrategy.h"

namespace SLM {

UniformInterpolationStrategy::UniformInterpolationStrategy() {
	// TODO Auto-generated constructor stub

}

UniformInterpolationStrategy::~UniformInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double UniformInterpolationStrategy::get(const Pattern& context)
{
	return 1.0;
}

std::string UniformInterpolationStrategy::name() const
{
	return "uniform";
}

} /* namespace SLM */
