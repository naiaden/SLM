/*
 * NprefInterpolationStrategy.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: louis
 */

#include "NprefInterpolationStrategy.h"

namespace SLM {

NprefInterpolationStrategy::NprefInterpolationStrategy(double npref) : npref(npref) {
	// TODO Auto-generated constructor stub

}

NprefInterpolationStrategy::~NprefInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double NprefInterpolationStrategy::get(const Pattern& context)
{
	if(context.isskipgram())
		return 1.0;
	return npref;
}

std::string NprefInterpolationStrategy::name() const
{
	return "npref" + std::to_string(npref);
}

} /* namespace SLM */
