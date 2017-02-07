/*
 * MLEInterpolationStrategy.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: onrust
 */

#include "MLEInterpolationStrategy.h"
#include "LanguageModel.h"

#include <functional>
#include <algorithm>
#include <numeric>

namespace SLM {

MLEInterpolationStrategy::MLEInterpolationStrategy(SLM::LanguageModel& lm) : lm(&lm) {
	// TODO Auto-generated constructor stub

}

MLEInterpolationStrategy::~MLEInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double MLEInterpolationStrategy::get(const Pattern& context) const
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

	std::map<Pattern, double>::const_iterator i = weights.find(context);
	if(i == weights.end())
	{
		std::vector<unsigned int> occurrenceCounts = lm->getCounts(contextContext);
		unsigned int sum = std::accumulate ( occurrenceCounts.begin( ) , occurrenceCounts.end( ) , 0 ) ;
		double mle = (1.0*lm->getCount(contextFocus, contextContext))/(1.0*sum);

		L_S << "MLEi: get(" << contextSize << ") sum:" << sum << " count:" << lm->getCount(contextFocus, contextContext) << "\n";

		return std::max(0.0000001, mle);

		// add to map
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
