/*
 * MLEInterpolationStrategy.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: onrust
 */

#include "MLEInterpolationStrategy.h"

namespace SLM {

MLEInterpolationStrategy::MLEInterpolationStrategy(SLM::LanguageModel& lm) {
	// TODO Auto-generated constructor stub

}

MLEInterpolationStrategy::~MLEInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double MLEInterpolationStrategy::get(const Pattern& context) const
{
	std::map<Pattern, double>::const_iterator i = weights.find(context);
	if(i == weights.end())
	{
		for
	} else
	{
		return i->second;
	}
}

std::string MLEInterpolationStrategy::name() const
{
	return "mle";
}

} /* namespace SLM */
