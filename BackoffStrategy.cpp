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

double BackoffStrategy::perplexityAndNextFile()
{
	totalCount += fileCount;
	totalOovs += fileOovs;
	totalLLH += fileLLH;
	totalSentences += sentences;

        double filePerplexity = 0.0;

	if(files)
	{
//		done();

		filePerplexity = pow(2, fileLLH/(fileCount/*-totalOovs*/));
		L_I << "BackoffStrategy: FILE (" << name() << ")\tF" << sentences << "\tP" << filePerplexity << "\tC" << (fileCount/*-totalOovs*/) << "\tO" << fileOovs << "\tL" << fileLLH << std::endl;
		outputFile << "BackoffStrategy: FILE (" << name() << ")\tF" << sentences << "\tP" << filePerplexity << "\tC" << (fileCount/*-totalOovs*/) << "\tO" << fileOovs << "\tL" << fileLLH << std::endl;


		double totalPerplexity = pow(2, totalLLH/(totalCount/*-totalOovs*/));
		L_I << "BackoffStrategy: TOTAL (" << name() << ")\tS" << sentences << "\tP" << totalPerplexity << "\tC" << (totalCount/*-totalOovs*/) << "\tO" << totalOovs << "\tL" << totalLLH << std::endl;

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

        return filePerplexity;
    
}

int BackoffStrategy::nextFile()
{
        perplexityAndNextFile();

	return files;
}

int BackoffStrategy::nextLine()
{
	if(sentences)
	{
		double sentPerplexity = pow(2, sentLLH/(1.0*sentCount/*-sentOovs*/));
		L_V << "BackoffStrategy: SENT (" << name() << ") sentence:" << sentences << "\tppl:" << sentPerplexity << "\tcount:" << (sentCount/*-sentOovs*/) << "\toovs:" << sentOovs << "\tllh:" << sentLLH << std::endl;
		sentsProbFile << sentences << "\t" << sentPerplexity << "\t" << (sentCount/*-sentOovs*/) << "\t" << sentOovs << "\t" << sentLLH << std::endl;
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
	nextLine();

//	double filePerplexity = pow(2, fileLLH/(fileCount/*-totalOovs*/));
//	L_I << "BackoffStrategy: " << name() << "\tF" << sentences << "\tP" << filePerplexity << "\tC" << (fileCount/*-totalOovs*/) << "\tO" << fileOovs << "\tL" << fileLLH << std::endl;
	--sentences;

	//	L_V << "BackoffStrategy: done\n";

	nextFile();

	double totalPerplexity = pow(2, totalLLH/(totalCount/*-totalOovs*/));
	outputFile << "BackoffStrategy: TOTAL (" << name() << ")\tS" << totalSentences << "\tP" << totalPerplexity << "\tC" << (totalCount/*-totalOovs*/) << "\tO" << totalOovs << "\tL" << totalLLH << std::endl;

//	if(files)
//	{
//		L_I << "BackoffStrategy: " << name() << " #C" << fileCount << "/O" << fileOovs << "[S" << sentences << "]:L" << fileLLH << "\n";
//	}

//	totalCount += fileCount;
//	totalOovs += fileOovs;
//	totalLLH += fileLLH;

//	double totalPerplexity = pow(2, totalLLH/(totalCount/*-totalOovs*/));
//	L_I << "BackoffStrategy: " << name() << "\tS" << sentences << "\tP" << totalPerplexity << "\tC" << (totalCount/*-totalOovs*/) << "\tO" << totalOovs << "\tL" << totalLLH << std::endl;
}

void BackoffStrategy::writeProbToFile(const Pattern& focus, const Pattern& context, double logProb, bool isOOV)
{
	if(isOOV)
	{
		/*std::cout*/ probsFile << "***";
	}

	/*std::cout*/ probsFile << "p(" << languageModel.toString(focus) << " |"
			  << languageModel.toString(context) << ") = "
			  << std::fixed << std::setprecision(20) << logProb
			  << std::endl;
}

} /* namespace SLM */
