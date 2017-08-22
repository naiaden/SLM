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

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "Utils.h"

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

LevenshteinInfo levenshtein(const std::vector<std::string> &ref, const std::vector<std::string> &hyp)
{
    LevenshteinInfo li;

    std::string sRef = "\"" + join(ref, " ") + "\"";
    std::string sHyp = "\"" + join(hyp, " ") + "\"";

    std::string externalEditops = "PYTHONPATH=/home/lonrust/local/lib/python2.7/site-packages python ~/Software/python-Levenshtein/editops.py";
    std::string space = " ";

    std::string functionCall = externalEditops + space + sRef + space + sHyp;
    std::string result = exec(functionCall.c_str());

    auto tokens = delimiterTokeniser(result, '\'');
    li.sub = std::count_if(tokens.begin(), tokens.end(), [](auto s){return s == "replace";});
    li.ins = std::count_if(tokens.begin(), tokens.end(), [](auto s){return s == "insert";});
    li.del = std::count_if(tokens.begin(), tokens.end(), [](auto s){return s == "delete";});

    li.wer = 100*( (li.sub+li.ins+li.del) / (double) ref.size());

    return li;
}

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

double OOV(int count, int oov)
{
	return (count ? 100.0*oov/(double) count : 0.0);
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

SLM::ReferenceId collectHypothesesForReferenceId(SLM::ReferenceId& rId, const std::string& path, bool addPadding)
{
	L_V << "RescoreTools: " << rId.getId() << "\n";

	std::vector<std::string> files = getFilesForReferenceId(rId, path);
	for(auto& file : files)
	{
		L_A << "RescoreTools: \t" << file << " " << path << "\n";
		rId.add(std::make_shared<SLM::AllHypotheses>(file, path, addPadding));
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
