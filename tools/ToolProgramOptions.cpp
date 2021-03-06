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
        if(s == "bestinfo")
                return ProgramMode::SELECTBESTWINFO;

	return ProgramMode::SELECTBEST;
}

std::string toString(ProgramMode mode)
{
	switch (mode) {
		case ProgramMode::CEILING:
			return "ceiling";
		case ProgramMode::GRIDSEARCH:
			return "search";
                case ProgramMode::SELECTBESTWINFO:
                        return "bestinfo";
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
        if(s == "order")
                return WeightMode::ORDER;

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
                case WeightMode::ORDER:
                        return "order";
		default:
			return "weighted";
	}
}

ToolProgramOptions::ToolProgramOptions(int argc, char** argv) {
	L_A << "ToolProgramOptions\t+ Processing program options\n";

	clp.add<std::string>("inputpath", 'i', "directory with nbest lists", true);
	clp.add<std::string>("outputpath", 'o', "directory to write results", true);
	clp.add<std::string>("referencepath", 'r', "directory with reference files", true);

	clp.add<std::string>("vocabulary", 'v', "vocabulary file (only for oov counting)", false);

	clp.add<std::string>("mode", 'm', "program mode: ceiling, best, bestinfo, search", false);
	clp.add<std::string>("weighting", 'w', "weighting mode: acoustic, language, (power,)weighted (-a -l), order", false);
	clp.add("reverse", '\0', "reverse the sort mode");
	clp.add("padding", '\0', "adds 2 bos markers ('<s>') to the start of the hypothesis");

	clp.add<double>("acousticweight", 'a', "acoustic model weight", false, 1.0);
	clp.add<double>("lmweight", 'l', "language model weight", false, 1.0);
	clp.add<double>("offset", 'O', "offset for the weights", false, 0.0);

	clp.add<std::string>("limit-to", '\0', "limit to dash separated reference ids", false, "");

	clp.add<std::string>("debug", '\0', "debug setting", false, "none");
	clp.parse_check(argc, argv);

	inputPath = clp.get<std::string>("inputpath");
	outputPath = clp.get<std::string>("outputpath");
	referencePath = clp.get<std::string>("referencepath");

	vocabFile = clp.get<std::string>("vocabulary");

	programMode = programFromString(clp.get<std::string>("mode"));
	weightMode = weightFromString(clp.get<std::string>("weighting"));

	aW = clp.get<double>("acousticweight");
	lW = clp.get<double>("lmweight");
        offset = clp.get<double>("offset");

	if(!clp.get<std::string>("limit-to").empty())
	{
		limitedReferenceIds = delimiterTokeniser(clp.get<std::string>("limit-to"), '-');
	}

	SLM::Logging::getInstance().set(clp.get<std::string>("debug"));

	reverse = clp.exist("reverse");
        padding = clp.exist("padding");

	char hostname[128];
	gethostname(hostname, sizeof hostname);
	hostName = std::string(hostname);


	L_V  	<< std::setw(30) << "Log level set on: " << SLM::Logging::getInstance().toString() << "\n"
			<< std::setw(30) << "Running on machine: " << hostName << "\n"
			<< "\n"
			<< std::setw(30) << "Program mode: " << toString(programMode) << "\n"
			<< std::setw(30) << "Weight mode: " << toString(weightMode) << "\n"
			<< std::setw(30) << "Reverse sort: " << (reverse ? "yes" : "no") << "\n"
			<< std::setw(30) << "Add padding: " << (padding ? "yes" : "no") << "\n"
			<< std::setw(30) << "Weights: " << "A[" << aW << "] L[" << lW << "] offset[" << offset << "]\n"
			<< "\n"
			<< std::setw(30) << "Limiting to: " << join(limitedReferenceIds, " ") << "\n"
			<< "\n"
			<< std::setw(30) << "Input path: " << inputPath << "\n"
			<< std::setw(30) << "Output path: " << outputPath << "\n"
			<< std::setw(30) << "Reference path: " << referencePath << "\n"
			<< std::setw(30) << "Vocabulary: " << (vocabFile.empty() ? "no" : vocabFile) << "\n";

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

double ToolProgramOptions::getOffset() const
{
        return offset;
}

std::vector<std::string> ToolProgramOptions::getLimitedReferenceIds() const
{
	return limitedReferenceIds;
}

bool ToolProgramOptions::isReverse() const
{
	return reverse;
}

bool ToolProgramOptions::addPadding() const
{
        return padding;
}

SLM::Sorter* ToolProgramOptions::getSorter()
{
	switch (weightMode) {
		case WeightMode::ACOUSTIC:
			return new AcousticSorter(reverse);
		case WeightMode::LANGUAGE:
			return new LanguageModelSorter(reverse);
		case WeightMode::POWERWEIGHTED:
			return new PowerWeightedSorter(aW, lW, offset, reverse);
                case WeightMode::ORDER:
                        return new NoSorter(reverse);
		default:
			return new WeightedSorter(aW, lW, offset, reverse);
	}
}

std::string ToolProgramOptions::getVocabFile() const
{
	return vocabFile;
}

} /* namespace SLM */
