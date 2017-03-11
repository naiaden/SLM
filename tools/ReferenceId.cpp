/*
 * ReferenceId.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#include "ReferenceId.h"

#include <iostream>

namespace SLM {

ReferenceId::ReferenceId(const std::string& id) : id(id) {
//	std::cout << "ReferenceId[" << id << "] intitialised" << std::endl;

}

ReferenceId::~ReferenceId() {
//	std::cout << "ReferenceId[" << id << "] deleted" << std::endl;
}

void ReferenceId::add(SLM::NBestList* nbl)
{
	nBestLists.push_back(nbl);
}

std::vector<SLM::NBestList*> ReferenceId::getNBestLists() const
{
	return nBestLists;
}

} /* namespace SLM */
