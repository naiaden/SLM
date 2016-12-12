/*
 * BackoffStrategy.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: onrust
 */

#include "BackoffStrategy.h"

#include "Logging.h"

namespace SLM {

BackoffStrategy::BackoffStrategy(SLM::LanguageModel& languageModel)
 : languageModel(languageModel)
{
	// TODO Auto-generated constructor stub

}

BackoffStrategy::~BackoffStrategy() {
	// TODO Auto-generated destructor stub
}

int BackoffStrategy::nextFile()
{
	L_V << "BackoffStrategy: next file\n";

	++files;
	totalCount += fileCount;
	totalOovs += fileOovs;
	totalProb += fileProb;

	fileCount = 0;
	fileOovs = 0;
	fileProb = 0.0;


	return files;
}

int BackoffStrategy::nextLine()
{
	L_V << "BackoffStrategy: next sentence\n";

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
	L_V << "BackoffStrategy: done\n";
}

} /* namespace SLM */
