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

	auto it = weights.find(context);
	if(it != weights.end())
	{
		return it->second;
	}
	else
	{
		double entropySum = 0.0;

		std::vector<unsigned int> occurrenceCounts = lm->getCounts(context);
		unsigned int sum = std::accumulate ( occurrenceCounts.begin( ) , occurrenceCounts.end( ) , 0 ) ;
		if(sum > 0)
		{
			for (auto count: occurrenceCounts)
			{
				if(count > 0)
				{
					double mle = (1.0*count)/(1.0*sum);
					entropySum += mle * log2(mle);
				}
			}
		}

		double entropy = 1.0 / (1.0 - entropySum);

		L_S << "Entropyi: get(" << contextSize << ") sum:" << sum << " entropysum:" << entropySum << " entropy:" << entropy << "\n";

		weights[context] = entropy;

		return entropy;
	}
}


std::string EntropyInterpolationStrategy::name() const
{
	return "entropy";
}

} /* namespace SLM */
