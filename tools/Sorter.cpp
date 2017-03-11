/*
 * Sorter.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#include "Sorter.h"

#include <vector>
#include <algorithm>

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
	std::vector<SLM::Hypothesis*> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](SLM::Hypothesis * lhs, SLM::Hypothesis * rhs) {
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

bool AcousticSorter::compare(SLM::Hypothesis* l, SLM::Hypothesis* r) const
{
	if(!l || !r) return false;
    return l->getAcousticScore() > r->getAcousticScore();
}

SLM::Hypothesis AcousticSorter::sort(const SLM::Hypotheses& nBestList) const
{
	std::vector<SLM::Hypothesis*> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](SLM::Hypothesis * lhs, SLM::Hypothesis * rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string AcousticSorter::getName() const
{
	return "acoustic";
}

////////////////////////

WeightedSorter::WeightedSorter(double acousticWeight, double languageModelWeight)
	: acousticWeight(acousticWeight), languageModelWeight(languageModelWeight) {
	// TODO Auto-generated constructor stub

}

WeightedSorter::~WeightedSorter() {
	// TODO Auto-generated destructor stub
}

bool WeightedSorter::compare(SLM::Hypothesis* l, SLM::Hypothesis* r) const
{
	if(!l || !r) return false;
    return acousticWeight*l->getAcousticScore() - languageModelWeight*l->getLanguageModelScore()
    		> acousticWeight*r->getAcousticScore() - languageModelWeight*r->getLanguageModelScore();
}

SLM::Hypothesis WeightedSorter::sort(const SLM::Hypotheses& nBestList) const
{
	std::vector<SLM::Hypothesis*> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](SLM::Hypothesis * lhs, SLM::Hypothesis * rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string WeightedSorter::getName() const
{
	return "weighted";
}

} /* namespace SLM */
