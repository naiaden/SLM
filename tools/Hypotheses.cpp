/*
 * NBestList.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: louis
 */

#include "Hypotheses.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Logging.h"

namespace SLM {

Hypotheses::Hypotheses(const std::string& fileName, const std::string& path) : fileName(fileName), path(path) {
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

	 L_V << "Hypotheses: " << hypotheses.size() << " hypotheses\n";

}

Hypotheses::~Hypotheses() {
	// TODO Auto-generated destructor stub
}

std::vector<SLM::Hypothesis*> Hypotheses::getHypotheses() const
{
	return hypotheses;
}

std::string Hypotheses::getFileName() const
{
	return fileName;
}

} /* namespace SLM */
