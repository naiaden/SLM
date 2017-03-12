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

#include <memory>

#include "Hypothesis.h"

namespace SLM {

class Hypotheses {
public:
	Hypotheses(const std::string& fileName, const std::string& path);
	virtual ~Hypotheses();

	std::vector<std::shared_ptr<SLM::Hypothesis>> getHypotheses() const;
	std::string getFileName() const;

	double getStartTime() const;
protected:
	std::string fileName;
	std::string path;
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses;

	double startTime;
};

} /* namespace SLM */

#endif /* HYPOTHESES_H_ */
