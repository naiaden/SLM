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

#include "Logging.h"
#include "Hypothesis.h"

namespace SLM {

Sorter::Sorter(bool reverse) : reverse(reverse) {
	// TODO Auto-generated constructor stub

}

Sorter::~Sorter() {
	// TODO Auto-generated destructor stub
}

//SLM::Hypothesis Sorter::sort(const SLM::Hypotheses& nBestList) const
//{
//	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
//	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::Hypothesis> lhs, std::shared_ptr<SLM::Hypothesis> rhs) {
//	        return this->compare(lhs,rhs);
//	    });
//	return *(hypotheses.front());
//}

////////////////////////

AcousticSorter::AcousticSorter(bool reverse) : Sorter(reverse) {
	// TODO Auto-generated constructor stub

}

AcousticSorter::~AcousticSorter() {
	// TODO Auto-generated destructor stub
}

bool AcousticSorter::compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const
{
	if(!l || !r)
		return false;
	if(reverse)
		l->getAcousticScore() < r->getAcousticScore();
    return l->getAcousticScore() > r->getAcousticScore();
}

SLM::Hypothesis AcousticSorter::sort(const SLM::AllHypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::PartialHypothesis> lhs, std::shared_ptr<SLM::PartialHypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string AcousticSorter::getName() const
{
	return std::string(reverse ? "reverse-" : "") + "acoustic";
}

////////////////////////

LanguageModelSorter::LanguageModelSorter(bool reverse) : Sorter(reverse) {
	// TODO Auto-generated constructor stub

}

LanguageModelSorter::~LanguageModelSorter() {
	// TODO Auto-generated destructor stub
}

bool LanguageModelSorter::compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const
{
	if(!l || !r)
		return false;
	if(reverse)
		l->getLanguageModelScore() < r->getLanguageModelScore();
    return l->getLanguageModelScore() < r->getLanguageModelScore();
}

SLM::Hypothesis LanguageModelSorter::sort(const SLM::AllHypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::PartialHypothesis> lhs, std::shared_ptr<SLM::PartialHypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string LanguageModelSorter::getName() const
{
	return std::string(reverse ? "reverse-" : "") + "languagemodel";
}

////////////////////////

WeightedSorter::WeightedSorter(double acousticWeight, double languageModelWeight, bool reverse)
	: Sorter(reverse), acousticWeight(acousticWeight), languageModelWeight(languageModelWeight) {
	// TODO Auto-generated constructor stub

}

WeightedSorter::~WeightedSorter() {
	// TODO Auto-generated destructor stub
}

bool WeightedSorter::compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const
{
	if(!l || !r)
	{
		return false;
	}

	if(reverse)
		return acousticWeight*l->getAcousticScore() + languageModelWeight*l->getLanguageModelScore()
			> acousticWeight*r->getAcousticScore() + languageModelWeight*r->getLanguageModelScore();

    return acousticWeight*l->getAcousticScore() + languageModelWeight*l->getLanguageModelScore()
    		< acousticWeight*r->getAcousticScore() + languageModelWeight*r->getLanguageModelScore();
}

SLM::Hypothesis WeightedSorter::sort(const SLM::AllHypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::PartialHypothesis> lhs, std::shared_ptr<SLM::PartialHypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string WeightedSorter::getName() const
{
	return std::string(reverse ? "reverse-" : "") + "weighted";
}

////////////////////////

PowerWeightedSorter::PowerWeightedSorter(double acousticWeight, double languageModelWeight, bool reverse)
	: WeightedSorter(acousticWeight, languageModelWeight, reverse) {
	// TODO Auto-generated constructor stub

}

PowerWeightedSorter::~PowerWeightedSorter() {
	// TODO Auto-generated destructor stub
}

bool PowerWeightedSorter::compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const
{
	if(!l || !r)
		return false;
	if(reverse)
	{
		pow(acousticWeight,l->getAcousticScore()) - pow(languageModelWeight,l->getLanguageModelScore())
			< pow(acousticWeight,r->getAcousticScore()) - pow(languageModelWeight,r->getLanguageModelScore());
	}
    return pow(acousticWeight,l->getAcousticScore()) - pow(languageModelWeight,l->getLanguageModelScore())
    		> pow(acousticWeight,r->getAcousticScore()) - pow(languageModelWeight,r->getLanguageModelScore());
}

SLM::Hypothesis PowerWeightedSorter::sort(const SLM::AllHypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
	std::sort(hypotheses.begin(), hypotheses.end(), [&](std::shared_ptr<SLM::PartialHypothesis> lhs, std::shared_ptr<SLM::PartialHypothesis> rhs) {
	        return this->compare(lhs,rhs);
	    });
	return *(hypotheses.front());
}

std::string PowerWeightedSorter::getName() const
{
	return std::string(reverse ? "reverse-" : "") + "powerweighted";
}

} /* namespace SLM */
