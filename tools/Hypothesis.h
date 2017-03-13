/*
 * Hypothesis.h
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#ifndef HYPOTHESIS_H_
#define HYPOTHESIS_H_

#include <string>
#include <exception>
#include <vector>

namespace SLM {

class IncompleteHypothesis: public std::exception
{
  virtual const char* what() const throw()
  {
    return "Hypothesis is not complete";
  }
};

class Hypothesis {
public:
	Hypothesis(const std::string& hypothesisEntry);
	virtual ~Hypothesis();

	double getAcousticScore() const;
	double getLanguageModelScore() const;
	std::string getSentence() const;
	std::vector<std::string> getTokens() const;
	int getCount() const;
protected:
	double acousticScore;
	double languageModelScore;
	int count;
	std::vector<std::string> tokens;
	std::string sentence;
};

} /* namespace SLM */

#endif /* HYPOTHESIS_H_ */
