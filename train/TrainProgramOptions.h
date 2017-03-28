/*
 * TrainProgramOptions.h
 *
 *  Created on: Mar 28, 2017
 *      Author: louis
 */

#ifndef TRAIN_TRAINPROGRAMOPTIONS_H_
#define TRAIN_TRAINPROGRAMOPTIONS_H_

#include "cmdline.h"

#include <string>
#include <vector>

namespace SLM {

class TrainProgramOptions {
public:
	TrainProgramOptions(int argc, char** argv);
	virtual ~TrainProgramOptions();

	std::string getModelName() const;

	std::vector<std::string> getInputFiles() const;

	std::string getOutputPath() const;

	bool doSkipgrams() const;
	bool doExtend() const;

	int getUnigramThreshold() const;
	int getNgramThreshold() const;
	int getSkipgramThreshold() const;
	int getPruneThreshold() const;

	int getSamples() const;
	int getBurnin() const;

	std::string getTrainVocabularyFile() const;
	std::string constructTrainVocabularyFile() const;
	void setTrainVocabularyFile(const std::string& fileName);

	std::string getTrainCorpusFile() const;
	std::string constructTrainCorpusFile() const;
	void setTrainCorpusFile(const std::string& fileName);

	std::string getPatternModelFile() const;
	std::string constructPatternModelFile() const;
	void setPatternModelFile(const std::string& fileName);

protected:
	cmdline::parser clp;

	std::string hostName;
	std::string modelName;

	std::vector<std::string> inputFiles;

	std::string outputPath;

	bool useSkipgrams = false;
	bool extendModel = false;

	int unigramThreshold = 1;
	int ngramThreshold = 1;
	int skipgramThreshold = 1;
	int pruneLevel = 1;

	int samples = 50;
	int burnin = 0;

	std::string trainVocabularyFile;
	std::string trainCorpusFile;
	std::string trainPatternModelFile;
	std::string trainSerialisedFile;
};

} /* namespace SLM */

#endif /* TRAIN_TRAINPROGRAMOPTIONS_H_ */
