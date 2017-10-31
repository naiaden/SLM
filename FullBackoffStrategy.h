/*
 * FullBackoffStrategy.h
 *
 *  Created on: Dec 13, 2016
 *      Author: onrust
 */

#ifndef FULLBACKOFFSTRATEGY_H_
#define FULLBACKOFFSTRATEGY_H_

#include "BackoffStrategy.h"

#include <unordered_map>

namespace SLM {

class InterpolationStrategy;

const std::string cacheExtension = "cache";

class FullBackoffStrategy: public BackoffStrategy
{
public:
	FullBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName, SLM::InterpolationStrategy* interpolationStrategy);
	virtual ~FullBackoffStrategy();

	virtual void init(SLM::LanguageModel& languageModel, const std::string& baseFileName);

	virtual std::string name() const;
	double prob(const Pattern& context, const Pattern& focus, bool isOOV);

        bool addToCache(const Pattern& pattern, double val);
	bool setIgnoreCache(bool setting);
protected:

	void writeCache();

	SLM::InterpolationStrategy* interpolationStrategy;

	std::ofstream cacheOutputFile;

	std::unordered_map<Pattern, double> cache;
	bool ignoreCache = false;
};

} /* namespace SLM */

#endif /* FULLBACKOFFSTRATEGY_H_ */
