/*
 * Logging.h
 *
 *  Created on: Dec 10, 2016
 *      Author: louis
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <ostream>
#include <iostream>

#define L_N SLM::Logging::getInstance() << SLM::LoggingLevel::NONE
#define L_I SLM::Logging::getInstance() << SLM::LoggingLevel::INFO
#define L_V SLM::Logging::getInstance() << SLM::LoggingLevel::VERBOSE
#define L_P SLM::Logging::getInstance() << SLM::LoggingLevel::PATTERN
#define L_S SLM::Logging::getInstance() << SLM::LoggingLevel::SUBPATTERN
#define L_A SLM::Logging::getInstance() << SLM::LoggingLevel::ALL

namespace SLM {


enum LoggingLevel { NONE = 0, INFO = 10, VERBOSE = 20, PATTERN = 30, SUBPATTERN = 40, ALL = 100};


class Logging {
public:


	static Logging& getInstance()
	{
		static Logging instance;
		return instance;
	}

	Logging(Logging const&) = delete;
	void operator=(Logging const&) = delete;

	LoggingLevel get() const;
	std::string toString() const;
	std::string toString(LoggingLevel level) const;
	void set(LoggingLevel level);
	void set(const std::string& level);

	bool doLog(LoggingLevel currentLevel) const;

	template<typename T> Logging& operator<<(const T& stream)
	{
		if(doLog(coutLevel))
		{
			std::cout << stream;
		}

		return *this;
	}

	typedef std::ostream& (*stream_function)(std::ostream&);
	Logging& operator<<(stream_function func)
	{
		if(doLog(coutLevel))
		{
			func(std::cout);
		}
		return *this;
	}

	Logging& operator<<(LoggingLevel dl)
	{
		coutLevel = dl;
		return *this;
	}

private:
	LoggingLevel coutLevel = LoggingLevel::NONE;
	LoggingLevel loggingLevel = LoggingLevel::NONE;

	Logging() {}
};

} /* namespace SLM */

#endif /* LOGGING_H_ */
