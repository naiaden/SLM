/*
 * NBestList.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#include "NBestList.h"

#include <fstream>
#include <iostream>
#include <string>

namespace SLM {

NBestList::NBestList(const std::string& fileName, const std::string& path) : fileName(fileName), path(path) {
//	std::filesystem::path my_path = ...;
	std::ifstream stream(path.c_str(), std::ios::binary);


	std::string line;
	 if (stream.is_open())
	  {
	    while ( std::getline (stream,line) )
	    {
	    	hypotheses.push_back(new Hypothesis(line));
	    }
	    stream.close();
	  }

	 std::cout << "Added " << hypotheses.size() << " hypotheses" << std::endl;

}

NBestList::~NBestList() {
	// TODO Auto-generated destructor stub
}

std::vector<SLM::Hypothesis*> NBestList::getHypotheses() const
{
	return hypotheses;
}

std::string NBestList::getFileName() const
{
	return fileName;
}

} /* namespace SLM */
