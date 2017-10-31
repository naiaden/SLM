#include "Logging.h"

#include "ProgramOptions.h"
#include "LanguageModel.h"
#include "BackoffStrategies.h"
#include "BackoffStrategy.h"
#include "NgramBackoffStrategy.h"
#include "UniformInterpolationStrategy.h"

#include <iostream>
#include <string>

#include <signal.h>

void sigintHandler(int sigNum)
{
    signal(SIGINT, sigintHandler);
    std::cerr << "(Use exit to terminate the program)" << std::endl;
}

enum class ProgramMode { SENTENCE, PATTERN };

int main(int argc, char** argv) 
{
    signal(SIGINT, sigintHandler);

    SLM::ProgramOptions po(argc, argv);
    SLM::LanguageModel lm(po);

    std::cout << "W=" << lm.getVocabularySize() << std::endl;

    ProgramMode pm = ProgramMode::SENTENCE;
    bool ignoreInput = false;

    //std::cout << "Format: backoffstrategy history1 history2 history3 focus" << std::endl;

    std::map<std::string, SLM::BackoffStrategy*> backoffStrategyCache;

    for (std::string line; std::getline(std::cin, line);) 
    {
        std::vector<std::string> words = whitespaceTokeniser(line);
        std::cout << "backoff: " << words[0] << std::endl;

        SLM::BackoffStrategy* bos = nullptr;

        if(words[0] == "exit") exit(0);
        auto it = backoffStrategyCache.find(words[0]);
        if (it != backoffStrategyCache.end())
        {
            bos = it->second;
        } else if(words[0] == "setmode")
        {   
            if(words[1] == "sentence") { pm = ProgramMode::SENTENCE; std::cerr << "Mode set to sentence\n"; }
            else if(words[1] == "pattern") { pm = ProgramMode::PATTERN; std::cerr << "Mode set to pattern\n"; }
            else std::cerr << "Unknown setmode: try sentence or pattern" << std::endl;
            ignoreInput = true;
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

        if(bos && !ignoreInput)
        {
            if(pm == ProgramMode::PATTERN)
            {
                std::stringstream contextStream;
                contextStream << words[1];
                contextStream << " " << words[2];
                contextStream << " " << words[3];

                Pattern context = lm.toPattern(contextStream.str(), true);
                Pattern focus = lm.toPattern(words[4], true);

                std::cout << (lm.isOOV(focus) ? "!" : "") << "[" << lm.toString(context) << "] " << lm.toString(focus) << "\n";
                std::cout << bos->prob(context, focus, false) << std::endl;
            } else
            {
                for(int i = 4; i < words.size(); ++i)
                {
                    std::stringstream contextStream;
                    //contextStream << words[i-(4-1)];

                    for(int ii = 1; ii < 4; ++ii)
                    {
                        //std::cout << "." << words[i-4+ii] << std::endl;
                        contextStream << " " << words[i-4+ii];
                    }

                    Pattern context = lm.toPattern(contextStream.str(), true);
                    Pattern focus = lm.toPattern(words[i], true);
                
                    //std::cout << (lm.isOOV(focus) ? "!" : "") << lm.toString(focus) << std::endl;

                    std::cout << (lm.isOOV(focus) ? "!" : "") << "[" << lm.toString(context) << "] " << lm.toString(focus) << "\n";
                    std::cout << bos->prob(context, focus, false) << std::endl;
                }
            }
        } else
        {
            std::cerr << "Invalid backoff strategy (for now,... only ngram and fulluni)\n";
        }

        ignoreInput = false;

   }

}
