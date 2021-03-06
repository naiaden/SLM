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
#include <experimental/optional>

#include "LanguageModel.h"

namespace SLM {

const std::string outputExtension = "output";
const std::string probsExtension = "probs";
const std::string sentencesExtension = "sentences";

class BackoffStrategy {
public:
	virtual ~BackoffStrategy();

	void openFiles(SLM::LanguageModel& languageModel, const std::string& baseFileName);
	virtual void init() const;

	virtual std::string name() const = 0;

	virtual double prob(const Pattern& context, const Pattern& focus, bool isOOV) = 0;
	int nextFile();
        double perplexityAndNextFile();
	int nextLine();
	void done();

	unsigned long long getTotalCount() const;
	unsigned long long getTotalOovs() const;

        virtual bool addToCache(const Pattern& pattern, double val) = 0;
        virtual std::experimental::optional<double> getFromCache(const Pattern& pattern) = 0;
protected:
	BackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName);

//	void init(SLM::LanguageModel& languageModel, const std::string& baseFileName);

	void writeProbToFile(const Pattern& focus, const Pattern& context, double logProb, bool isOOV);

	double totalLLH = 0.0;
	unsigned long long totalCount = 0;
	unsigned long long totalOovs = 0;
	unsigned long totalSentences = 0;

	int files = 0;
	double fileLLH = 0.0;
	unsigned long long fileCount = 0;
	unsigned long long fileOovs = 0;

	unsigned long sentences = 0;
	double sentLLH = 0.0;
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
