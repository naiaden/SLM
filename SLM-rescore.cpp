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
#include <stdexcept>

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
	SLM::BackoffStrategies bo(po, lm);

	SLM::ProgressTimer pt;

	for(std::string inputFile : po.getTestInputFiles())
	{
		L_V << "SLMr: Reading " << inputFile << "\n";
		std::ifstream file(inputFile);

		bo.nextFile();
		pt.nextFile();

		int currentRank = 0;
		SLM::NBestList nbestList;

		std::string retrievedString;
		while(std::getline(file, retrievedString))
		{
			bo.nextLine();
			pt.nextLine();

			std::stringstream linestream(retrievedString);

			std::string acousticModelScoreString;
			double acousticModelScore;
			std::string languageModelScoreString;
			double languageModelScore;
			std::string numberOfWordsString;
			int numberOfWords;
			std::string sentenceString;

			try
			{
				std::getline(linestream, acousticModelScoreString, ' ');
				acousticModelScore = std::stod(acousticModelScoreString);
				std::getline(linestream, languageModelScoreString, ' ');
				languageModelScore = std::stod(languageModelScoreString);
				std::getline(linestream, numberOfWordsString, ' ');
				numberOfWords = std::stoi(numberOfWordsString);
				std::getline(linestream, sentenceString);
			} catch (const std::invalid_argument& ia)
			{
				std::cerr << "Invalid argument: " << ia.what() << '\n';
				std::cerr << "In file: " << inputFile << std::endl;
				std::cerr << "For line (" << currentRank << "): " << retrievedString << std::endl;
				continue;
			}

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

				try
				{
					Pattern context = lm.toPattern(contextStream.str());
					Pattern focus = lm.toPattern(words[i]);

					L_P << "SLMr: [" << lm.toString(context) << "] " << lm.toString(focus) << "\n";
					bo.prob(context, focus);
//					double pr = bo.prob(context, focus);
//					L_P << "SLMr: {" << pr << "}\n";

//					if(lm.isOOV(focus))
//					{
//						//++oovs;
//					}
//					else
//					{
						++numberOfUsedPatterns;
						lprob += pr;
//					}
				} catch (const UnknownTokenError &e)
				{
//					std::cerr << "Invalid argument: " << e.what() << '\n';
					std::cerr << "Unknown token error in file: " << inputFile << std::endl;
					std::cerr << "For line (" << currentRank << "): " << retrievedString << std::endl;
					std::cerr << "In pattern: " << contextStream.str() << " " << words[i] << std::endl;
					continue;
				}

				pt.nextPattern();

				pt.toString();

			}
			nbi->setRescore(pow(2, lprob/numberOfUsedPatterns));

		}

		nbestList.determineNewRanks();
		nbestList.printToFile(inputFile, po.getTestOutputDirectory());

	}
//	bo.done();

	return EXIT_SUCCESS;
}
