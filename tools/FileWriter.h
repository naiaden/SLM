/*
 * FileWriter.h
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#ifndef FILEWRITER_H_
#define FILEWRITER_H_

#include <fstream>

namespace SLM {

class FileWriter {
public:
	FileWriter(const std::string& outputDir, const std::string& fileName);
	virtual ~FileWriter();


protected:
	std::string outputDir;
	std::string fileName;
	std::ofstream file;
};

class GenericFileWriter : public FileWriter
{
public:
	GenericFileWriter(const std::string& outputDir, const std::string& fileName);
	virtual ~GenericFileWriter();

	void addLine(const std::string& line);
protected:
};

class ReferenceFileWriter : public FileWriter
{
public:
	ReferenceFileWriter(const std::string& outputDir, const std::string& fileName);
	virtual ~ReferenceFileWriter();

	void addLine(const std::string& line);
protected:
	bool isFirstLine = true;
};

} /* namespace SLM */

#endif /* FILEWRITER_H_ */
