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
//	init(languageModel, baseFileName);

//	std::string cacheFileName  = baseFileName + "_" + name() + "." + cacheExtension;
//	L_V << "FullBackoffStrategy: (" << name() << ")" << std::setw(30) << "Cache output file:" << cacheFileName << "\n";
//	cacheOutputFile.open(cacheFileName);

}

void FullBackoffStrategy::init(SLM::LanguageModel& languageModel, const std::string& baseFileName)
{
	openFiles(languageModel, baseFileName);

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

bool FullBackoffStrategy::addToCache(const Pattern& pattern, double val)
{
    if(!ignoreCache)
    {
        cache[pattern] = val;
        return true;
    }

    return false;
}

void FullBackoffStrategy::writeCache()
{

}

double FullBackoffStrategy::prob(const Pattern& context, const Pattern& focus, bool isOOV)
{
	L_S << "FullBackoffStrategy(" << interpolationStrategy->name() << "): Estimating prob for " << languageModel.toString(context)
			<< " " << languageModel.toString(focus) << "\n";

	double logProb = 0.0;

	if(!isOOV)
	{
		// implement skipgrams from layer 3 on
		double prob = languageModel.getProbS4(focus, context, interpolationStrategy, cache, ignoreCache);
//		double prob = languageModel.getProb4(focus, context);

		logProb = log2(prob);

		++sentCount;
		sentLLH -= logProb;
	} else
	{
		++sentOovs;
	}

	L_S << "FullBackoffStrategy(" << interpolationStrategy->name() << "): \t\t" << logProb << "\n";

	writeProbToFile(focus, context, logProb, isOOV);

	return logProb;
}

} /* namespace SLM */
