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
#include "Sorter.h"

namespace SLM {

Hypotheses::Hypotheses(const std::string& fileName, const std::string& path) : fileName(fileName), path(path)
{
	startTime = std::stod(delimiterTokeniser(fileName, ':')[1]);
}

Hypotheses::~Hypotheses()
{

}

std::string Hypotheses::getFileName() const
{
	return fileName;
}

double Hypotheses::getStartTime() const
{
	return startTime;
}

AllHypotheses::AllHypotheses(const std::string& fileName, const std::string& path) : SLM::Hypotheses(fileName, path) {
	std::string fullPath = path + "/" + fileName;
	std::ifstream stream(fullPath.c_str(), std::ios::binary);

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
}

AllHypotheses::~AllHypotheses() {
}

std::vector<std::shared_ptr<SLM::Hypothesis>> AllHypotheses::getHypotheses() const
{
	return hypotheses;
}

BestHypotheses::BestHypotheses(const std::string& fileName, const std::string& path, SLM::Sorter* sorter)
	: Hypotheses(fileName, path)
{
	std::string fullPath = path + "/" + fileName;
	std::ifstream stream(fullPath.c_str(), std::ios::binary);

	std::cout << fileName << std::endl;

	std::string line;
	if (stream.is_open())
	{
		while ( std::getline (stream,line) )
		{
			try
			{
				std::shared_ptr<SLM::PartialHypothesis> h = std::make_shared<PartialHypothesis>(line);

				if(!bestHypothesis) // nullptr
				{
					bestHypothesis = h;
				}

				if(sorter->compare(bestHypothesis, h))
				{
					std::cout << h->getLanguageModelScore() << " is better than " << bestHypothesis->getLanguageModelScore() << std::endl;
					bestHypothesis = h;

				}

			}
			catch (...)
			{
				L_V << "Hypotheses: In file " << fileName << "\n\tignore " << line << "\n";
			}
		}
		stream.close();
	}
}

BestHypotheses::~BestHypotheses()
{

}


} /* namespace SLM */
