//============================================================================
// Name        : RescoreTools.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <string>
#include <iostream>
#include <experimental/filesystem>

#include <unordered_map>

#include "Utils.h"
#include "ReferenceId.h"
#include "Sorter.h"
#include "FileWriter.h"

// grid search optimal parameters
// best hyp
// oracle ceiling

int main()
{
    std::string path = "/home/louis/data/slm/nbest";
    std::string outputPath = "/home/louis/data/slm/nbest-output";

    std::unordered_map<std::string, SLM::ReferenceId> referenceIds;

    for (auto & p : std::experimental::filesystem::directory_iterator(path))
    {
    	std::string fileName = delimiterTokeniser(p.path(), '/').back();
    	std::string fileId = delimiterTokeniser(fileName, '.').front();

    	referenceIds.emplace (fileId, SLM::ReferenceId(fileId));
    }

    std::cout << referenceIds.size() << " references found" << std::endl;

    for (auto & r : referenceIds)
    {
    	std::cout << r.first << std::endl;
    	for (auto & p : std::experimental::filesystem::directory_iterator(path))
		{
			std::string fileName = delimiterTokeniser(p.path(), '/').back();
			if(startsWith(fileName, r.first))
			{
				std::cout << " " << fileName << std::endl;
				r.second.add(new SLM::NBestList(fileName, p.path()));
			}
		}

    	std::cout << r.second.getNBestLists().size() << std::endl;

    	// if get best hypothesis per nbest file
		{
    		SLM::FileWriter fw(outputPath, r.first);


    		SLM::AcousticSorter sorter;

    		for(auto & nbl : r.second.getNBestLists())
    		{
    			std::cout << "!!!\t\t" << nbl->getFileName() << "\t\t" << sorter.sort(*nbl).getAcousticScore() << "\n" << std::endl;
    			fw.addLine(sorter.sort(*nbl).getSentence() + "\n");
    		}
		}
    }




}
