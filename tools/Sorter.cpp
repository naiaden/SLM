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
#include <iostream>
#include <iomanip>

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

NoSorter::NoSorter(bool reverse) : Sorter(reverse) {
	// TODO Auto-generated constructor stub

}

NoSorter::~NoSorter() {
	// TODO Auto-generated destructor stub
}

bool NoSorter::compare(std::shared_ptr<SLM::PartialHypothesis> l, std::shared_ptr<SLM::PartialHypothesis> r) const
{
    return true;
}

SLM::Hypothesis NoSorter::sort(const SLM::AllHypotheses& nBestList) const
{
	std::vector<std::shared_ptr<SLM::Hypothesis>> hypotheses = nBestList.getHypotheses();
        if(reverse) std::reverse(hypotheses.begin(), hypotheses.end());
	return *(hypotheses.front());
}

std::string NoSorter::getName() const
{
	return std::string(reverse ? "reverse-" : "") + "order";
}


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

WeightedSorter::WeightedSorter(double acousticWeight, double languageModelWeight, double offset, bool reverse)
	: Sorter(reverse), acousticWeight(acousticWeight), languageModelWeight(languageModelWeight), offset(offset) {
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

        double lval = acousticWeight*l->getAcousticScore() + languageModelWeight*l->getLanguageModelScore() + offset*l->getCount();
        double rval = acousticWeight*r->getAcousticScore() + languageModelWeight*r->getLanguageModelScore() + offset;

        //std::cout << "L(A/B/C): " << std::setprecision(5) << acousticWeight*l->getAcousticScore()
        //          << "\t" << std::setprecision(5) << languageModelWeight*l->getLanguageModelScore()
        //          << "\t" << std::setprecision(5) << l->getCount()
        //          << "\t" << std::setprecision(5) << lval
        //          << "\tR(A/B/C): " << std::setprecision(5) << acousticWeight*r->getAcousticScore()
        //          << "\t" << std::setprecision(5) << languageModelWeight*r->getLanguageModelScore()
        //          << "\t" << std::setprecision(5) << r->getCount()
        //          << "\t" << std::setprecision(5) << rval
        //          << "\t" << std::string((lval > rval) ? "L" : "R")
        //          << std::endl;


	if(reverse)
		return acousticWeight*l->getAcousticScore() + languageModelWeight*l->getLanguageModelScore() + offset*l->getCount()
			> acousticWeight*r->getAcousticScore() + languageModelWeight*r->getLanguageModelScore() + offset*r->getCount();

    return acousticWeight*l->getAcousticScore() + languageModelWeight*l->getLanguageModelScore() + offset*l->getCount()
    		< acousticWeight*r->getAcousticScore() + languageModelWeight*r->getLanguageModelScore() + offset*r->getCount();
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

PowerWeightedSorter::PowerWeightedSorter(double acousticWeight, double languageModelWeight, double offset, bool reverse)
	: WeightedSorter(acousticWeight, languageModelWeight, offset, reverse) {
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
		pow(acousticWeight,l->getAcousticScore()) - pow(languageModelWeight,l->getLanguageModelScore()) + offset
			< pow(acousticWeight,r->getAcousticScore()) - pow(languageModelWeight,r->getLanguageModelScore()) + offset;
	}
    return pow(acousticWeight,l->getAcousticScore()) - pow(languageModelWeight,l->getLanguageModelScore()) + offset
    		> pow(acousticWeight,r->getAcousticScore()) - pow(languageModelWeight,r->getLanguageModelScore()) + offset;
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
