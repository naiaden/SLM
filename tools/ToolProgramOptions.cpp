/*
 * ToolProgramOptions.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#include "ToolProgramOptions.h"

#include <unistd.h>
#include <iomanip>

#include "Logging.h"

namespace SLM {

ToolProgramOptions::ToolProgramOptions(int argc, char** argv) {
	L_A << "ProgramOptions\t+ Processing program options\n";

	clp.add<std::string>("inputpath", 'i', "directory with nbest lists", true);
	clp.add<std::string>("outputpath", 'o', "directory to write results", true);
	clp.add<std::string>("referencepath", 'r', "directory with reference files", true);
	clp.add<std::string>("debug", '\0', "debug setting", false, "none");
	clp.parse_check(argc, argv);

	inputPath = clp.get<std::string>("inputpath");
	outputPath = clp.get<std::string>("outputpath");
	referencePath = clp.get<std::string>("referencepath");

	SLM::Logging::getInstance().set(clp.get<std::string>("debug"));


	char hostname[128];
	gethostname(hostname, sizeof hostname);
	hostName = std::string(hostname);


	L_V  	<< std::setw(30) << "Log level set on: " << SLM::Logging::getInstance().toString() << "\n"
			<< std::setw(30) << "Running on machine: " << hostName << "\n"
			<< "\n"
			<< std::setw(30) << "Input path: " << inputPath << "\n"
			<< std::setw(30) << "Output path: " << outputPath << "\n"
			<< std::setw(30) << "Reference path: " << referencePath << "\n";

	L_A << "ProgramOptions\t- Processing program options\n";
}

std::string ToolProgramOptions::getInputPath() const
{
	return inputPath;
}

std::string ToolProgramOptions::getOutputPath() const
{
	return outputPath;
}

std::string ToolProgramOptions::getReferencePath() const
{
	return referencePath;
}

ToolProgramOptions::~ToolProgramOptions() {
	// TODO Auto-generated destructor stub
}



} /* namespace SLM */
