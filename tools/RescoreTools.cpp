//============================================================================
// Name        : RescoreTools.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <string>
#include <iostream>
#include <vector>
#include <experimental/filesystem>

#include <unordered_map>
#include <numeric>

#include "Logging.h"

#include "Utils.h"
#include "ReferenceId.h"
#include "Sorter.h"
#include "FileWriter.h"
#include "FileReader.h"
#include "TextPreprocessor.h"
#include "ToolProgramOptions.h"

// grid search optimal parameters
// best hyp
// oracle ceiling

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

SLM::ReferenceId collectHypothesesForReferenceId(SLM::ReferenceId& rId, const std::string& path)
{
	L_V << "RescoreTools: " << rId.getId() << "\n";

	for (auto & p : std::experimental::filesystem::directory_iterator(path))
	{
		std::string fileName = delimiterTokeniser(p.path(), '/').back();
		if(startsWith(fileName, rId.getId()))
		{
			L_A << "RescoreTools: \t" << fileName << "\n";
			rId.add(new SLM::Hypotheses(fileName, p.path()));
		}
	}

	L_V << "RescoreTools: " << rId.getNBestLists().size() << " hypotheses for " << rId.getId() << "\n" << std::endl;

	return rId;
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




int main(int argc, char** argv)
{
	SLM::ToolProgramOptions po(argc, argv);

	SLM::Sorter* sorter = po.getSorter();

	SLM::CGNTextPreprocessor tpp;

    std::unordered_map<std::string, SLM::ReferenceId> referenceIds = collectReferenceIds(po.getInputPath());

    std::vector<double> globalWER;

    if(po.getLimitedReferenceIds().size())
    {
    	std::unordered_map<std::string, SLM::ReferenceId> subsetReferenceIds;
    	for(auto & r : po.getLimitedReferenceIds())
    	{
    		std::unordered_map<std::string,SLM::ReferenceId>::const_iterator rIt = referenceIds.find (r);

    		  if ( rIt != referenceIds.end() )
    			  subsetReferenceIds.emplace(r, rIt->second);
    	}

    	referenceIds = subsetReferenceIds;
    }

    for (auto & r : referenceIds)
    {
    	collectHypothesesForReferenceId(r.second, po.getInputPath());

    	if(po.isProgramMode(SLM::ProgramMode::SELECTBEST))
		{
    		SLM::ReferenceFileWriter fw(po.getOutputPath(), r.first + "-" + sorter->getName());
    		SLM::ReferenceFileReader fr(po.getReferencePath(), r.first + ".stm");

    		std::vector<std::string> reference = fr.getTokens();
    		std::vector<std::string> hypothesisTokens;


    		for(auto & nbl : r.second.getNBestLists())
    		{
    			if(nbl->getHypotheses().size())
    			{
					std::vector<std::string> f = sorter->sort(*nbl).getTokens();
					hypothesisTokens.insert(std::end(hypothesisTokens), std::begin(f), std::end(f));
    			}
    		}

    		double localWER = WER(reference, tpp.removeFillers(hypothesisTokens, true));
    		globalWER.push_back(localWER);
    		std::cout << r.first << "\t" << localWER << std::endl;

		}
    }

    std::cout << "WER: " << std::accumulate( globalWER.begin(), globalWER.end(), 0.0)/globalWER.size();



}
