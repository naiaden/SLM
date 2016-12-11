//============================================================================
// Name        : SLM.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>

#include "Logging.h"
#include "ProgramOptions.h"

#include "LanguageModel.h"
#include "Utils.cpp"

int main(int argc, char** argv) {

	SLM::ProgramOptions po(argc, argv);
//	SLM::LanguageModel lm(po);

	for(auto& inputFile : po.getTestInputFiles())
	{
		L_V << "SLM: Reading " << inputFile << "\n";
		std::ifstream file(inputFile);

		std::string retrievedString;
		while(std::getline(file, retrievedString))
		{
			// hack
			retrievedString = "<s> <s> <s> " + retrievedString;
			std::cout << retrievedString << std::endl;

			std::vector<std::string> words = whitespaceTokeniser(retrievedString);
		}

	}

			// for pattern




	return EXIT_SUCCESS;
}
