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
#include "LanguageModel.h"
#include "Utils.cpp"

int main(int argc, char** argv) {

	SLM::ProgramOptions po(argc, argv);
	SLM::LanguageModel lm(po);
	SLM::BackoffStrategies bo(po, lm);

	for(std::string inputFile : po.getTestInputFiles())
	{
		L_V << "SLM: Reading " << inputFile << "\n";
		std::ifstream file(inputFile);
		bo.nextFile();

		std::string retrievedString;
		while(std::getline(file, retrievedString))
		{
			// hack
			retrievedString = "<s> <s> <s> " + retrievedString;
			bo.nextLine();

			std::vector<std::string> words = whitespaceTokeniser(retrievedString);
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

				L_P << "SLM: [" << lm.toString(context) << "] " << lm.toString(focus) << "\n";
				bo.prob(context, focus);
			}
		}

	}

	return EXIT_SUCCESS;
}
