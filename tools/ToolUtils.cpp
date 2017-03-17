/*
 * ToolUtils.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: louis
 */

#include "ToolUtils.h"

#include <experimental/filesystem>
#include <algorithm>
#include <numeric>

#include "Utils.h"
#include "Logging.h"

// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
double WER(const std::vector<std::string> &ref, const std::vector<std::string> &hyp)
{
	// To change the type this function manipulates and returns, change
	// the return type and the types of the two variables below.
	int s1len = ref.size();
	int s2len = hyp.size();

	auto column_start = (decltype(s1len))1;

	auto column = new decltype(s1len)[s1len + 1];
	std::iota(column + column_start, column + s1len + 1, column_start);

	for (auto x = column_start; x <= s2len; x++) {
		column[0] = x;
		auto last_diagonal = x - column_start;
		for (auto y = column_start; y <= s1len; y++) {
			auto old_diagonal = column[y];
			auto possibilities = {
				column[y] + 1,
				column[y - 1] + 1,
				last_diagonal + (ref[y - 1] == hyp[x - 1]? 0 : 1)
			};
			column[y] = std::min(possibilities);
			last_diagonal = old_diagonal;
		}
	}
	auto result = column[s1len];
	delete[] column;
	return 100* (result/(double) s1len);
}

std::unordered_map<std::string, SLM::ReferenceId> collectReferenceIds(const std::string& path)
{
	std::unordered_map<std::string, SLM::ReferenceId> referenceIds;

	for (auto & p : std::experimental::filesystem::directory_iterator(path))
	{
		std::string fileName = delimiterTokeniser(p.path(), '/').back();
		std::string fileId = delimiterTokeniser(fileName, '.').front();

		referenceIds.emplace (fileId, SLM::ReferenceId(fileId));
	}

	L_I << "RescoreTools: " << referenceIds.size() << " references found\n";

	return referenceIds;
}

std::vector<std::string> getFilesForReferenceId(SLM::ReferenceId& rId, const std::string& path)
{
	std::vector<std::string> files;

	for (auto & p : std::experimental::filesystem::directory_iterator(path))
	{
		std::string fileName = delimiterTokeniser(p.path(), '/').back();
		if(startsWith(fileName, rId.getId()))
		{
			files.push_back(fileName);
		}
	}

	return files;
}

SLM::ReferenceId collectHypothesesForReferenceId(SLM::ReferenceId& rId, const std::string& path)
{
	L_V << "RescoreTools: " << rId.getId() << "\n";

	std::vector<std::string> files = getFilesForReferenceId(rId, path);
	for(auto& file : files)
	{
		L_A << "RescoreTools: \t" << file << " " << path << "\n";
		rId.add(std::make_shared<SLM::AllHypotheses>(file, path));
	}

	L_V << "RescoreTools: " << rId.getNBestLists().size() << " hypotheses for " << rId.getId() << "\n" << std::endl;

	return rId;
}


std::unordered_map<std::string, SLM::ReferenceId> referenceIdsPreprocessing(std::unordered_map<std::string, SLM::ReferenceId> references, std::vector<std::string> specials)
{
    if(specials.size())
    {
    	std::unordered_map<std::string, SLM::ReferenceId> subsetReferenceIds;
    	for(auto & r : specials)
    	{
    		std::unordered_map<std::string,SLM::ReferenceId>::const_iterator rIt = references.find (r);

    		  if ( rIt != references.end() )
    			  subsetReferenceIds.emplace(r, rIt->second);
    	}

    	return subsetReferenceIds;
    } else
    {
    	return references;
    }
}
