/*
 * ProgramOptions.h
 *
 *  Created on: Dec 10, 2016
 *      Author: louis
 */

#ifndef PROGRAMOPTIONS_H_
#define PROGRAMOPTIONS_H_

#include <string>
#include <vector>

#include "cmdline.h"


namespace SLM {

class ProgramOptions {

public:
	ProgramOptions(int argc, char** argv);
	virtual ~ProgramOptions();

	std::string getTrainDirectory() const;
	std::string getTrainModelName() const;

	std::string getTrainCorpus() const;
	std::string getTrainPatternModel() const;
	std::string getTrainVocabulary() const;
	std::string getTrainLanguageModel() const;

	std::string getTestModelName() const;
	std::string getTestInputDirectory() const;
	std::string getTestOutputDirectory() const;

	std::string getTestCorpus() const;
	std::string getTestPatternModel() const;
	std::string getTestVocabulary() const;

	std::vector<std::string> getTestInputFiles() const;

	int getOrder() const;

	std::string getHostName() const;
private:
	cmdline::parser clp;

	std::string trainDirectory;
	std::string trainModelName;

	std::string trainCorpus;
	std::string trainPatternModel;
	std::string trainVocabulary;
	std::string trainLanguageModel;


	std::string testModelName;
	std::string testInputDirectory;
	std::string testOutputDirectory;

	std::string testCorpus;
	std::string testPatternModel;
	std::string testVocabulary;

	std::vector<std::string> testInputFiles;

	int order = 0;
	// backoff

	std::string hostName;
};

} /* namespace SLM */

#endif /* PROGRAMOPTIONS_H_ */
