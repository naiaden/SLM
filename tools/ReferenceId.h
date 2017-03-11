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

#include "Hypotheses.h"

namespace SLM {

class ReferenceId {
public:
	ReferenceId(const std::string& id);
	virtual ~ReferenceId();

	void add(SLM::Hypotheses* nbl);
	std::vector<SLM::Hypotheses*> getNBestLists() const;

	std::string getId() const;
protected:
	std::string id;
	std::vector<SLM::Hypotheses*> nBestLists;
};

} /* namespace SLM */

#endif /* REFERENCEID_H_ */
