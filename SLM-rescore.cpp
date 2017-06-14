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
#include "RescoreModule.h"
#include "TextPreprocessor.h"

int main(int argc, char** argv) {

	SLM::ProgramOptions po(argc, argv);
	SLM::LanguageModel lm(po);

	SLM::BackoffStrategy* bo = SLM::BackoffStrategiesFactory::fromProgramOptions(po, lm)[0];
	SLM::RescoreModule rm(bo, po.getTestOutputDirectory());

	SLM::ProgressTimer pt;
        if(po.isDisableProgress())
        {
            pt.disablePrinting();
        }

	SLM::CGNTextPreprocessor cgnTP;

	for(std::string inputFile : po.getTestInputFiles())
	{
		L_V << "SLMr: Reading " << inputFile << "\n";
		std::ifstream file(inputFile);

		bo->nextFile();
		pt.nextFile();
		rm.nextFile(inputFile);

		int currentRank = 0;
		int lineNumber = 0;

		std::string retrievedString;
		while(std::getline(file, retrievedString))
		{
			++lineNumber;
			bo->nextLine();
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
                                sentenceString = trim(sentenceString);
			} catch (const std::invalid_argument& ia)
			{
				std::cerr << "Invalid argument: " << ia.what() << '\n';
				std::cerr << "In file: " << inputFile << std::endl;
				std::cerr << "For line (" << lineNumber << "): " << retrievedString << std::endl;
				continue;
			}

//			rm.addLine(sentenceString, ++currentRank, acousticModelScore, languageModelScore, numberOfWords);
                        if(!endsWith(sentenceString, "</s>"))
                        {
                            std::cerr << "Invalid line (no </s>): " << sentenceString << "(" << inputFile << ":" << lineNumber << ")\n";
                            continue;
                        }

			// hack
			sentenceString = "<s> <s> " + sentenceString;

			std::vector<std::string> tokens = whitespaceTokeniser(sentenceString);
			std::vector<std::string> words = cgnTP.removeFillers(tokens);
			L_P << "SLMr: Reading " << sentenceString << "\n";
			L_P << "SLMr: Contains " << words.size() << " words (incl. sentence markers)\n";

			sentenceString = join(words, " ");
			L_P << "SLMr: Normalised to " << sentenceString << "\n";
			rm.addLine(sentenceString, ++currentRank, acousticModelScore, languageModelScore, numberOfWords);

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

					bool isOOV = lm.isOOV(focus);

					rm.evaluatePattern(focus, context, isOOV);

				} catch (const UnknownTokenError &e)
				{
//					std::cerr << "Invalid argument: " << e.what() << '\n';
					std::cerr << "Unknown token error in file: " << inputFile << std::endl;
					std::cerr << "For line (" << lineNumber << "): " << retrievedString << std::endl;
					std::cerr << "In pattern: " << contextStream.str() << " " << words[i] << std::endl;
					continue;
				}
//
				pt.nextPattern();
//
				pt.toString();
//
			}
			rm.rescoreLine();
		}
		rm.rescoreFile();

	}
	bo->done();

	return EXIT_SUCCESS;
}
