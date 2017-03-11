/*
 * NBestList.h
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#ifndef HYPOTHESES_H_
#define HYPOTHESES_H_

#include <string>
#include <vector>

#include "Hypothesis.h"

namespace SLM {

class Hypotheses {
public:
	Hypotheses(const std::string& fileName, const std::string& path);
	virtual ~Hypotheses();

	std::vector<SLM::Hypothesis*> getHypotheses() const;
	std::string getFileName() const;
protected:
	std::string fileName;
	std::string path;
	std::vector<SLM::Hypothesis*> hypotheses;
};

} /* namespace SLM */

#endif /* HYPOTHESES_H_ */
