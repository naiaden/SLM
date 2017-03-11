/*
 * Hypothesis.h
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#ifndef HYPOTHESIS_H_
#define HYPOTHESIS_H_

#include <string>
#include <vector>

namespace SLM {

class Hypothesis {
public:
	Hypothesis(const std::string& hypothesisEntry);
	virtual ~Hypothesis();

	double getAcousticScore() const;
	double getLanguageModelScore() const;
	std::string getSentence() const;
protected:
	double acousticScore;
	double languageModelScore;
	std::vector<std::string> tokens;
	std::string sentence;
};

} /* namespace SLM */

#endif /* HYPOTHESIS_H_ */
