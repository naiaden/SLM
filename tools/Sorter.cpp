/*
 * Sorter.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#include "Sorter.h"

#include <vector>
#include <algorithm>
#include <cmath>

#include "Hypothesis.h"

namespace SLM {

Sorter::Sorter() {
	// TODO Auto-generated constructor stub

}

Sorter::~Sorter() {
	// TODO Auto-generated destructor stub
}

SLM::Hypothesis Sorter::sort(const SLM::Hypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::Hypothesis> lhs, std::shared_ptr<SLM::Hypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

////////////////////////

AcousticSorter::AcousticSorter() {
	// TODO Auto-generated constructor stub

}

AcousticSorter::~AcousticSorter() {
	// TODO Auto-generated destructor stub
}

bool AcousticSorter::compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const
{
	if(!l || !r) return false;
    return l->getAcousticScore() > r->getAcousticScore();
}

SLM::Hypothesis AcousticSorter::sort(const SLM::Hypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::Hypothesis> lhs, std::shared_ptr<SLM::Hypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string AcousticSorter::getName() const
{
	return "acoustic";
}

////////////////////////

LanguageModelSorter::LanguageModelSorter() {
	// TODO Auto-generated constructor stub

}

LanguageModelSorter::~LanguageModelSorter() {
	// TODO Auto-generated destructor stub
}

bool LanguageModelSorter::compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const
{
	if(!l || !r) return false;
    return l->getLanguageModelScore() < r->getLanguageModelScore();
}

SLM::Hypothesis LanguageModelSorter::sort(const SLM::Hypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::Hypothesis> lhs, std::shared_ptr<SLM::Hypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string LanguageModelSorter::getName() const
{
	return "languagemodel";
}

////////////////////////

WeightedSorter::WeightedSorter(double acousticWeight, double languageModelWeight)
	: acousticWeight(acousticWeight), languageModelWeight(languageModelWeight) {
	// TODO Auto-generated constructor stub

}

WeightedSorter::~WeightedSorter() {
	// TODO Auto-generated destructor stub
}

bool WeightedSorter::compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const
{
	if(!l || !r) return false;
    return acousticWeight*l->getAcousticScore() - languageModelWeight*l->getLanguageModelScore()
    		> acousticWeight*r->getAcousticScore() - languageModelWeight*r->getLanguageModelScore();
}

SLM::Hypothesis WeightedSorter::sort(const SLM::Hypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::Hypothesis> lhs, std::shared_ptr<SLM::Hypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string WeightedSorter::getName() const
{
	return "weighted";
}

////////////////////////

PowerWeightedSorter::PowerWeightedSorter(double acousticWeight, double languageModelWeight)
	: WeightedSorter(acousticWeight, languageModelWeight) {
	// TODO Auto-generated constructor stub

}

PowerWeightedSorter::~PowerWeightedSorter() {
	// TODO Auto-generated destructor stub
}

bool PowerWeightedSorter::compare(std::shared_ptr<SLM::Hypothesis> l, std::shared_ptr<SLM::Hypothesis> r) const
{
	if(!l || !r) return false;
    return pow(acousticWeight,l->getAcousticScore()) - pow(languageModelWeight,l->getLanguageModelScore())
    		> pow(acousticWeight,r->getAcousticScore()) - pow(languageModelWeight,r->getLanguageModelScore());
}

SLM::Hypothesis PowerWeightedSorter::sort(const SLM::Hypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::Hypothesis> lhs, std::shared_ptr<SLM::Hypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string PowerWeightedSorter::getName() const
{
	return "powerweighted";
}

} /* namespace SLM */
