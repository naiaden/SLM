/*
 * FullBackoffStrategy.cpp
 *
 *  Created on: Dec 13, 2016
 *      Author: onrust
 */

#include "FullBackoffStrategy.h"

#include "Logging.h"
#include "InterpolationStrategy.h"

namespace SLM {

FullBackoffStrategy::FullBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName, SLM::InterpolationStrategy* interpolationStrategy)
 : BackoffStrategy(languageModel, baseFileName), interpolationStrategy(interpolationStrategy)
{
	init(languageModel, baseFileName);

}

FullBackoffStrategy::~FullBackoffStrategy() {
	// TODO Auto-generated destructor stub
}

std::string FullBackoffStrategy::name() const
{
	return "full";
}

double FullBackoffStrategy::prob(const Pattern& context, const Pattern& focus)
{
	L_S << "FullBackoffStrategy: Estimating prob for " << languageModel.toString(context)
			<< " " << languageModel.toString(focus) << "\n";

	// implement skipgrams from layer 3 on
	double prob = languageModel.getProbS4(focus, context, interpolationStrategy);
	double logProb = log2(prob);

	if(false /* oov */)
	{
		++sentOovs;
	}

	++sentCount;
	sentProb -= logProb;

	L_S << "FullBackoffStrategy: \t\t" << logProb << "\n";

	return logProb;
}

} /* namespace SLM */
