/*
 * TrainProgramOptions.cpp
 *
 *  Created on: Mar 28, 2017
 *      Author: louis
 */

#include "TrainProgramOptions.h"

#include <unistd.h>
#include <iomanip>



#include "Logging.h"
#include "Utils.h"

namespace SLM {

const std::string corpusExtension = "dat";
const std::string patternmodelExtension = "patternmodel";
const std::string vocabularyExtension = "cls";
const std::string languagemodelExtension = "ser";

TrainProgramOptions::TrainProgramOptions(int argc, char** argv) {
	L_A << "TrainProgramOptions\t+ Processing program options\n";

	clp.add<std::string>("modelname", 'm', "train model name", true);

	clp.add<std::string>("trainoutput", 'o', "train output directory", true);

	clp.add<std::string>("loadtraincorpus", '\0', "load colibri encoded corpus", false);
	clp.add<std::string>("loadtrainpatternmodel", '\0', "load colibri encoded pattern model", false);
	clp.add<std::string>("loadtrainvocabulary", '\0', "load colibri class file", false);
	clp.add<std::string>("loadtrainserialisedmodel", '\0', "load SLM language model", false);

	clp.add<std::string>("debug", '\0', "debug setting", false, "none");

	clp.add<int>("samples", 's', "samples", false, 50);
	clp.add<int>("burnin", 'b', "burnin", false, 0);
	clp.add("skipgram", 'S', "train with skipgrams");

	clp.add<int>("sthreshold", 'T', "threshold for skipgrams", false, 1);
	clp.add<int>("threshold", 't', "threshold for ngrams", false, 1);
	clp.add<int>("unigramthreshold", 'W', "unigram threshold", false, 1);
	clp.add<int>("prunedonsubsumed", 'p', "prune all n-grams that are not subsumed by higher order n-grams", false, 0);

	clp.add<std::string>("extendmodel", 'E', "extend current model (with larger n or skips)", false, "");
	clp.parse_check(argc, argv);


	outputPath = clp.get<std::string>("trainoutput");
	modelName = clp.get<std::string>("modelname");

	trainCorpusFile = clp.get<std::string>("loadtraincorpus");
	trainPatternModelFile = clp.get<std::string>("loadtrainpatternmodel");
	trainVocabularyFile = clp.get<std::string>("loadtrainvocabulary");
	trainSerialisedFile = clp.get<std::string>("loadtrainserialisedmodel");

	extendModel = clp.get<std::string>("extendmodel");

	samples = clp.get<int>("samples");
	burnin = clp.get<int>("burnin");
	useSkipgrams = clp.exist("skipgram");

	skipgramThreshold = clp.get<int>("sthreshold");
	ngramThreshold = clp.get<int>("threshold");
	unigramThreshold = clp.get<int>("unigramthreshold");
	pruneLevel = clp.get<int>("prunedonsubsumed");

	inputFiles.insert(std::end(inputFiles), std::begin(clp.rest()), std::end(clp.rest()));

	SLM::Logging::getInstance().set(clp.get<std::string>("debug"));

//	if(trainCorpusFile.empty()) { trainCorpusFile = modelName + "." + corpusExtension; }
//	if(trainPatternModelFile.empty()) { trainPatternModelFile = modelName + "." + patternmodelExtension; }
//	if(trainVocabularyFile.empty()) { trainVocabularyFile = modelName + "." + vocabularyExtension; }
//	if(trainSerialisedFile.empty()) { trainSerialisedFile = modelName + "." + languagemodelExtension; }

	char hostname[128];
	gethostname(hostname, sizeof hostname);
	hostName = std::string(hostname);

	L_V 	<< std::setw(30) << "Log level set on: " << SLM::Logging::getInstance().toString() << "\n"
			<< std::setw(30) << "Running on machine: " << hostName << "\n"
			<< "\n"
			<< std::setw(30) << "Writing model to: " << outputPath << "\n"
			<< std::setw(30) << "With model name: " << modelName << "\n"
			<< std::setw(30) << "Extending the model: " << (extendModel.empty() ? "no" : extendModel) << "\n"
			<< "\n"
			<< std::setw(30) << "Corpus file: " << trainCorpusFile << "\n"
			<< std::setw(30) << "Pattern model file: " << trainPatternModelFile << "\n"
			<< std::setw(30) << "Vocabulary file: " << trainVocabularyFile << "\n"
			<< std::setw(30) << "Serialised file: " << trainSerialisedFile << "\n"
			<< "\n"
			<< std::setw(30) << "Samples: " << samples << " (of which " << burnin << " burnin)\n"
			<< std::setw(30) << "Using skipgrams: " << (useSkipgrams ? "yes" : "no") << "\n"
			<< std::setw(30) << "Unigram threshold: " << ngramThreshold << "\n"
			<< std::setw(30) << "ngram threshold: " << ngramThreshold << "\n"
			<< std::setw(30) << "Skipgram threshold: " << skipgramThreshold << "\n"
			<< std::setw(30) << "Prune threshold: " << pruneLevel << "\n";

	L_A << "TrainProgramOptions\t- Processing program options\n";
}

TrainProgramOptions::~TrainProgramOptions() {
	// TODO Auto-generated destructor stub
}

std::string TrainProgramOptions::getModelName() const
{
	return modelName;
}

std::vector<std::string> TrainProgramOptions::getInputFiles() const
{
	return inputFiles;
}

std::string TrainProgramOptions::getTrainVocabularyFile() const
{
	return trainVocabularyFile;
}

std::string TrainProgramOptions::constructTrainVocabularyFile() const
{
	return outputPath + "/" + modelName + "." + vocabularyExtension;
}

void TrainProgramOptions::setTrainVocabularyFile(const std::string& fileName)
{
	trainVocabularyFile = fileName;
}

std::string TrainProgramOptions::getTrainCorpusFile() const
{
	return trainCorpusFile;
}

std::string TrainProgramOptions::constructTrainCorpusFile() const
{
	return outputPath + "/" + modelName + "." + corpusExtension;
}

void TrainProgramOptions::setTrainCorpusFile(const std::string& fileName)
{
	trainCorpusFile = fileName;
}

std::string TrainProgramOptions::getPatternModelFile() const
{
	return trainPatternModelFile;
}
std::string TrainProgramOptions::constructPatternModelFile() const
{
	return outputPath + "/" + modelName + "." + patternmodelExtension;
}
void TrainProgramOptions::setPatternModelFile(const std::string& fileName)
{
	trainPatternModelFile = fileName;
}

std::string TrainProgramOptions::getSerialisedFile() const
{
	return trainSerialisedFile;
}
std::string TrainProgramOptions::constructSerialisedFile() const
{
	return outputPath + "/" + modelName + "." + languagemodelExtension;
}
void TrainProgramOptions::setSerialisedFile(const std::string& fileName)
{
	trainSerialisedFile = fileName;
}

std::string TrainProgramOptions::getOutputPath() const
{
	return outputPath;
}

bool TrainProgramOptions::doSkipgrams() const
{
	return useSkipgrams;
}
std::string TrainProgramOptions::getExtendModel() const
{
	return extendModel;
}

int TrainProgramOptions::getUnigramThreshold() const
{
	return unigramThreshold;
}
int TrainProgramOptions::getNgramThreshold() const
{
	return ngramThreshold;
}
int TrainProgramOptions::getSkipgramThreshold() const
{
	return skipgramThreshold;
}
int TrainProgramOptions::getPruneThreshold() const
{
	return pruneLevel;
}

int TrainProgramOptions::getSamples() const
{
	return samples;
}
int TrainProgramOptions::getBurnin() const
{
	return burnin;
}

} /* namespace SLM */
