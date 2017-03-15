/*
 * ToolProgramOptions.h
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#ifndef TOOLS_TOOLPROGRAMOPTIONS_H_
#define TOOLS_TOOLPROGRAMOPTIONS_H_

#include "cmdline.h"

#include "Sorter.h"

namespace SLM {

enum ProgramMode { GRIDSEARCH, SELECTBEST, CEILING };
enum WeightMode { ACOUSTIC, LANGUAGE, WEIGHTED, POWERWEIGHTED };

class ToolProgramOptions {
public:
	ToolProgramOptions(int argc, char** argv);
	virtual ~ToolProgramOptions();

	std::string getInputPath() const;
	std::string getOutputPath() const;
	std::string getReferencePath() const;

	bool isProgramMode(ProgramMode mode) const;
	ProgramMode getProgramMode() const;

	bool isWeightMode(WeightMode mode) const;
	WeightMode getWeightMode() const;

	double getAcousticWeight() const;
	double getLanguageModelWeight() const;

	bool isReverse() const;

	std::vector<std::string> getLimitedReferenceIds() const;

	SLM::Sorter* getSorter();
protected:
	cmdline::parser clp;

	std::string inputPath;
	std::string outputPath;
	std::string referencePath;

	std::string hostName;

	double aW = 1.0;
	double lW = 1.0;

	ProgramMode programMode = ProgramMode::SELECTBEST;
	WeightMode weightMode = WeightMode::WEIGHTED;

	std::vector<std::string> limitedReferenceIds;
	bool reverse;
};

} /* namespace SLM */

#endif /* TOOLS_TOOLPROGRAMOPTIONS_H_ */
