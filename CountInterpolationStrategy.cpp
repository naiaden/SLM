/*
 * CountInterpolationStrategy.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: louis
 */

#include "CountInterpolationStrategy.h"

namespace SLM {

CountInterpolationStrategy::CountInterpolationStrategy(SLM::LanguageModel& lm) : lm(&lm) {
	// TODO Auto-generated constructor stub

}

CountInterpolationStrategy::~CountInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double CountInterpolationStrategy::get(const Pattern& context)
{
	Pattern contextContext;
	Pattern contextFocus;

	int contextSize = context.size();

	if(contextSize == 1)
	{
		contextFocus = context;
	} else if(contextSize == 2)
	{
		contextContext = Pattern(context, 0, 1);
		contextFocus = Pattern(context, 1, 1);
	} else if(contextSize == 3)
	{
		contextContext = Pattern(context, 0, 2);
		contextFocus = Pattern(context, 2, 1);
	} else if(contextSize == 4)
	{
		contextContext = Pattern(context, 0, 3);
		contextFocus = Pattern(context, 3, 1);
	}

//	std::map<Pattern, double>::const_iterator i = weights.find(context);
//	if(i == weights.end())
//	{
//		weights[context] = mle;
		return 1.0 + lm->getCount(contextFocus, contextContext);
//	} else
//	{
//		return i->second;
//	}
}

std::string CountInterpolationStrategy::name() const
{
	return "count";
}

} /* namespace SLM */
