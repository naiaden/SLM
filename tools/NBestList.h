/*
 * NBestList.h
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#ifndef NBESTLIST_H_
#define NBESTLIST_H_

#include <string>
#include <vector>

#include "Hypothesis.h"

namespace SLM {

class NBestList {
public:
	NBestList(const std::string& fileName, const std::string& path);
	virtual ~NBestList();

	std::vector<SLM::Hypothesis*> getHypotheses() const;
	std::string getFileName() const;
protected:
	std::string fileName;
	std::string path;
	std::vector<SLM::Hypothesis*> hypotheses;
};

} /* namespace SLM */

#endif /* NBESTLIST_H_ */
