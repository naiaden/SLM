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
#include <numeric>

#include "Logging.h"
#include "Utils.h"

namespace SLM {

Hypotheses::Hypotheses(const std::string& fileName, const std::string& path) : fileName(fileName), path(path) {
//	std::filesystem::path my_path = ...;
	std::ifstream stream(path.c_str(), std::ios::binary);


	std::string line;
	 if (stream.is_open())
	  {
	    while ( std::getline (stream,line) )
	    {
	    	try
	    	  {
	    		std::shared_ptr<SLM::Hypothesis> h = std::make_shared<Hypothesis>(line);
	    		hypotheses.push_back(h);
	    	  }
	    	  catch (...)
	    	  {
	    	    L_V << "Hypotheses: In file " << fileName << "\n\tignore " << line << "\n";
	    	  }


	    }
	    stream.close();
	  }

	 L_V << "Hypotheses: " << hypotheses.size() << " hypotheses for " << fileName << "\n";

	 startTime = std::stod(delimiterTokeniser(fileName, ':')[1]);
}

Hypotheses::~Hypotheses() {
//	for (auto i : hypotheses)
//		    delete i;
//	hypotheses.clear();
}

std::vector<std::shared_ptr<SLM::Hypothesis>> Hypotheses::getHypotheses() const
{
	return hypotheses;
}

std::string Hypotheses::getFileName() const
{
	return fileName;
}

double Hypotheses::getStartTime() const
{
	return startTime;
}

} /* namespace SLM */
