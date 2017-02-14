/*
 * EntropyInterpolationStrategy.cpp
 *
 *  Created on: Feb 7, 2017
 *      Author: louis
 */

#include "EntropyInterpolationStrategy.h"
#include "LanguageModel.h"

#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace SLM {

EntropyInterpolationStrategy::EntropyInterpolationStrategy(SLM::LanguageModel& lm) : lm(&lm) {
	// TODO Auto-generated constructor stub

}

EntropyInterpolationStrategy::~EntropyInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double EntropyInterpolationStrategy::get(const Pattern& context)
{
	int contextSize = context.size();

	std::map<Pattern, double>::const_iterator i = weights.find(context);
	if(i == weights.end())
	{
		double entropySum = 0.0;

		std::vector<unsigned int> occurrenceCounts = lm->getCounts(context);
		unsigned int sum = std::accumulate ( occurrenceCounts.begin( ) , occurrenceCounts.end( ) , 0 ) ;
		for (auto count: occurrenceCounts)
		{
			if(count > 0)
			{
				double mle = (1.0*count)/(1.0*sum);
				if(!std::isnormal(mle))
				{
					mle = 0.0000001;
				}
				mle = std::max(0.0000001, mle);

				entropySum += mle * log(mle);
			}
		}

		double entropy = 1.0 + std::abs(1.0 / (-entropySum - 0.75));

		L_S << "Entropyi: get(" << contextSize << ") sum:" << sum << " entropysum:" << entropySum << " entropy:" << entropy << "\n";

//		weights[context] = entropy;
		weights.emplace(std::make_pair(context, entropy));

		return entropy;

		// add to map
	} else
	{
		return i->second;
	}
}


std::string EntropyInterpolationStrategy::name() const
{
	return "entropy";
}

} /* namespace SLM */
