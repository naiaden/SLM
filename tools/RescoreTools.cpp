//============================================================================
// Name        : RescoreTools.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <string>
#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include <stdlib.h>

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

// grid search optimal parameters
// best hyp
// oracle ceiling

int main(int argc, char** argv)
{
    SLM::ToolProgramOptions po(argc, argv);
    SLM::Sorter* sorter = po.getSorter();
    SLM::CGNTextPreprocessor tpp;

    std::unordered_map<std::string, SLM::ReferenceId> referenceIds = referenceIdsPreprocessing(collectReferenceIds(po.getInputPath()), po.getLimitedReferenceIds());

    std::vector<double> globalWER;

    SLM::GenericFileWriter fw(po.getOutputPath(), "selected_hyps");

    for (auto & r : referenceIds)
    {
        L_A << "\n\n--------------------\n\n";

    	if(po.isProgramMode(SLM::ProgramMode::CEILING))
    	{
            std::cout << "READING reference file " << r.first << "\n";
            SLM::ReferenceFileReader fr(po.getReferencePath(), r.first + ".stm");
            std::vector<std::string> reference = fr.getTokens();

            collectHypothesesForReferenceId(r.second, po.getInputPath(), po.addPadding());

            double currentWER = 10000.0;
            double bestWER = 10000.0;


            std::vector<std::vector<std::string>> potentialHypotheses;
            std::vector<std::string> hypothesisTokens;
            std::vector<std::vector<std::string>> bestHypotheses;

            for(auto & nbl : r.second.getTimeSortedNBestLists())
            {
                L_V << "I Processing " << nbl->getStartTime() << "\n";
                L_V << "I Contains " << nbl->getHypotheses().size() << " hypotheses\n";
                potentialHypotheses.push_back(nbl->getHypotheses().front()->getTokens());
                bestHypotheses.push_back(nbl->getHypotheses().front()->getTokens());
            }

            {
                hypothesisTokens.clear();
                for(int i = 0; i < potentialHypotheses.size(); ++i)
                {
                    hypothesisTokens.insert(std::end(hypothesisTokens), std::begin(potentialHypotheses.at(i)), std::end(potentialHypotheses.at(i)));
                }

                L_V << "I HYP " << join(hypothesisTokens, " ") << std::endl;

                currentWER = WER(reference, tpp.removeFillers(hypothesisTokens, true));
                L_P << "cWER: " << currentWER << std::endl;
            }

            int nCtr = 0;
            for(auto & nbl : r.second.getTimeSortedNBestLists())
            {
                L_V << "II Processing " << nbl->getStartTime() << "\n";
                L_V << "II Contains " << nbl->getHypotheses().size() << " hypotheses\n";
                for(auto hyp : nbl->getHypotheses())
                {
                    if(nCtr > 0)
                    {
                        potentialHypotheses.at(nCtr-1) = bestHypotheses.at(nCtr-1);
                    }

                    potentialHypotheses[nCtr] = hyp->getTokens();
                
                    // construct hypothesis
                    hypothesisTokens.clear();
                    for(int i = 0; i < potentialHypotheses.size(); ++i)
                    {
                        hypothesisTokens.insert(std::end(hypothesisTokens), std::begin(potentialHypotheses.at(i)), std::end(potentialHypotheses.at(i)));
                    }
                    currentWER = WER(reference, tpp.removeFillers(hypothesisTokens, true));

                    if(currentWER < bestWER)
                    {
                        L_I << "!" << nCtr << "," << currentWER << "! " << join(potentialHypotheses.at(nCtr), " ") << std::endl;
                        bestHypotheses.at(nCtr) = potentialHypotheses.at(nCtr);
                        bestWER = currentWER;
                    }
                }


                ++nCtr;
            }
            std::cout << r.first << "\t" << bestWER << std::endl;
            globalWER.push_back(bestWER);
        }

    	if(po.isProgramMode(SLM::ProgramMode::SELECTBESTWINFO))
        {
            L_P << "SelectBestWithInfo Mode is used\n";

            collectHypothesesForReferenceId(r.second, po.getInputPath(), po.addPadding());

            L_P << "Hypotheses collected\n";

            SLM::ReferenceFileWriter fw(po.getOutputPath(), r.first + "-" + sorter->getName());
            SLM::ReferenceFileReader fr(po.getReferencePath(), r.first + ".stm");

            L_P << "Files initialised\n";

            std::vector<std::string> reference = fr.getTokens();
            std::vector<std::string> hypothesisTokens;


            for(auto & nbl : r.second.getTimeSortedNBestLists())
            {
                L_V << "Processing " << nbl->getStartTime() << "\n";

                if(nbl->getHypotheses().size())
                {
                    std::vector<std::string> f = sorter->sort(*nbl).getTokens();
                    hypothesisTokens.insert(std::end(hypothesisTokens), std::begin(f), std::end(f));
                }
            }

            LevenshteinInfo li = levenshtein(reference, tpp.removeFillers(hypothesisTokens, true));
            double localWER = li.wer; //WER(reference, tpp.removeFillers(hypothesisTokens, true));

            if(SLM::Logging::getInstance().doLog(SLM::LoggingLevel::ALL))
            {
                L_A << "[REFERENCE] " << join(reference, " ") << "\n[HYPOTHESIS] " << join(tpp.removeFillers(hypothesisTokens, true), " ") << "\n";
            }
    		
            std::cout << r.first 
                << "\t" << 0.01*reference.size()*localWER << "\t" << localWER 
                << "\t" << li.ins << "\t" << 1.0*li.ins/reference.size() 
                << "\t" << li.del << "\t" << 1.0*li.del/reference.size() 
                << "\t" << li.sub << "\t" << 1.0*li.sub/reference.size() 
                << std::endl;

            fw.addLine(join(tpp.removeFillers(hypothesisTokens, true), " "));

            globalWER.push_back(localWER);
            //L_I << r.first << "\t" << localWER << "\n";

            //std::cout << " WER: " << localWER << std::endl;
        }

    	if(po.isProgramMode(SLM::ProgramMode::SELECTBEST))
        {
            L_P << "SelectBest Mode is used\n";

            collectHypothesesForReferenceId(r.second, po.getInputPath(), po.addPadding());

            L_P << "Hypotheses collected\n";

            SLM::ReferenceFileWriter fw(po.getOutputPath(), r.first + "-" + sorter->getName());
            SLM::ReferenceFileReader fr(po.getReferencePath(), r.first + ".stm");

            L_P << "Files initialised\n";

            std::vector<std::string> reference = fr.getTokens();
            std::vector<std::string> hypothesisTokens;


            for(auto & nbl : r.second.getTimeSortedNBestLists())
            {
                L_V << "Processing " << nbl->getStartTime() << "\n";

                if(nbl->getHypotheses().size())
                {
                    std::vector<std::string> f = sorter->sort(*nbl).getTokens();
                    hypothesisTokens.insert(std::end(hypothesisTokens), std::begin(f), std::end(f));
                }
            }

            double localWER = WER(reference, tpp.removeFillers(hypothesisTokens, true));

            if(SLM::Logging::getInstance().doLog(SLM::LoggingLevel::ALL))
            {
                L_A << "[REFERENCE] " << join(reference, " ") << "\n[HYPOTHESIS] " << join(tpp.removeFillers(hypothesisTokens, true), " ") << "\n";
            }
    		
            std::cout << r.first << "\t" << localWER << std::endl;//"\t" << join(tpp.removeFillers(hypothesisTokens, true), " ") << std::endl;

            fw.addLine(join(tpp.removeFillers(hypothesisTokens, true), " "));

            globalWER.push_back(localWER);
            //L_I << r.first << "\t" << localWER << "\n";

            //std::cout << " WER: " << localWER << std::endl;
        }

    	r.second.clear();


    }

    std::cout << "GWER: " << std::accumulate( globalWER.begin(), globalWER.end(), 0.0)/globalWER.size() << std::endl;

    delete sorter;



}
