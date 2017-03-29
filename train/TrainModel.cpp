/*
 * TrainModel.cpp
 *
 *  Created on: Mar 28, 2017
 *      Author: louis
 */


#include "TrainProgramOptions.h"
#include "TrainLanguageModel.h"
#include "TrainProgressPrinter.h"

#include "Logging.h"

int main(int argc, char** argv)
{
	SLM::TrainProgramOptions tpo(argc, argv);
	SLM::TrainLanguageModel tlm(tpo);

	SLM::TrainProgressPrinter tpp;
	tpp.start();

	for(int sample = 0; sample < 1/*tpo.getSamples()*/; ++sample)
	{
		SLM::PatternContainer* pc = tlm.getNextPattern();
		while(pc)
		{
//			std::cout << "[" << pc->sentenceNumber << "/" << pc->patternNumber << "] " << tlm.toString(pc->context) << " " << tlm.toString(pc->focus) << std::endl;
			pc = tlm.getNextPattern();

			if(sample > 0)
			{
				tlm.decrement(pc->focus, pc->context);
			}
			tlm.increment(pc->focus, pc->context);

			tpp.next();
		}

		if(sample % 10 == 9)
		{
			L_I << "\n[LLH=" << tlm.log_likelihood() << "]\n";
			tlm.resample_hyperparameters();
		}

		std::cout << "DONE" << std::endl;
	}
}
