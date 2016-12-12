/*
 * LanguageModel.h
 *
 *  Created on: Dec 10, 2016
 *      Author: louis
 */

#ifndef LANGUAGEMODEL_H_
#define LANGUAGEMODEL_H_

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include "ProgramOptions.h"

namespace SLM {

class LanguageModel {
public:
	LanguageModel(const ProgramOptions& programOptions);
	virtual ~LanguageModel();

	Pattern toPattern(const std::string& patternString);
	std::string toString(const Pattern& pattern);

	const std::vector<Pattern>& getVocabulary() const;

	double getProb(const Pattern& focus, const Pattern& context);
private:
	void initialise(const ProgramOptions& programOptions);
	void defaultPatternModelOptions();
	void extendClassEncoder(const std::vector<std::string>& inputFiles, const std::string& outputFile);
	void loadLanguageModel(const std::string& inputFile);

	ClassEncoder classEncoder;
	ClassDecoder classDecoder;
	IndexedCorpus* indexedCorpus;
	PatternModel<uint32_t> patternModel;
	PatternModelOptions patternModelOptions;

	std::vector<Pattern> vocabulary;
//	cpyp::PYLPM<4> lm;
};

} /* namespace SLM */

#endif /* LANGUAGEMODEL_H_ */
