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

	void add(std::shared_ptr<SLM::AllHypotheses> nbl);
	std::vector<std::shared_ptr<SLM::AllHypotheses>> getNBestLists() const;
	std::vector<std::shared_ptr<SLM::AllHypotheses>> getTimeSortedNBestLists();

	void clear();

	std::string getId() const;
protected:
	std::string id;
	std::vector<std::shared_ptr<SLM::AllHypotheses>> nBestLists;
};

} /* namespace SLM */

#endif /* REFERENCEID_H_ */
