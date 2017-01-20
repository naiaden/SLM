/*
 * NgramBackoffStrategy.h
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#ifndef NGRAMBACKOFFSTRATEGY_H_
#define NGRAMBACKOFFSTRATEGY_H_

#include "BackoffStrategy.h"

namespace SLM {

class NgramBackoffStrategy: public BackoffStrategy {
public:
	NgramBackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName);
	virtual ~NgramBackoffStrategy();

	virtual void init(SLM::LanguageModel& languageModel, const std::string& baseFileName);

	std::string name() const;
	double prob(const Pattern& context, const Pattern& focus);
};

} /* namespace SLM */

#endif /* NGRAMBACKOFFSTRATEGY_H_ */
