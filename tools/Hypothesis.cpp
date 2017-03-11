/*
 * Hypothesis.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#include "Hypothesis.h"

#include <vector>
#include <iostream>

#include "Utils.h"

namespace SLM {

Hypothesis::Hypothesis(const std::string& hypothesisEntry) {
	sentence = hypothesisEntry;

	std::vector<std::string> entries = whitespaceTokeniser(hypothesisEntry);

	acousticScore = std::stod(entries[0]);
	languageModelScore = std::stod(entries[1]);
	count = std::stoi(entries[2]);

	tokens = std::vector<std::string>(entries.begin() + 3, entries.end()) ;

//	std::cout << acousticScore << " - " << languageModelScore << " - " << join(tokens, " ") << std::endl;
}

Hypothesis::~Hypothesis() {
	// TODO Auto-generated destructor stub
}

double Hypothesis::getAcousticScore() const
{
	return acousticScore;
}

double Hypothesis::getLanguageModelScore() const
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

int Hypothesis::getCount() const
{
	return count;
}

} /* namespace SLM */
