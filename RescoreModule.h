/*
 * RescoreModule.h
 *
 *  Created on: Feb 21, 2017
 *      Author: louis
 */

#ifndef RESCOREMODULE_H_
#define RESCOREMODULE_H_

#include "BackoffStrategy.h"
#include "NBestList.h"
#include <pattern.h>

namespace SLM {

class RescoreModule {
public:
	RescoreModule(SLM::BackoffStrategy* bo, const std::string& outputDirectory);
	virtual ~RescoreModule();

	void nextFile(const std::string& originalFileName);
	void rescoreFile();
	void addLine(const std::string& sentenceString, int originalRank, double acousticModelScore, double languageModelScore, int numberOfWords);
	void rescoreLine();
	void evaluatePattern(const Pattern& focus, const Pattern& context, bool isOOV);
protected:
	SLM::BackoffStrategy* backoffStrategy;
	SLM::NBestList nbestList;

	SLM::NBestItem* currentLine = nullptr;

	double lprob = 0.0;
	int oovs = 0;
	int usedPatterns = 0;
	int usedPatternsForLine = 0;

	std::string originalFileName;
	std::string outputDirectory;
};

} /* namespace SLM */

#endif /* RESCOREMODULE_H_ */
