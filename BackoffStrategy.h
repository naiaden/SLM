/*
 * BackoffStrategy.h
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#ifndef BACKOFFSTRATEGY_H_
#define BACKOFFSTRATEGY_H_

#include <fstream>

#include <pattern.h>

#include "LanguageModel.h"

namespace SLM {

const std::string outputExtension = "output";
const std::string probsExtension = "probs";
const std::string sentencesExtension = "sentences";

class BackoffStrategy {
public:
	virtual ~BackoffStrategy();

	virtual std::string name() const = 0;

	virtual double prob(const Pattern& context, const Pattern& focus) = 0;
	int nextFile();
	int nextLine();
	void done();
protected:
	BackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName);

	void init(SLM::LanguageModel& languageModel, const std::string& baseFileName);

	double totalProb = 0.0;
	unsigned long long totalCount = 0;
	unsigned long long totalOovs = 0;
	unsigned long totalSentences = 0;

	int files = 0;
	double fileProb = 0.0;
	unsigned long long fileCount = 0;
	unsigned long long fileOovs = 0;
	unsigned long fileSentences = 0;

	unsigned long sentences = 0;
	double sentProb = 0.0;
	unsigned long long sentCount = 0;
	unsigned long long sentOovs = 0;

	std::ofstream outputFile;
	std::ofstream probsFile;
	std::ofstream sentsProbFile;

	SLM::LanguageModel& languageModel;
private:

};

} /* namespace SLM */

#endif /* BACKOFFSTRATEGY_H_ */
