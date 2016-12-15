/*
 * NgramBackoffStrategy.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#include "NgramBackoffStrategy.h"

#include "Logging.h"

namespace SLM {

NgramBackoffStrategy::NgramBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName)
 : BackoffStrategy(languageModel, baseFileName)
{
	init(languageModel, baseFileName);

}

NgramBackoffStrategy::~NgramBackoffStrategy() {
	// TODO Auto-generated destructor stub
}

std::string NgramBackoffStrategy::name() const
{
	return "ngram";
}

double NgramBackoffStrategy::prob(const Pattern& context, const Pattern& focus)
{
	L_S << "NgramBackoffStrategy: Estimating prob for " << languageModel.toString(context)
			<< " " << languageModel.toString(focus) << "\n";

	double prob = languageModel.getProb(focus, context);
	double logProb = log2(prob);

	if(false /* oov */)
	{
		++sentOovs;
	}

	++sentCount;
	sentLLH -= logProb;

	L_S << "NgramBackoffStrategy: \t\t" << logProb << "\n";

	return logProb;
}

} /* namespace SLM */
