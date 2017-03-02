/*
 * RescoreModule.cpp
 *
 *  Created on: Feb 21, 2017
 *      Author: louis
 */

#include "RescoreModule.h"

#include "Logging.h"

namespace SLM {

RescoreModule::RescoreModule(SLM::BackoffStrategy* bo, const std::string& outputDirectory)
	: backoffStrategy(bo), outputDirectory(outputDirectory)
{
	// TODO Auto-generated constructor stub

}

RescoreModule::~RescoreModule() {
	// TODO Auto-generated destructor stub
}

void RescoreModule::nextFile(const std::string& originalFileName)
{
	this->originalFileName = originalFileName;

	nbestList = SLM::NBestList();
}

void RescoreModule::addLine(const std::string& originalSentenceString, int originalRank, double acousticModelScore, double languageModelScore, int numberOfWords)
{
	currentLine = new SLM::NBestItem(originalSentenceString, originalRank, acousticModelScore, languageModelScore, numberOfWords);


	lprob = 0.0;
	oovs = 0;
}

void RescoreModule::rescoreLine()
{
//	L_P << "RescoreModule: rescore ppl =" << pow(2, lprob/usedPatterns) << "\n";
	currentLine->setRescore(pow(2, lprob/usedPatterns));
	nbestList.add(currentLine);
}

void RescoreModule::rescoreFile()
{
	nbestList.determineNewRanks();
	nbestList.printToFile(originalFileName, outputDirectory);
}

void RescoreModule::evaluatePattern(const Pattern& focus, const Pattern& context, bool isOOV)
{
	double prob = backoffStrategy->prob(context, focus, isOOV);

	if(isOOV)
	{
		L_P << "RescoreModule: ***";
		++oovs;
	} else
	{
		L_P << "RescoreModule: ";
		++usedPatterns;
		lprob -= prob;
	}

	L_P << "prob =" << prob << "\n";
}


} /* namespace SLM */
