/*
 * NgramBackoffStrategy.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#include "NgramBackoffStrategy.h"

#include "Logging.h"

namespace SLM {

NgramBackoffStrategy::NgramBackoffStrategy(SLM::LanguageModel& languageModel)
 : BackoffStrategy(languageModel)
{
	// TODO Auto-generated constructor stub

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
	L_P << "NgramBackoffStrategy: Estimating prob for " << languageModel.toString(context)
			<< " " << languageModel.toString(focus) << "\n";
	return 2.0;
}

} /* namespace SLM */
