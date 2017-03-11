/*
 * ToolProgramOptions.h
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#ifndef TOOLS_TOOLPROGRAMOPTIONS_H_
#define TOOLS_TOOLPROGRAMOPTIONS_H_

#include "cmdline.h"

namespace SLM {

class ToolProgramOptions {
public:
	ToolProgramOptions(int argc, char** argv);
	virtual ~ToolProgramOptions();

	std::string getInputPath() const;
	std::string getOutputPath() const;

protected:
	cmdline::parser clp;

	std::string inputPath;
	std::string outputPath;

	std::string hostName;
};

} /* namespace SLM */

#endif /* TOOLS_TOOLPROGRAMOPTIONS_H_ */
