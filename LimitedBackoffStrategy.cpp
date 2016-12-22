/*
 * LimitedBackoffStrategy.cpp
 *
 *  Created on: Dec 16, 2016
 *      Author: onrust
 */

#include "LimitedBackoffStrategy.h"

#include "Logging.h"
#include "InterpolationStrategy.h"

namespace SLM {

LimitedBackoffStrategy::LimitedBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName, SLM::InterpolationStrategy* interpolationStrategy)
 : FullBackoffStrategy(languageModel, baseFileName, interpolationStrategy)
{
	// TODO Auto-generated constructor stub

}

LimitedBackoffStrategy::~LimitedBackoffStrategy() {
	// TODO Auto-generated destructor stub
}

std::string LimitedBackoffStrategy::name() const
{
	return "limited";
}

double LimitedBackoffStrategy::prob(const Pattern& context, const Pattern& focus)
{
	L_S << "LimitedBackoffStrategy: Estimating prob for " << languageModel.toString(context)
			<< " " << languageModel.toString(focus) << "\n";

	// implement skipgrams from layer 3 on
	double prob = languageModel.getProbLS4(focus, context, interpolationStrategy, cache);

	double logProb = log2(prob);

	if(false /* oov */)
	{
		++sentOovs;
	}

	++sentCount;
	sentLLH -= logProb;

	L_S << "LimitedBackoffStrategy: \t\t" << logProb << "\n";

	writeProbToFile(focus, context, logProb);

	return logProb;
}

double LimitedBackoffStrategy::getNormalisationFactor(const Pattern& context)
{
//	std::map<Pattern, double>::const_iterator it = weights.find(context);
//	if(it == weights.end())
//	{
//		// for each pattern
//		//    if in crp: N++
//		//    if not in crp: B++
//		//		probs += prob
//		unsigned long N = 0;
//		unsigned long B = 0;
//		double probs = 0.0;
//
//		for(Pattern p: languageModel.getVocabulary())
//		{
//
//		}
//	}
//	else
//	{
//		return it.second;
//	}



	return 0.0;
}

} /* namespace SLM */
