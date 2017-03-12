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

#include <memory>

#include "Hypotheses.h"

namespace SLM {

class ReferenceId {
public:
	ReferenceId(const std::string& id);
	virtual ~ReferenceId();

	void add(std::shared_ptr<SLM::Hypotheses> nbl);
	std::vector<std::shared_ptr<SLM::Hypotheses>> getNBestLists() const;
	std::vector<std::shared_ptr<SLM::Hypotheses>> getTimeSortedNBestLists();

	void clear();

	std::string getId() const;
protected:
	std::string id;
	std::vector<std::shared_ptr<SLM::Hypotheses>> nBestLists;
};

} /* namespace SLM */

#endif /* REFERENCEID_H_ */
