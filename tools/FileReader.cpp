/*
 * FileReader.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#include "FileReader.h"

#include <vector>
#include <iostream>

#include "Utils.h"

namespace SLM {

FileReader::FileReader(const std::string& inputDir, const std::string& fileName) {
	file.open(inputDir + "/" + fileName);

}

FileReader::~FileReader() {
	file.close();
}



////////////////////////

ReferenceFileReader::ReferenceFileReader(const std::string& inputDir, const std::string& fileName)
	: FileReader(inputDir, fileName)
{
	std::string line;
	if (file.is_open())
	{
		while ( getline (file,line) )
		{ // std::vector<std::string>(entries.begin() + 3, entries.end()) ;
			std::vector<std::string> lineTokens = whitespaceTokeniser(line);

			// 6:: fv601210 1 V60018 4.170 5.660 DUMMY uh Stevaert
			lineTokens = std::vector<std::string>(lineTokens.begin() + 6, lineTokens.end()) ;
			tokens.insert(std::end(tokens), std::begin(lineTokens), std::end(lineTokens));
		}
		file.close();
	}
}

ReferenceFileReader::~ReferenceFileReader()
{
}

std::vector<std::string> ReferenceFileReader::getTokens() const
{
	return tokens;
}

} /* namespace SLM */
