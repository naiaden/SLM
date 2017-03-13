/*
 * Sorter.h
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#ifndef SORTER_H_
#define SORTER_H_

#include "Hypotheses.h"

#include <memory>

#include "ReferenceId.h"

namespace SLM {

class Sorter {
public:
	Sorter(bool reverse = false);
	virtual ~Sorter();

	virtual SLM::Hypothesis sort(const SLM::Hypotheses& nBestList) const = 0;
	virtual bool compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const = 0;

	virtual std::string getName() const = 0;
protected:
	bool reverse;
};

class AcousticSorter : public Sorter {
public:
	AcousticSorter(bool reverse = false);
	virtual ~AcousticSorter();

	SLM::Hypothesis sort(const SLM::Hypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const;

	std::string getName() const;
};

class LanguageModelSorter : public Sorter {
public:
	LanguageModelSorter(bool reverse = false);
	virtual ~LanguageModelSorter();

	SLM::Hypothesis sort(const SLM::Hypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const;

	std::string getName() const;
};

class WeightedSorter : public Sorter {
public:
	WeightedSorter(double acousticWeight, double languageModelWeight, bool reverse = false);
	virtual ~WeightedSorter();

	SLM::Hypothesis sort(const SLM::Hypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const;

	std::string getName() const;
protected:
	double acousticWeight;
	double languageModelWeight;
};

class PowerWeightedSorter : public WeightedSorter {
public:
	PowerWeightedSorter(double acousticWeight, double languageModelWeight, bool reverse = false);
	virtual ~PowerWeightedSorter();

	SLM::Hypothesis sort(const SLM::Hypotheses& nBestList) const;
	bool compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const;

	std::string getName() const;
};

} /* namespace SLM */

#endif /* SORTER_H_ */
