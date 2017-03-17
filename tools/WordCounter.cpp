/*
 * WordCounter.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: louis
 */



#include <string>
#include <iostream>
#include <vector>
#include <experimental/filesystem>

#include <unordered_map>
#include <memory>
#include <numeric>

#include "Logging.h"

#include "Utils.h"
#include "ToolUtils.h"
#include "ReferenceId.h"
#include "Sorter.h"
#include "FileWriter.h"
#include "FileReader.h"
#include "TextPreprocessor.h"
#include "ToolProgramOptions.h"
#include "WordList.h"

int main(int argc, char** argv)
{
	SLM::ToolProgramOptions po(argc, argv);


	SLM::CGNTextPreprocessor tpp;

	SLM::WordList wl(po.getInputPath(), po.getVocabFile());

    std::unordered_map<std::string, SLM::ReferenceId> referenceIds = referenceIdsPreprocessing(collectReferenceIds(po.getInputPath()), po.getLimitedReferenceIds());


    for(auto & r : referenceIds)
    {
    	SLM::ReferenceFileReader fr(po.getReferencePath(), r.first + ".stm");

    	int oov = 0;
    	for(auto & t : fr.getTokens())
    	{
    		if(!wl.contains(t)) ++oov;
    	}
    	std::cout << r.first << "\t" << oov << std::endl;
    }


//
//
//    for (auto & r : referenceIds)
//    {
//
//    	if(po.isProgramMode(SLM::ProgramMode::CEILING))
////    	if(true) // select best and select fase
//    	{
//    		SLM::ReferenceFileReader fr(po.getReferencePath(), r.first + ".stm");
//			std::vector<std::string> reference = fr.getTokens();
//
//
//    		std::vector<std::shared_ptr<SLM::BestHypotheses>> hypotheses;
//    		std::vector<std::string> hypothesesFiles = getFilesForReferenceId(r.second, po.getInputPath());
//    		for(auto& hFile : hypothesesFiles)
//    		{
//    			hypotheses.push_back(std::make_shared<SLM::BestHypotheses>(hFile, po.getInputPath(), sorter));
//    		}
//
//    		std::vector<std::string> hypothesisTokens;
//    		for(auto& hyp : hypotheses)
//    		{
//    			if(hyp->getBestHypothesis())
//    			{
//					std::vector<std::string> tokens = hyp->getBestHypothesis()->getTokens();
//					hypothesisTokens.insert(std::end(hypothesisTokens), std::begin(tokens), std::end(tokens));
//    			}
//    		}
//
//    		double localWER = WER(reference, tpp.removeFillers(hypothesisTokens, true));
//
//    		if(SLM::Logging::getInstance().doLog(SLM::LoggingLevel::ALL))
//			{
//				L_A << "[REFERENCE] " << join(reference, " ") << "\n[HYPOTHESIS] " << join(tpp.removeFillers(hypothesisTokens, true), " ") << "\n";
//			}
//
//
//    		globalWER.push_back(localWER);
//			L_I << r.first << "\t" << localWER << "\n";
//    	}
//
//
//    	if(po.isProgramMode(SLM::ProgramMode::SELECTBEST))
////    	if(false)
//		{
//    		collectHypothesesForReferenceId(r.second, po.getInputPath());
//
//    		SLM::ReferenceFileWriter fw(po.getOutputPath(), r.first + "-" + sorter->getName());
//    		SLM::ReferenceFileReader fr(po.getReferencePath(), r.first + ".stm");
//
//    		std::vector<std::string> reference = fr.getTokens();
//    		std::vector<std::string> hypothesisTokens;
//
//
//    		for(auto & nbl : r.second.getTimeSortedNBestLists())
//    		{
//    			L_V << "Processing " << nbl->getStartTime() << "\n";
//
//    			if(nbl->getHypotheses().size())
//    			{
//					std::vector<std::string> f = sorter->sort(*nbl).getTokens();
//					hypothesisTokens.insert(std::end(hypothesisTokens), std::begin(f), std::end(f));
//    			}
//    		}
//
//    		double localWER = WER(reference, tpp.removeFillers(hypothesisTokens, true));
//
//    		if(SLM::Logging::getInstance().doLog(SLM::LoggingLevel::ALL))
//    		{
//    			L_A << "[REFERENCE] " << join(reference, " ") << "\n[HYPOTHESIS] " << join(tpp.removeFillers(hypothesisTokens, true), " ") << "\n";
//    		}
//
//    		globalWER.push_back(localWER);
//    		L_I << r.first << "\t" << localWER << "\n";
//
//		}
//
//    	r.second.clear();
//    }
//
//    std::cout << "WER: " << std::accumulate( globalWER.begin(), globalWER.end(), 0.0)/globalWER.size() << std::endl;
//


}

