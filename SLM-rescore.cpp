//============================================================================
// Name        : SLM.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <tuple>

#include "Logging.h"
#include "ProgramOptions.h"

#include "BackoffStrategies.h"
#include "NgramBackoffStrategy.h"
#include "LanguageModel.h"
#include "Utils.h"

#include "Timer.h"

#include "NBestList.h"

int main(int argc, char** argv) {

	SLM::ProgramOptions po(argc, argv);
	SLM::LanguageModel lm(po);
	SLM::NgramBackoffStrategy bos(lm, po.getTestModelName());
	bos.init(lm, po.getTestModelName());

	SLM::ProgressTimer pt;

	for(std::string inputFile : po.getTestInputFiles())
	{
		L_V << "SLMr: Reading " << inputFile << "\n";
		std::ifstream file(inputFile);
		bos.nextFile();
		pt.nextFile();

		int currentRank = 0;
		SLM::NBestList nbestList;

		std::string retrievedString;
		while(std::getline(file, retrievedString))
		{

			std::stringstream linestream(retrievedString);

			std::string acousticModelScoreString;
			double acousticModelScore;

			std::string languageModelScoreString;
			double languageModelScore;

			std::string numberOfWordsString;
			int numberOfWords;

			std::string sentenceString;

			std::getline(linestream, acousticModelScoreString, ' ');
			acousticModelScore = std::stod(acousticModelScoreString);

			std::getline(linestream, languageModelScoreString, ' ');
			languageModelScore = std::stod(languageModelScoreString);

			std::getline(linestream, numberOfWordsString, ' ');
			numberOfWords = std::stoi(numberOfWordsString);

			std::getline(linestream, sentenceString);

			SLM::NBestItem* nbi = new SLM::NBestItem(sentenceString, ++currentRank, acousticModelScore, languageModelScore, numberOfWords);
			nbestList.add(nbi);



//			// hack
			sentenceString = "<s> <s> " + sentenceString;
//			bos.nextLine();
//			pt.nextLine();
//
			std::vector<std::string> words = whitespaceTokeniser(sentenceString);
			L_P << "SLMr: Reading " << sentenceString << "\n";
			L_P << "SLMr: Contains " << words.size() << " words (incl. sentence markers)\n";

			double lprob = 0.0;
			int numberOfUsedPatterns = 0;

			for(int i = (4-1); i < words.size(); ++i)
			{
				std::stringstream contextStream;
				contextStream << words[i-(4-1)];

				for(int ii = 1; ii < 4 - 1; ++ii)
				{
					contextStream << " " << words[i-(4-1)+ii];
				}

				Pattern context = lm.toPattern(contextStream.str());
				Pattern focus = lm.toPattern(words[i]);

				L_P << "SLMr: [" << lm.toString(context) << "] " << lm.toString(focus) << "\n";
				double pr = bos.prob(context, focus);
				L_P << "SLMr: {" << pr << "}\n";

				lprob += pr;
//				pt.nextPattern();
//
//				pt.toString();
			}
			nbi->setRescore(lprob);

		}

		nbestList.determineNewRanks();
		nbestList.printToFile(inputFile, po.getTestOutputDirectory());

	}
//	bo.done();

	return EXIT_SUCCESS;
}
