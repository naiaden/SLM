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
#include <exception>

#include "Hypothesis.h"
//#include "Sorter.h"



namespace SLM {

class /*SLM::*/Sorter;

class Hypotheses {
public:
	Hypotheses(const std::string& fileName, const std::string& path);
	virtual ~Hypotheses();

	virtual std::string getFileName() const final;

	virtual double getStartTime() const final;
protected:
	std::string fileName;
	std::string path;

	double startTime;

};

class AllHypotheses : public SLM::Hypotheses {
public:
	AllHypotheses(const std::string& fileName, const std::string& path);
	virtual ~AllHypotheses();

	std::vector<std::shared_ptr<SLM::Hypothesis>> getHypotheses() const;
protected:
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses;
};

class BestHypotheses : public SLM::Hypotheses {
public:
	BestHypotheses(const std::string& fileName, const std::string& path, SLM::Sorter* sorter);
	virtual ~BestHypotheses();

protected:
	std::shared_ptr<SLM::PartialHypothesis> bestHypothesis;
//	std::
};

} /* namespace SLM */

#endif /* HYPOTHESES_H_ */
