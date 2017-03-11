/*
 * ReferenceId.h
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#ifndef REFERENCEID_H_
#define REFERENCEID_H_

#include <string>
#include <vector>

#include "NBestList.h"

namespace SLM {

class ReferenceId {
public:
	ReferenceId(const std::string& id);
	virtual ~ReferenceId();

	void add(SLM::NBestList* nbl);
	std::vector<SLM::NBestList*> getNBestLists() const;
protected:
	std::string id;
	std::vector<SLM::NBestList*> nBestLists;
};

} /* namespace SLM */

#endif /* REFERENCEID_H_ */
