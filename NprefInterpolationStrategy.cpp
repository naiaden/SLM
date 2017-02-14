/*
 * NprefInterpolationStrategy.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: louis
 */

#include "NprefInterpolationStrategy.h"

namespace SLM {

NprefInterpolationStrategy::NprefInterpolationStrategy() {
	// TODO Auto-generated constructor stub

}

NprefInterpolationStrategy::~NprefInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double NprefInterpolationStrategy::get(const Pattern& context)
{
	if(context.isskipgram())
		return 1.0;
	return 2.0;
}

std::string NprefInterpolationStrategy::name() const
{
	return "npref";
}

} /* namespace SLM */
