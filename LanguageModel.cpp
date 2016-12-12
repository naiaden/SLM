/*
 * LanguageModel.cpp
 *
 *  Created on: Dec 10, 2016
 *      Author: louis
 */

#include "LanguageModel.h"

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <boost/archive/binary_iarchive.hpp>

#include "Logging.h"



namespace SLM {

LanguageModel::LanguageModel(const ProgramOptions& programOptions) {
	initialise(programOptions);

}

LanguageModel::~LanguageModel() {
//	delete indexedCorpus;
}

const std::vector<Pattern>& LanguageModel::getVocabulary() const
{
	return vocabulary;
}

Pattern LanguageModel::toPattern(const std::string& patternString)
{
	return classEncoder.buildpattern(patternString);
}

std::string LanguageModel::toString(const Pattern& pattern)
{
	return pattern.tostring(classDecoder);
}

void LanguageModel::initialise(const ProgramOptions& programOptions)
{
	L_I << "LanguageModel: Intialisation\n";

	L_V << "LanguageModel: Loading language model options\n";
	defaultPatternModelOptions();

	L_V << "LanguageModel: Loading vocabulary\n";
	classEncoder.load(programOptions.getTrainVocabulary());

	L_V << "LanguageModel: Loading class decoder\n";
	classDecoder.load(programOptions.getTrainVocabulary());

	L_V << "LanguageModel: Loading indexed corpus\n";
	indexedCorpus = new IndexedCorpus(programOptions.getTrainCorpus());

	L_V << "LanguageModel: Training pattern model\n";
	patternModel = PatternModel<uint32_t>(programOptions.getTrainPatternModel(), patternModelOptions, nullptr, indexedCorpus);

	L_V << "LanguageModel: Extending the class encoder\n";
	extendClassEncoder(programOptions.getTestInputFiles(), programOptions.getTestCorpus());

	L_V << "LanguageModel: Loading language model\n";
	loadLanguageModel(programOptions.getTrainLanguageModel());

	L_V << "LanguageModel: Initialise vocabulary\n";
	PatternSet<uint64_t> vocabularySet = patternModel.extractset(1,1);
	vocabulary = std::vector<Pattern>(vocabularySet.begin(), vocabularySet.end());
	L_V << "LanguageModel: Vocabulary contains " << vocabulary.size() << " items\n";
}

void LanguageModel::loadLanguageModel(const std::string& inputFile)
{
	std::ifstream ifs(inputFile, std::ios::binary);
	if(!ifs.good())
	{
		std::cerr << "Something went wrong with loading the language model: " << inputFile << std::endl;
	}
	boost::archive::binary_iarchive ia(ifs);
}

void LanguageModel::defaultPatternModelOptions()
{
	patternModelOptions.MAXLENGTH = 4;
	patternModelOptions.MINLENGTH = 1;
	patternModelOptions.DOSKIPGRAMS_EXHAUSTIVE = true;
	patternModelOptions.DOREVERSEINDEX = true;
	patternModelOptions.QUIET = false;
	patternModelOptions.MINTOKENS = 1;
}

void LanguageModel::extendClassEncoder(const std::vector<std::string>& inputFiles, const std::string& outputFile)
{
	for(auto& i: inputFiles)
	{
		classEncoder.encodefile(i, outputFile, 1, 1, 0, 1);
	}

	classEncoder.save(outputFile);
	classDecoder.load(outputFile);
}

} /* namespace SLM */
