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

void ReferenceId::add(SLM::Hypotheses* nbl)
{
	nBestLists.push_back(nbl);
}

std::vector<SLM::Hypotheses*> ReferenceId::getNBestLists() const
{
	return nBestLists;
}

std::string ReferenceId::getId() const
{
	return id;
}

} /* namespace SLM */
