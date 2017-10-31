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

}

void NgramBackoffStrategy::init(SLM::LanguageModel& languageModel, const std::string& baseFileName)
{
	openFiles(languageModel, baseFileName);
}

NgramBackoffStrategy::~NgramBackoffStrategy() {
	// TODO Auto-generated destructor stub
}

std::string NgramBackoffStrategy::name() const
{
	return "ngram";
}

bool NgramBackoffStrategy::addToCache(const Pattern& pattern, double val)
{
    return false;
}

double NgramBackoffStrategy::prob(const Pattern& context, const Pattern& focus, bool isOOV)
{
	L_S << "NgramBackoffStrategy: Estimating prob for " << languageModel.toString(context)
			<< " " << languageModel.toString(focus) << "\n";

	double logProb = 0.0;

	if(!isOOV)
	{
		double prob = languageModel.getProb4(focus, context);
		logProb = log2(prob);

		++sentCount;
		sentLLH -= logProb;
	} else
	{
		++sentOovs;
	}

	L_S << "NgramBackoffStrategy: \t\t" << logProb << "\n";

	writeProbToFile(focus, context, logProb, isOOV);

	return logProb;
}

} /* namespace SLM */
