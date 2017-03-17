/*
 * FileReader.h
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#ifndef TOOLS_FILEREADER_H_
#define TOOLS_FILEREADER_H_

#include <fstream>
#include <vector>
#include <string>

namespace SLM {

class FileReader {
public:

	FileReader(const std::string& inputDir, const std::string& fileName);
	FileReader(const std::string& fileName);
	virtual ~FileReader();

protected:
	std::string inputDir;
	std::string fileName;
	std::ifstream file;
};

class ReferenceFileReader : public FileReader
{
public:
	ReferenceFileReader(const std::string& inputDir, const std::string& fileName);
	virtual ~ReferenceFileReader();

	std::vector<std::string> getTokens() const;
protected:
	std::vector<std::string> tokens;
};

} /* namespace SLM */

#endif /* TOOLS_FILEREADER_H_ */
