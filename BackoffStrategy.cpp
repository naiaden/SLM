/*
 * BackoffStrategy.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#include "BackoffStrategy.h"

#include "Logging.h"

namespace SLM {

BackoffStrategy::BackoffStrategy(SLM::LanguageModel& languageModel, const std::string& baseFileName)
 : languageModel(languageModel)
{
}

void BackoffStrategy::init(SLM::LanguageModel& languageModel, const std::string& baseFileName)
{
	std::string outputFileName = baseFileName + name() + "." + outputExtension;
	L_V << "BackoffStrategy: (" << name() << ")" << std::setw(30) << "Output file:" << outputFileName << "\n";
	outputFile.open(outputFileName);

	std::string probsFileName = baseFileName + name() + "." + probsExtension;
	L_V << "BackoffStrategy: (" << name() << ")" << std::setw(30) << "Probs output file:" << probsFileName << "\n";
	probsFile.open(probsFileName);

	std::string sentenceFileName  = baseFileName + name() + "." + sentencesExtension;
	L_V << "BackoffStrategy: (" << name() << ")" << std::setw(30) << "Sentences output file:" << sentenceFileName << "\n";
	sentsProbFile.open(sentenceFileName);
}

BackoffStrategy::~BackoffStrategy() {
	outputFile.close();
	probsFile.close();
	sentsProbFile.close();
}

int BackoffStrategy::nextFile()
{
	done();

	++files;
	L_V << "BackoffStrategy: (" << name() << ") next file\n";


	fileCount = 0;
	fileOovs = 0;
	fileProb = 0.0;

	sentences = 0;
	sentCount = 0;
	sentOovs = 0;
	sentProb = 0.0;


	return files;
}

int BackoffStrategy::nextLine()
{
	L_V << "BackoffStrategy: (" << name() << ") next sentence\n";

	++sentences;

	fileCount += sentCount;
	fileOovs += sentOovs;
	fileProb += sentProb;

	sentCount = 0;
	sentOovs = 0;
	sentProb = 0.0;

}

void BackoffStrategy::done()
{
//	L_V << "BackoffStrategy: done\n";
	if(files)
	{
		L_I << "BackoffStrategy: " << name() << " #" << fileCount << "/" << fileOovs << "[" << sentences << "]:" << fileProb << "\n";
	}

	totalCount += fileCount;
	totalOovs += fileOovs;
	totalProb += fileProb;
}

void BackoffStrategy::writeProbToFile(const Pattern& focus, const Pattern& context, double logProb)
{
	if(languageModel.isOOV(focus))
	{
		probsFile << "***";
	}

	probsFile << "p(" << languageModel.toString(focus) << " |"
			  << languageModel.toString(context) << ") = "
			  << std::fixed << std::setprecision(20) << logProb
			  << "\n";
}

} /* namespace SLM */
