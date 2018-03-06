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
#include <algorithm>

#include "Logging.h"
#include "ProgramOptions.h"

#include "BackoffStrategies.h"
#include "LanguageModel.h"
#include "FullBackoffStrategy.h"
#include "LimitedBackoffStrategy.h"
#include "ValueInterpolationStrategy.h"
#include "Utils.h"

#include "Timer.h"

class ParamSearcher
{
    public:
        ParamSearcher(SLM::ValueInterpolationWeights& viw): viw(viw) {}
        virtual ~ParamSearcher() {};

        double previousPPL = 100000;
        double currentPPL = 100000;
        double lowestPPL = 100000;
        double factor = 1;
        unsigned int iterations = 0;

        double updatePPL(double ppl)
        {
            if(ppl > 0) 
            { previousPPL = currentPPL;
              currentPPL = ppl;

              lastIterWasDecrease = (currentPPL < previousPPL);

                log.push_back(std::make_pair(viw.toString(), currentPPL));
            }

            if(previousPPL < currentPPL)
            {
                //factor *= -1;
                //L_V << "Switching factor to " << factor << std::endl;
            }

            lowestPPL = std::min(lowestPPL, currentPPL);

            std::cout << iterations << "\t["<< iterationsSinceChange << "]\t" << viw.toString() << "\t--->\tcurrent: " << currentPPL << "\tprevious: " << previousPPL << "\tlowest: " << lowestPPL << std::endl;
            return previousPPL;
        }

        bool lastIterWasDecrease = false;
        unsigned iterationsSinceChange = 0;
        SLM::ValueInterpolationWeights& viw;

        virtual int updateParam() = 0;

        std::vector<std::pair<std::string, double>> log;
};  

class GridParamSearcher : public ParamSearcher
{
    public:
        GridParamSearcher(SLM::ValueInterpolationWeights& viw) : ParamSearcher(viw) {}
        virtual ~GridParamSearcher() {} 

        double increment = 1;
        int range = 10;
        int min = 1;

        bool inRange(double val) const
        {
            return val < range && val > 0;
        }
        
        double getRandom()
        {
            return rand() % (range-min) + min;
        }


        int updateParam()
        {
            ++iterations;

            if(iterationsSinceChange > 25)
            {
                best_d =    getRandom();
                best_cd =   getRandom();
                best_bcd =  getRandom();
                best_b_d =  getRandom();
                best_abcd = getRandom();
                best_a_cd = getRandom();
                best_ab_d = getRandom();
                best_a__d = getRandom();
                viw.w_d =    getRandom();
                viw.w_cd =   getRandom();
                viw.w_bcd =  getRandom();
                viw.w_b_d =  getRandom();
                viw.w_abcd = getRandom();
                viw.w_a_cd = getRandom();
                viw.w_ab_d = getRandom();
                viw.w_a__d = getRandom();
                iterationsSinceChange = 0; 
            }

            ++iterationsSinceChange;

            if(currentParam == 0)
            {
                if(!inRange(viw.w_d))
                {
                    viw.w_d = best_d;
                    currentParam = 1;
                    viw.w_cd = min;

                } else
                {
                    if(tempPPL > currentPPL) 
                    {
                        tempPPL = currentPPL;
                        best_d = viw.w_d;
                        iterationsSinceChange=0;
                    }
                    viw.w_d += increment*factor;
                }
            }

            
            if(currentParam == 1)
            {
                if(!inRange(viw.w_cd))
                {
                    viw.w_cd = best_cd;
                    currentParam = 2;
                    viw.w_b_d = min;
                } else
                {
                    if(tempPPL > currentPPL) 
                    {
                        tempPPL = currentPPL;
                        best_cd = viw.w_cd;
                        iterationsSinceChange=0;
                    }
                    viw.w_cd += increment*factor;
                }
            }

            
            if(currentParam == 2)
            {
                if(!inRange(viw.w_b_d))
                {
                    viw.w_b_d = best_b_d;
                    currentParam = 3;
                    viw.w_bcd = min;
                } else
                {
                    if(tempPPL > currentPPL) 
                    {
                        tempPPL = currentPPL;
                        best_b_d = viw.w_b_d;
                        iterationsSinceChange=0;
                    }
                    viw.w_b_d += increment*factor;
                }
            }

            
            if(currentParam == 3)
            {
                if(!inRange(viw.w_bcd))
                {
                    viw.w_bcd = best_bcd;
                    currentParam = 4;
                    viw.w_a__d = min;
                } else
                {
                    if(tempPPL > currentPPL) 
                    {
                        tempPPL = currentPPL;
                        best_bcd = viw.w_bcd;
                        iterationsSinceChange=0;
                    }
                    viw.w_bcd += increment*factor;
                }
            }

            
            if(currentParam == 4)
            {
                if(!inRange(viw.w_a__d))
                {
                    viw.w_a__d = best_a__d;
                    currentParam = 5;
                    viw.w_ab_d = min;
                } else
                {
                    if(tempPPL > currentPPL) 
                    {
                        tempPPL = currentPPL;
                        best_a__d = viw.w_a__d;
                        iterationsSinceChange=0;
                    }
                    viw.w_a__d += increment*factor;
                }
            }

            
            if(currentParam == 5)
            {
                if(!inRange(viw.w_ab_d))
                {
                    viw.w_ab_d = best_ab_d;
                    currentParam = 6;
                    viw.w_a_cd = min;
                } else
                {
                    if(tempPPL > currentPPL) 
                    {
                        tempPPL = currentPPL;
                        best_ab_d = viw.w_ab_d;
                        iterationsSinceChange=0;
                    }
                    viw.w_ab_d += increment*factor;
                }
            }

            
            if(currentParam == 6)
            {
                if(!inRange(viw.w_a_cd))
                {
                    viw.w_a_cd = best_a_cd;
                    currentParam = 7;
                    viw.w_abcd = min;
                } else
                {
                    if(tempPPL > currentPPL) 
                    {
                        tempPPL = currentPPL;
                        best_a_cd = viw.w_a_cd;
                        iterationsSinceChange=0;
                    }
                    viw.w_a_cd += increment*factor;
                }
            }

            
            if(currentParam == 7)
            {
                if(!inRange(viw.w_abcd))
                {
                    viw.w_abcd = best_abcd;
                    currentParam = 2;
                    viw.w_b_d = min;
                } else
                {
                    if(tempPPL > currentPPL) 
                    {
                        tempPPL = currentPPL;
                        best_abcd = viw.w_abcd;
                        iterationsSinceChange=0;
                    }
                    viw.w_abcd += increment*factor;
                }
            }
        }

        int currentParam = 2;

        double tempPPL = 100000;

        double best_d = 0.1;
        double best_cd = 0.1;
        double best_bcd = 0.1;
        double best_b_d = 0.1;
        double best_abcd = 0.1;
        double best_a_cd = 0.1;
        double best_ab_d = 0.1;
        double best_a__d = 0.1;
};

int main(int argc, char** argv) {

	SLM::ProgramOptions po(argc, argv);
	SLM::LanguageModel lm(po);

        SLM::ValueInterpolationWeights viWeights = po.getValues();
        SLM::ValueInterpolationStrategy* viStrategy = new SLM::ValueInterpolationStrategy(viWeights);
        //SLM::FullBackoffStrategy fbs(lm, po.getTestModelName(), viStrategy);
        SLM::LimitedBackoffStrategy fbs(lm, po.getTestModelName(), viStrategy);
        fbs.setIgnoreCache(true);

        GridParamSearcher gps(viWeights);

        int iterations = 0;
        std::string inputFile = po.getTestInputFiles().front();

        // filePerplexity = pow(2, fileLLH/(fileCount/*-totalOovs*/));

        while(true)//iterations++ < 20)
        {
            L_V << "SLM: Reading " << inputFile << "\n";
            std::ifstream file(inputFile);
            
            gps.updatePPL(fbs.perplexityAndNextFile());
            gps.updateParam();

            std::string retrievedString;
            while(std::getline(file, retrievedString))
            {
                if(po.addSentenceMarkers())
                {
                    retrievedString = "<s> <s> " + retrievedString;
                }
                fbs.nextLine();

                std::vector<std::string> words = whitespaceTokeniser(retrievedString);
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

                        L_P << "SLM: [" << lm.toString(context) << "] " << lm.toString(focus) << "\n";
                        double pr = fbs.prob(context, focus, lm.isOOV(focus));

                    } catch (const UnknownTokenError &e)
                    {
                        std::cerr << "Unknown token error: " << contextStream.str() << " " << words[i] << std::endl;
                        continue;
                    }
                }
            }
        }

	std::cout << "DONE" << std::endl;
	fbs.done();

	return EXIT_SUCCESS;
}
