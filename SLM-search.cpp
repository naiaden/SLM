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
#include "FullBackoffStrategy.h"
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

        double updatePPL(double ppl)
        {
            if(ppl > 0) 
            { previousPPL = currentPPL;
              currentPPL = ppl;

              lastIterWasDecrease = (currentPPL < previousPPL);
            }
            std::cout << "PPL ---> " << currentPPL << std::endl;
            return previousPPL;
        }

        bool lastIterWasDecrease = false;
        SLM::ValueInterpolationWeights& viw;

        virtual int updateParam() = 0;
};  

class GridParamSearcher : public ParamSearcher
{
    public:
        GridParamSearcher(SLM::ValueInterpolationWeights& viw) : ParamSearcher(viw) {}
        virtual ~GridParamSearcher() {} 

        int updateParam()
        {
            if(currentParam == 0)
                viw.w_d += 0.1;
        }

        int currentParam = 0;

        double w_d = 0.1;
        double w_cd = 0.1;
        double w_bcd = 0.1;
        double w_b_d = 0.1;
        double w_abcd = 0.1;
        double w_a_cd = 0.1;
        double w_ab_d = 0.1;
        double w_a__d = 0.1;
};

int main(int argc, char** argv) {

	SLM::ProgramOptions po(argc, argv);
	SLM::LanguageModel lm(po);

        SLM::ValueInterpolationWeights viWeights;
        SLM::ValueInterpolationStrategy* viStrategy = new SLM::ValueInterpolationStrategy(viWeights);
        SLM::FullBackoffStrategy fbs(lm, po.getTestModelName(), viStrategy);

        GridParamSearcher gps(viWeights);

        int iterations = 0;
        std::string inputFile = po.getTestInputFiles().front();

        // filePerplexity = pow(2, fileLLH/(fileCount/*-totalOovs*/));

        while(iterations++ < 7)
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
