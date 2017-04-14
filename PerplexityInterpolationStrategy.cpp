/*
 * PerplexityInterpolationStrategy.cpp
 *
 *  Created on: Mar 31, 2017
 *      Author: louis
 */

#include "PerplexityInterpolationStrategy.h"
#include "LanguageModel.h"

#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace SLM {

PerplexityInterpolationStrategy::PerplexityInterpolationStrategy(SLM::LanguageModel& lm) : lm(&lm) {
	// TODO Auto-generated constructor stub

}

PerplexityInterpolationStrategy::~PerplexityInterpolationStrategy() {
	// TODO Auto-generated destructor stub
}

double PerplexityInterpolationStrategy::get(const Pattern& context)
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
					entropySum -= mle * log2(mle);
				}
			}
		}

		double perplexity = 1.0 / ( std::exp2(entropySum));

		L_S << "Perplexity: get(" << contextSize << ") sum:" << sum << " entropysum:" << entropySum << " perplexity:" << perplexity << "\n";

		weights[context] = perplexity;

		return perplexity;
	}
}


std::string PerplexityInterpolationStrategy::name() const
{
	return "perplexity";
}

} /* namespace SLM */
