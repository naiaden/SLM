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
#include "Utils.h"

namespace SLM {

ProgramMode programFromString(const std::string& s)
{
	if(s == "ceiling")
		return ProgramMode::CEILING;
	if(s == "search")
		return ProgramMode::GRIDSEARCH;

	return ProgramMode::SELECTBEST;
}

std::string toString(ProgramMode mode)
{
	switch (mode) {
		case ProgramMode::CEILING:
			return "ceiling";
		case ProgramMode::GRIDSEARCH:
			return "search";
		default:
			return "best";
	}
}

WeightMode weightFromString(const std::string& s)
{
	if(s == "acoustic")
		return WeightMode::ACOUSTIC;
	if(s == "language")
		return WeightMode::LANGUAGE;
	if(s == "powerweighted")
		return WeightMode::POWERWEIGHTED;

	return WeightMode::WEIGHTED;
}

std::string toString(WeightMode mode)
{
	switch (mode) {
		case WeightMode::ACOUSTIC:
			return "acoustic";
		case WeightMode::LANGUAGE:
			return "language";
		case WeightMode::POWERWEIGHTED:
			return "powerweighted";
		default:
			return "weighted";
	}
}

ToolProgramOptions::ToolProgramOptions(int argc, char** argv) {
	L_A << "ProgramOptions\t+ Processing program options\n";

	clp.add<std::string>("inputpath", 'i', "directory with nbest lists", true);
	clp.add<std::string>("outputpath", 'o', "directory to write results", true);
	clp.add<std::string>("referencepath", 'r', "directory with reference files", true);

	clp.add<std::string>("mode", 'm', "program mode: ceiling, best, search", false);
	clp.add<std::string>("weighting", 'w', "weighting mode: acoustic, language, (power,)weighted (-a -l)", false);
	clp.add("reverse", '\0', "reverse the sort mode");

	clp.add<double>("acousticweight", 'a', "acoustic model weight", false, 1.0);
	clp.add<double>("lmweight", 'l', "language model weight", false, 1.0);

	clp.add<std::string>("limit-to", '\0', "limit to dash separated reference ids", false, "");

	clp.add<std::string>("debug", '\0', "debug setting", false, "none");
	clp.parse_check(argc, argv);

	inputPath = clp.get<std::string>("inputpath");
	outputPath = clp.get<std::string>("outputpath");
	referencePath = clp.get<std::string>("referencepath");

	programMode = programFromString(clp.get<std::string>("mode"));
	weightMode = weightFromString(clp.get<std::string>("weighting"));

	aW = clp.get<double>("acousticweight");
	lW = clp.get<double>("lmweight");

	if(!clp.get<std::string>("limit-to").empty())
	{
		limitedReferenceIds = delimiterTokeniser(clp.get<std::string>("limit-to"), '-');
	}

	SLM::Logging::getInstance().set(clp.get<std::string>("debug"));

	reverse = clp.exist("reverse");

	char hostname[128];
	gethostname(hostname, sizeof hostname);
	hostName = std::string(hostname);


	L_V  	<< std::setw(30) << "Log level set on: " << SLM::Logging::getInstance().toString() << "\n"
			<< std::setw(30) << "Running on machine: " << hostName << "\n"
			<< "\n"
			<< std::setw(30) << "Program mode: " << toString(programMode) << "\n"
			<< std::setw(30) << "Weight mode: " << toString(weightMode) << "\n"
			<< std::setw(30) << "Reverse sort: " << (reverse ? "yes" : "no") << "\n"
			<< std::setw(30) << "Weights: " << "A[" << aW << "] L[" << lW << "] \n"
			<< "\n"
			<< std::setw(30) << "Limiting to: " << join(limitedReferenceIds, " ") << "\n"
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

bool ToolProgramOptions::isProgramMode(ProgramMode mode) const
{
	return mode == programMode;
}

ProgramMode ToolProgramOptions::getProgramMode() const
{
	return programMode;
}

bool ToolProgramOptions::isWeightMode(WeightMode mode) const
{
	return mode == weightMode;
}

WeightMode ToolProgramOptions::getWeightMode() const
{
	return weightMode;
}

double ToolProgramOptions::getAcousticWeight() const
{
	return aW;
}

double ToolProgramOptions::getLanguageModelWeight() const
{
	return lW;
}

std::vector<std::string> ToolProgramOptions::getLimitedReferenceIds() const
{
	return limitedReferenceIds;
}

bool ToolProgramOptions::isReverse() const
{
	return reverse;
}

SLM::Sorter* ToolProgramOptions::getSorter()
{
	switch (weightMode) {
		case WeightMode::ACOUSTIC:
			return new AcousticSorter(reverse);
		case WeightMode::LANGUAGE:
			return new LanguageModelSorter(reverse);
		case WeightMode::POWERWEIGHTED:
			return new PowerWeightedSorter(aW, lW, reverse);
		default:
			return new WeightedSorter(aW, lW, reverse);
	}
}

} /* namespace SLM */
