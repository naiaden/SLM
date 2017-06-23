/*
 * ToolUtils.h
 *
 *  Created on: Mar 17, 2017
 *      Author: louis
 */

#ifndef TOOLS_TOOLUTILS_H_
#define TOOLS_TOOLUTILS_H_

#include <vector>
#include <string>
#include <unordered_map>

#include "ReferenceId.h"

double WER(const std::vector<std::string> &ref, const std::vector<std::string> &hyp);
double OOV(int count, int oov);

std::unordered_map<std::string, SLM::ReferenceId> collectReferenceIds(const std::string& path);
std::vector<std::string> getFilesForReferenceId(SLM::ReferenceId& rId, const std::string& path);
SLM::ReferenceId collectHypothesesForReferenceId(SLM::ReferenceId& rId, const std::string& path, bool addPadding = false);
std::unordered_map<std::string, SLM::ReferenceId> referenceIdsPreprocessing(std::unordered_map<std::string, SLM::ReferenceId> references, std::vector<std::string> specials);

#endif /* TOOLS_TOOLUTILS_H_ */
