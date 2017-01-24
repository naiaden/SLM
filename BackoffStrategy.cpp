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

void BackoffStrategy::init() const
{

}

void BackoffStrategy::openFiles(SLM::LanguageModel& languageModel, const std::string& baseFileName)
{
	std::string outputFileName = baseFileName + "_" + name() + "." + outputExtension;
	L_V << "BackoffStrategy: (" << name() << ")" << std::setw(30) << "Output file:" << outputFileName << "\n";
	outputFile.open(outputFileName);

	std::string probsFileName = baseFileName + "_" + name() + "." + probsExtension;
	L_V << "BackoffStrategy: (" << name() << ")" << std::setw(30) << "Probs output file:" << probsFileName << "\n";
	probsFile.open(probsFileName);

	std::string sentenceFileName  = baseFileName + "_" + name() + "." + sentencesExtension;
	L_V << "BackoffStrategy: (" << name() << ")" << std::setw(30) << "Sentences output file:" << sentenceFileName << "\n";
	sentsProbFile.open(sentenceFileName);
}

BackoffStrategy::~BackoffStrategy() {
	outputFile.flush();
	outputFile.close();
	probsFile.flush();
	probsFile.close();
	sentsProbFile.flush();
	sentsProbFile.close();
}

int BackoffStrategy::nextFile()
{
	if(files)
	{
		done();
	}

	++files;
	L_V << "BackoffStrategy: (" << name() << ") next file\n";


	fileCount = 0;
	fileOovs = 0;
	fileLLH = 0.0;

	sentences = 0;
	sentCount = 0;
	sentOovs = 0;
	sentLLH = 0.0;


	return files;
}

int BackoffStrategy::nextLine()
{
	L_V << "BackoffStrategy: (" << name() << ") next sentence\n";

	if(sentences)
	{
		double sentPerplexity = pow(2, sentLLH/(sentCount-sentOovs));
		sentsProbFile << sentences << "\t" << sentPerplexity << "\t" << (sentCount-sentOovs) << "\t" << sentOovs << "\t" << sentLLH << std::endl;
	}

	++sentences;

	fileCount += sentCount;
	fileOovs += sentOovs;
	fileLLH += sentLLH;

	sentCount = 0;
	sentOovs = 0;
	sentLLH = 0.0;

	return sentences;
}

void BackoffStrategy::done()
{
//	L_V << "BackoffStrategy: done\n";
	nextLine();

	if(files)
	{
		L_I << "BackoffStrategy: " << name() << " #" << fileCount << "/" << fileOovs << "[" << sentences << "]:" << fileLLH << "\n";
	}

	totalCount += fileCount;
	totalOovs += fileOovs;
	totalLLH += fileLLH;

	double totalPerplexity = pow(2, totalLLH/(totalCount-totalOovs));
	L_I << "BackoffStrategy: " << name() << "\t" << sentences-1 << "\t" << totalPerplexity << "\t" << (totalCount-totalOovs) << "\t" << totalOovs << "\t" << totalLLH << std::endl;
}

void BackoffStrategy::writeProbToFile(const Pattern& focus, const Pattern& context, double logProb)
{
	if(languageModel.isOOV(focus))
	{
		/*probsFile*/ std::cout << "***";
	}

	/*probsFile*/ std::cout << "p(" << languageModel.toString(focus) << " |"
			  << languageModel.toString(context) << ") = "
			  << std::fixed << std::setprecision(20) << logProb
			  << std::endl;
}

} /* namespace SLM */
