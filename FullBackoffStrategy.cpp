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

const std::string cacheExtension = "cache";

FullBackoffStrategy::FullBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName, SLM::InterpolationStrategy* interpolationStrategy)
 : BackoffStrategy(languageModel, baseFileName), interpolationStrategy(interpolationStrategy)
{
	init(languageModel, baseFileName);

	std::string cacheFileName  = baseFileName + "_" + name() + "." + cacheExtension;
	L_V << "FullBackoffStrategy: (" << name() << ")" << std::setw(30) << "Cache output file:" << cacheFileName << "\n";
	cacheOutputFile.open(cacheFileName);

}

FullBackoffStrategy::~FullBackoffStrategy() {
	cacheOutputFile.flush();
	cacheOutputFile.close();
}

std::string FullBackoffStrategy::name() const
{
	return "full" + interpolationStrategy->name();
}

bool FullBackoffStrategy::setIgnoreCache(bool setting)
{
	ignoreCache = setting;
	return ignoreCache;
}

void FullBackoffStrategy::writeCache()
{

}

double FullBackoffStrategy::prob(const Pattern& context, const Pattern& focus)
{
	L_S << "FullBackoffStrategy: Estimating prob for " << languageModel.toString(context)
			<< " " << languageModel.toString(focus) << "\n";

	// implement skipgrams from layer 3 on
	double prob = languageModel.getProbS4(focus, context, interpolationStrategy, cache, ignoreCache);

	double logProb = log2(prob);

	if(false /* oov */)
	{
		++sentOovs;
	}

	++sentCount;
	sentLLH -= logProb;

	L_S << "FullBackoffStrategy: \t\t" << logProb << "\n";

	writeProbToFile(focus, context, logProb);

	return logProb;
}

} /* namespace SLM */
