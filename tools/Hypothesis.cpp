/*
 * Hypothesis.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#include "Hypothesis.h"

#include <vector>
#include <iostream>

#include "Logging.h"
#include "Utils.h"

namespace SLM {

PartialHypothesis::PartialHypothesis(const std::string& hypothesisEntry) {

	std::vector<std::string> entries = whitespaceTokeniser(hypothesisEntry);

	if(entries.size() > 4)
	{
		acousticScore = std::stod(entries[0]);
		languageModelScore = std::stod(entries[1]);
		count = std::stoi(entries[2]);
	} else
	{
		throw SLM::IncompleteHypothesis();
	}
}

PartialHypothesis::~PartialHypothesis()
{

}

Hypothesis::Hypothesis(const std::string& hypothesisEntry, bool addPadding) : PartialHypothesis(hypothesisEntry) {
	if(addPadding)
        {
                sentence = "<s> <s> " + hypothesisEntry;
        } else
        {
                sentence = hypothesisEntry;
        }


	std::vector<std::string> entries = whitespaceTokeniser(hypothesisEntry);

	if(entries.size() > 4)
	{
		tokens = std::vector<std::string>(entries.begin() + 3, entries.end()) ;
	} else
	{
		throw SLM::IncompleteHypothesis();
	}
}

Hypothesis::~Hypothesis() {
	// TODO Auto-generated destructor stub
}

double PartialHypothesis::getAcousticScore() const
{
	return acousticScore;
}

double PartialHypothesis::getLanguageModelScore() const
{
	return languageModelScore;
}

std::string Hypothesis::getSentence() const
{
	return sentence;
}

std::vector<std::string> Hypothesis::getTokens() const
{
	return tokens;
}

int PartialHypothesis::getCount() const
{
	return count;
}

} /* namespace SLM */
