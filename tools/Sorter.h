/*
 * Sorter.h
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#ifndef SORTER_H_
#define SORTER_H_

#include "Hypotheses.h"
#include "ReferenceId.h"

namespace SLM {

class Sorter {
public:
	Sorter();
	virtual ~Sorter();

	virtual SLM::Hypothesis sort(const SLM::Hypotheses& nBestList) const = 0;
	virtual bool compare(SLM::Hypothesis* l, SLM::Hypothesis* r) const = 0;

	virtual std::string getName() const = 0;
};

class AcousticSorter : public Sorter {
public:
	AcousticSorter();
	virtual ~AcousticSorter();

	SLM::Hypothesis sort(const SLM::Hypotheses& nBestList) const;
	bool compare(SLM::Hypothesis* l, SLM::Hypothesis* r) const;

	std::string getName() const;
};

class WeightedSorter : public Sorter {
public:
	WeightedSorter(double acousticWeight, double languageModelWeight);
	virtual ~WeightedSorter();

	SLM::Hypothesis sort(const SLM::Hypotheses& nBestList) const;
	bool compare(SLM::Hypothesis* l, SLM::Hypothesis* r) const;

	std::string getName() const;
protected:
	double acousticWeight;
	double languageModelWeight;
};

} /* namespace SLM */

#endif /* SORTER_H_ */
