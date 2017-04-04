/*
 * ProgramOptions.cpp
 *
 *  Created on: Dec 10, 2016
 *      Author: louis
 */



#include <unistd.h>
#include <iomanip>

#include <fstream>
#include <iterator>
#include <algorithm>

#include "Logging.h"
#include "ProgramOptions.h"


namespace SLM {

const std::string corpusExtension = "dat";
const std::string patternmodelExtension = "patternmodel";
const std::string vocabularyExtension = "cls";
const std::string languagemodelExtension = "ser";

ProgramOptions::ProgramOptions(int argc, char** argv) {
	L_A << "ProgramOptions\t+ Processing program options\n";

	//

	clp.add<int>("n", 'n', "n", false, 4);

	clp.add<std::string>("traindirectory", 'o', "train output directory", true);
	clp.add<std::string>("trainmodelname", 'm', "train model name", true);

	clp.add<std::string>("traincorpus", '\0', "load colibri encoded corpus", false);
	clp.add<std::string>("trainpatternmodel", '\0', "load colibri encoded pattern model", false);
	clp.add<std::string>("trainvocabulary", '\0', "load colibri class file", false);
	clp.add<std::string>("trainlanguagemodel", '\0', "load SLM language model", false);

	clp.add<std::string>("testmodelname", 'M', "test model name", true);
	clp.add<std::string>("testinputdirectory", 'I', "test input directory", false);
	clp.add<std::string>("testinputfiles", '\0', "file with filename per line", false);
	clp.add<std::string>("testoutputdirectory", 'O', "test output directory", true);

	clp.add<double>("npref", '\0', "npref value", false, 2.0);

	clp.add<std::string>("backoff", 'B', "backoff method", false);
	clp.add<bool>("ignorecache", '\0', "ignore cache", false);
	clp.add<std::string>("debug", '\0', "debug setting", false, "none");
	clp.parse_check(argc, argv);

	//

	trainDirectory = clp.get<std::string>("traindirectory");
	trainModelName = trainDirectory + "/" + clp.get<std::string>("trainmodelname");

	trainCorpus = clp.get<std::string>("traincorpus");
	trainPatternModel = clp.get<std::string>("trainpatternmodel");
	trainVocabulary = clp.get<std::string>("trainvocabulary");
	trainLanguageModel = clp.get<std::string>("trainlanguagemodel");

	testInputDirectory = clp.get<std::string>("testinputdirectory");
	testInputFilesList = clp.get<std::string>("testinputfiles");
	testOutputDirectory = clp.get<std::string>("testoutputdirectory");
	testModelName = testOutputDirectory + "/" + clp.get<std::string>("testmodelname");

	testCorpus = testModelName + "." + corpusExtension;
	testPatternModel = testModelName + "." + patternmodelExtension;
	testVocabulary = testModelName + "." + vocabularyExtension;

	backoffOptions = clp.get<std::string>("backoff");

	npref = clp.get<double>("npref");

	char hostname[128];
	gethostname(hostname, sizeof hostname);
	hostName = std::string(hostname);

	if(!testInputFilesList.empty())
	{
		std::ifstream inputFiles(testInputFilesList);

		std::copy(std::istream_iterator<std::string>(inputFiles),
				std::istream_iterator<std::string>(),
		         back_inserter(testInputFiles));
	}

	testInputFiles.insert(std::end(testInputFiles), std::begin(clp.rest()), std::end(clp.rest()));

	//

	if(trainCorpus.empty()) { trainCorpus = trainModelName + "." + corpusExtension; }
	if(trainPatternModel.empty()) { trainPatternModel = trainModelName + "." + patternmodelExtension; }
	if(trainVocabulary.empty()) { trainVocabulary = trainModelName + "." + vocabularyExtension; }
	if(trainLanguageModel.empty()) { trainLanguageModel = trainModelName + "." + languagemodelExtension; }

	//

	SLM::Logging::getInstance().set(clp.get<std::string>("debug"));

	L_V  	<< std::setw(30) << "Log level set on: " << SLM::Logging::getInstance().toString() << "\n"
			<< std::setw(30) << "Running on machine: " << hostName << "\n"
			<< "\n"
			<< std::setw(30) << "Input model directory: " << trainDirectory << "\n"
			<< std::setw(30) << "Input model name: " << trainModelName << "\n"
			<< "\n"
			<< std::setw(30) << "Input corpus file: " << trainCorpus << "\n"
			<< std::setw(30) << "Input pattern model file: " << trainPatternModel << "\n"
			<< std::setw(30) << "Input vocabulary file: " << trainVocabulary << "\n"
			<< std::setw(30) << "Input language model file: " << trainLanguageModel << "\n"
			<< "\n"
			<< std::setw(30) << "Output model directory: " << testOutputDirectory << "\n"
			<< std::setw(30) << "Output model name: " << testModelName << "\n"
			<< "\n"
			<< std::setw(30) << "Output corpus file: " << testCorpus << "\n"
			<< std::setw(30) << "Output pattern model file: " << testPatternModel << "\n"
			<< std::setw(30) << "Output vocabulary file: " << testVocabulary << "\n"
			<< "\n"
			<< std::setw(30) << "Number of test input files: " << testInputFiles.size() << "\n"
			<< "\n"
			<< std::setw(30) << "Backoff strategies: " << backoffOptions << "\n"
			<< std::setw(30) << "Npref ratio: " << npref << "\n";

	//

	L_A << "ProgramOptions\t- Processing program options\n";
}

ProgramOptions::~ProgramOptions() {
	// TODO Auto-generated destructor stub
}

std::string ProgramOptions::getTrainDirectory() const
{
	return trainDirectory;
}

std::string ProgramOptions::getTrainModelName() const
{
	return trainModelName;
}

std::string ProgramOptions::getTrainCorpus() const
{
	return trainCorpus;
}
std::string ProgramOptions::getTrainPatternModel() const
{
	return trainPatternModel;
}
std::string ProgramOptions::getTrainVocabulary() const
{
	return trainVocabulary;
}
std::string ProgramOptions::getTrainLanguageModel() const
{
	return trainLanguageModel;
}

std::string ProgramOptions::getTestModelName() const
{
	return testModelName;
}
std::string ProgramOptions::getTestInputDirectory() const
{
	return testInputDirectory;
}
std::string ProgramOptions::getTestOutputDirectory() const
{
	return testOutputDirectory;
}

std::string ProgramOptions::getTestCorpus() const
{
	return testCorpus;
}
std::string ProgramOptions::getTestPatternModel() const
{
	return testPatternModel;
}
std::string ProgramOptions::getTestVocabulary() const
{
	return testVocabulary;
}

std::vector<std::string> ProgramOptions::getTestInputFiles() const
{
	return testInputFiles;
}

int ProgramOptions::getOrder() const
{
	return order;
}

bool ProgramOptions::isIgnoreCache() const
{
	return ignoreCache;
}

std::string ProgramOptions::getHostName() const
{
	return hostName;
}

std::string ProgramOptions::getBackoffOptions() const
{
	return backoffOptions;
}

double ProgramOptions::getNpref() const
{
	return npref;
}

} /* namespace SLM */
