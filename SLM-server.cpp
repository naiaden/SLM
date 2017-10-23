#include "Logging.h"

#include "ProgramOptions.h"
#include "LanguageModel.h"
#include "BackoffStrategies.h"
#include "BackoffStrategy.h"
#include "NgramBackoffStrategy.h"
#include "UniformInterpolationStrategy.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) 
{
    SLM::ProgramOptions po(argc, argv);
    SLM::LanguageModel lm(po);

    std::cout << "Format: backoffstrategy history1 history2 history3 focus" << std::endl;

    std::map<std::string, SLM::BackoffStrategy*> backoffStrategyCache;

    for (std::string line; std::getline(std::cin, line);) 
    {
        std::vector<std::string> words = whitespaceTokeniser(line);
        std::cout << "backoff: " << words[0] << std::endl;

        SLM::BackoffStrategy* bos = nullptr;

        auto it = backoffStrategyCache.find(words[0]);
        if (it != backoffStrategyCache.end())
        {
            bos = it->second;
        } else 
        {
            if(words[0] == "ngram")
            {
                bos = SLM::BackoffStrategiesFactory::createNgramBackoffStrategy(po, lm);
            } else if(words[0] == "fulluni")
            {
                SLM::InterpolationStrategy* is = new SLM::UniformInterpolationStrategy();
                bos = SLM::BackoffStrategiesFactory::createFullBackoffStrategy(po, lm, is);
            }
            backoffStrategyCache[words[0]] = bos;
        }

        if(bos)
        {
            std::stringstream contextStream;
            contextStream << words[1];
            contextStream << " " << words[2];
            contextStream << " " << words[3];

            Pattern context = lm.toPattern(contextStream.str(), true);
            Pattern focus = lm.toPattern(words[4], true);

            std::cout << (lm.isOOV(focus) ? "!" : "") << "[" << lm.toString(context) << "] " << lm.toString(focus) << "\n";
            std::cout << bos->prob(context, focus, lm.isOOV(focus)) << std::endl;
        } else
        {
            std::cerr << "Invalid backoff strategy (for now,... only ngram and fulluni)\n";
        }

   }

}
