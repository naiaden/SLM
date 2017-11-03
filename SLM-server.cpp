#include "Logging.h"

#include "ProgramOptions.h"
#include "LanguageModel.h"
#include "BackoffStrategies.h"
#include "BackoffStrategy.h"
#include "NgramBackoffStrategy.h"
#include "UniformInterpolationStrategy.h"

#include <iostream>
#include <fstream>
#include <string>

#include <signal.h>
#include "Utils.h"

void sigintHandler(int sigNum)
{
    signal(SIGINT, sigintHandler);
    std::cerr << "(Use exit to terminate the program)" << std::endl;
}

enum class ProgramMode { FILE, SENTENCE, PATTERN};
enum class InputMode { ALL, EACH };

class ProgramState
{
    public:
        ProgramState(SLM::ProgramOptions& po, SLM::LanguageModel& lm, ProgramMode pm, InputMode im) : po(po), lm(lm), pm(pm), im(im)
        {

        }

        virtual ~ProgramState()
        {
            unsetProbsOutputFile();
        }   

        void unsetProbsOutputFile()
        {
            if(probsWriter.is_open())
            {
                probsWriter.flush();
                probsWriter.close();
            }
        }


        void setProbsOutputFile(const std::string& outputFile)
        {
            unsetProbsOutputFile();

            L_A << "sPOF" << std::endl;

            probsOutputFile = outputFile;
            probsWriter.open(probsOutputFile, std::fstream::out | std::fstream::app);
        }

        void writeToProbsFile(const std::string& output)
        {
            if(probsWriter.is_open())
            {
                L_A << "wTPF: " << output << std::endl;
                probsWriter << output;
            }
        }

        SLM::ProgramOptions po;
        SLM::LanguageModel lm;
        ProgramMode pm;
        InputMode im;
        bool ignoreInput;
        std::map<std::string, SLM::BackoffStrategy*> backoffStrategyCache;
        SLM::BackoffStrategy* bos = nullptr;
        
        std::ofstream probsWriter;
        std::string probsOutputFile;
};

class InputAction
{
    public:
    InputAction(ProgramState& ps) : ps(ps)
    {

    }

        ProgramState& ps;
    virtual void act() = 0;
};

class ExitInputAction : public InputAction
{
    public:
        ExitInputAction(ProgramState& ps) : InputAction(ps)
        {

        }

        void act()
        {
            L_V << "ExitInputAction" << std::endl;
            ps.unsetProbsOutputFile();

            exit(0);
        }
};

class SetProbsFileInputAction : public InputAction
{
    public:
        SetProbsFileInputAction(ProgramState& ps, std::string& outputFileName) : InputAction(ps), outputFileName(outputFileName)
        {
        }

        std::string outputFileName;
        void act()
        {
            if(outputFileName == "-")
                ps.unsetProbsOutputFile();
            else
                ps.setProbsOutputFile(outputFileName);
        }
};  

bool setBackoffStrategy(ProgramState& ps, const std::string& strat)
{
    auto it = ps.backoffStrategyCache.find(strat);
    if (it != ps.backoffStrategyCache.end())
    {
        ps.bos = it->second;
    } else
    {
        if(strat == "ngram")
        {
            ps.bos = SLM::BackoffStrategiesFactory::createNgramBackoffStrategy(ps.po, ps.lm);
            ps.backoffStrategyCache["ngram"] = ps.bos;
        } else if(strat == "fulluni")
        {
            SLM::InterpolationStrategy* is = new SLM::UniformInterpolationStrategy();
            ps.bos = SLM::BackoffStrategiesFactory::createFullBackoffStrategy(ps.po, ps.lm, is);
            ps.backoffStrategyCache["fulluni"] = ps.bos;
        } else
        {
            std::cerr << "Unknown backoff strategy\n";
            return false;
        }
    }
    return true;
}

class InputModeInputAction : public InputAction
{
    public:
        InputModeInputAction(ProgramState& ps, std::string& spec, std::string& mode) : InputAction(ps), mode(mode), spec(spec)
        {

        }

        std::string mode;
        std::string spec;

        void act()
        {
            if(spec == "all")
            {
                if(setBackoffStrategy(ps, mode))
                {
                    ps.im = InputMode::ALL;
                    std::cerr << "Input mode changed to all with mode " << mode << "\n";
                } else
                {
                    std::cerr << "Cannot use this backoff strategy\n";
                }
            } else if(spec == "each")
            {
                ps.im = InputMode::EACH;
                std::cerr << "Input mode changed to each\n";
            } else
            {
                std::cerr << "Unknown input mode (all, each)\n";
            }
            
        }
};

class SetModeInputAction : public InputAction
{
    public:
        SetModeInputAction(ProgramState& ps, std::string& mode) : InputAction(ps), mode(mode)
        {
        }

    std::string mode;

    void act()
    {
        if(mode == "sentence")
        {
            ps.pm = ProgramMode::SENTENCE;
            std::cerr << "Program mode changed to sentence\n";
        } else if(mode == "pattern")
        {
            ps.pm = ProgramMode::PATTERN;
            std::cerr << "Program mode changed to pattern\n";
        } else if(mode == "file")
        {
            ps.pm = ProgramMode::FILE;
            std::cerr << "Program mode changed to file\n";
        } else
        {
            std::cerr << "Unknown program mode (pattern, sentence, file)\n";
        }

    }
};

class ComputerProbsInputAction : public InputAction
{
    public:
        ComputerProbsInputAction(ProgramState& ps, const std::vector<std::string>& words) : InputAction(ps), words(words)
        {}

    std::vector<std::string> words;
    void act()
    {
        if(ps.im == InputMode::ALL)
        {
                std::stringstream contextStream;
                contextStream << words[0];
                contextStream << " " << words[1];
                contextStream << " " << words[2];

                Pattern context = ps.lm.toPattern(contextStream.str(), true);
                Pattern focus = ps.lm.toPattern(words[3], true);
               
                std::stringstream outputStream;
                outputStream << (ps.lm.isOOV(focus) ? "!" : "") << "[" << ps.lm.toString(context) << "] " << ps.lm.toString(focus) << "\t";
                outputStream << ps.bos->prob(context, focus, false) << "\t";
                outputStream << ps.bos->name() << "\n";

                std::cout << outputStream.str();
                ps.writeToProbsFile(outputStream.str());
            
        } else
        {
            if(setBackoffStrategy(ps, words[0]))
            {
                // backoff strategy is determined
                // leeeeeet's go

                std::stringstream contextStream;
                contextStream << words[1];
                contextStream << " " << words[2];
                contextStream << " " << words[3];

                Pattern context = ps.lm.toPattern(contextStream.str(), true);
                Pattern focus = ps.lm.toPattern(words[4], true);
               
                std::stringstream outputStream;
                outputStream << (ps.lm.isOOV(focus) ? "!" : "") << "[" << ps.lm.toString(context) << "] " << ps.lm.toString(focus) << "\t";
                outputStream << ps.bos->prob(context, focus, false) << "\t";
                outputStream << ps.bos->name() << "\n";

                std::cout << outputStream.str();
                ps.writeToProbsFile(outputStream.str());
            }
        }
    }

    private:
};

class InvalidInputAction : public InputAction
{
    public:
    InvalidInputAction(ProgramState&) : InputAction(ps)
    {
    }

    void act()
    {
        L_V << "InvalidInputAction" << std::endl;
    }
};

std::shared_ptr<InputAction> handleInput(ProgramState& ps, std::string& input)
{
    if(input.empty())
        return std::make_shared<InvalidInputAction>(ps);

    std::vector<std::string> words = whitespaceTokeniser(input);
    if(words.size() == 0)
        return std::make_shared<InvalidInputAction>(ps);

    if(words[0] == "exit")
        return std::make_shared<ExitInputAction>(ps);

    if(words[0] == "setmode" && words.size() > 1)
        return std::make_shared<SetModeInputAction>(ps, words[1]);

    if(words[0] == "inputmode" && words.size() > 1)
        return std::make_shared<InputModeInputAction>(ps, words[1], (words.size()>2? words[2] : words[1])); // words[1] in the latter is jst a dummy

    if(words[0] == "probsfile" && words.size() > 1)
        return std::make_shared<SetProbsFileInputAction>(ps, words[1]);

    if(words.size() >= 4)
        return std::make_shared<ComputerProbsInputAction>(ps, words);

    return std::make_shared<InvalidInputAction>(ps);

}

int main(int argc, char** argv) 
{

    SLM::ProgramOptions po(argc, argv);
    SLM::LanguageModel lm(po);

    ProgramState ps(po, lm, ProgramMode::PATTERN, InputMode::EACH);

    std::cout << "W=" << lm.getVocabularySize() << std::endl;
    signal(SIGINT, sigintHandler);


    for (std::string line; std::getline(std::cin, line);) 
    {
        handleInput(ps, line)->act();



/*        if(ps.bos && !ignoreInput)
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
*/
   }

}
