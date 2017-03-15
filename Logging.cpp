/*
 * Logging.cpp
 *
 *  Created on: Dec 10, 2016
 *      Author: louis
 */

#include "Logging.h"


namespace SLM {

const std::string LL_NONE = "none";
const std::string LL_INFO = "info";
const std::string LL_VERBOSE = "verbose";
const std::string LL_PATTERN = "pattern";
const std::string LL_SUBPATTERN = "subpattern";
const std::string LL_ALL = "all";

std::string Logging::toString(LoggingLevel debugLevel) const
{
	switch (debugLevel) {
		case LoggingLevel::NONE:
			return LL_NONE;
		case LoggingLevel::PATTERN:
			return LL_PATTERN;
		case LoggingLevel::SUBPATTERN:
			return LL_SUBPATTERN;
		case LoggingLevel::ALL:
			return LL_ALL;
		case LoggingLevel::VERBOSE:
			return LL_VERBOSE;
		case LoggingLevel::INFO:
			return LL_INFO;
		default:
			return "unknown";
	}
}

std::string Logging::toString() const
{
	return toString(loggingLevel);
}

LoggingLevel Logging::get() const
{
	std::cout << "RETURN " << toString(loggingLevel) << std::endl;
	return loggingLevel;
}

void Logging::set(LoggingLevel level)
{
	loggingLevel = level;

	std::cout << "1Level set to " << toString(loggingLevel) << std::endl;
}

void Logging::set(const std::string& level)
{
	if(level == LL_PATTERN) loggingLevel = LoggingLevel::PATTERN;
	else if(level == LL_SUBPATTERN) loggingLevel = LoggingLevel::SUBPATTERN;
	else if(level == LL_ALL) loggingLevel = LoggingLevel::ALL;
	else if(level == LL_INFO) loggingLevel = LoggingLevel::INFO;
	else if(level == LL_VERBOSE) loggingLevel = LoggingLevel::VERBOSE;
	else if(level == LL_NONE) loggingLevel = LoggingLevel::NONE;

	L_V << "Debug output level is " << toString(loggingLevel) << "\n";
}

bool Logging::doLog(LoggingLevel currentLevel) const
{
	return currentLevel <= loggingLevel && loggingLevel != LoggingLevel::NONE;
}

} /* namespace SLM */
