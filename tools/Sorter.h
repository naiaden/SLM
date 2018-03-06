/*
 * Sorter.h
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#ifndef SORTER_H_
#define SORTER_H_

#include <memory>

#include "Hypotheses.h"
#include "ReferenceId.h"

namespace SLM {

class Sorter {
public:
	Sorter(bool reverse = false);
	virtual ~Sorter();

	virtual SLM::Hypothesis sort(const SLM::AllHypotheses& nBestList) const = 0;
	virtual bool compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const = 0;

	virtual std::string getName() const = 0;
protected:
	bool reverse;
};

class NoSorter : public Sorter {
public:
	NoSorter(bool reverse = false);
	virtual ~NoSorter();

	SLM::Hypothesis sort(const SLM::AllHypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const;

	std::string getName() const;
};

class AcousticSorter : public Sorter {
public:
	AcousticSorter(bool reverse = false);
	virtual ~AcousticSorter();

	SLM::Hypothesis sort(const SLM::AllHypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const;

	std::string getName() const;
};

class LanguageModelSorter : public Sorter {
public:
	LanguageModelSorter(bool reverse = false);
	virtual ~LanguageModelSorter();

	SLM::Hypothesis sort(const SLM::AllHypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const;

	std::string getName() const;
};

class WeightedSorter : public Sorter {
public:
	WeightedSorter(double acousticWeight, double languageModelWeight, double offset, bool reverse = false);
	virtual ~WeightedSorter();

	SLM::Hypothesis sort(const SLM::AllHypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const;

	std::string getName() const;
protected:
	double acousticWeight;
	double languageModelWeight;
        double offset;
};

class PowerWeightedSorter : public WeightedSorter {
public:
	PowerWeightedSorter(double acousticWeight, double languageModelWeight, double offset, bool reverse = false);
	virtual ~PowerWeightedSorter();

	SLM::Hypothesis sort(const SLM::AllHypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const;

	std::string getName() const;
};

} /* namespace SLM */

#endif /* SORTER_H_ */
