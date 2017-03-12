/*
 * ReferenceId.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#include "ReferenceId.h"

#include <iostream>
#include <algorithm>

namespace SLM {

ReferenceId::ReferenceId(const std::string& id) : id(id) {
//	std::cout << "ReferenceId[" << id << "] intitialised" << std::endl;

}

ReferenceId::~ReferenceId() {
//	clear();
}

void ReferenceId::add(SLM::Hypotheses* nbl)
{
	nBestLists.push_back(nbl);
}

std::vector<SLM::Hypotheses*> ReferenceId::getNBestLists() const
{
	return nBestLists;
}

std::vector<SLM::Hypotheses*> ReferenceId::getTimeSortedNBestLists()
{
	std::sort(nBestLists.begin(), nBestLists.end(),
	    [](const Hypotheses* const a, const Hypotheses* const b) -> bool
	{
	    return a->getStartTime() < b->getStartTime();
	});

	return nBestLists;
}

void ReferenceId::clear()
{
	for (auto i : nBestLists)
	    delete i;
	nBestLists.clear();
}

std::string ReferenceId::getId() const
{
	return id;
}

} /* namespace SLM */
