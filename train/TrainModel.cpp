/*
 * TrainModel.cpp
 *
 *  Created on: Mar 28, 2017
 *      Author: louis
 */


#include "TrainProgramOptions.h"
#include "TrainLanguageModel.h"

int main(int argc, char** argv)
{
	SLM::TrainProgramOptions tpo(argc, argv);
	SLM::TrainLanguageModel tlm(tpo);

	for(int sample = 0; sample < tpo.getSamples(); ++sample)
	{
		SLM::PatternContainer* pc = tlm.getNextPattern();
		while(pc)
		{
			std::cout << "[" << pc->sentenceNumber << "/" << pc->patternNumber << "] " << tlm.toString(pc->context) << " " << tlm.toString(pc->focus) << std::endl;
			pc = tlm.getNextPattern();
		}

		std::cout << "DONE" << std::endl;
	}
}
