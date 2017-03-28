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

	}
}
