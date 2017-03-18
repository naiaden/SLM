/*
 * FileWriter.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: louis
 */

#include "FileWriter.h"

#include <fstream>

namespace SLM {

FileWriter::FileWriter(const std::string& outputDir, const std::string& fileName) {
	file.open(outputDir + "/" + fileName);
}

FileWriter::~FileWriter() {
	file.close();
}

////////////////////////

GenericFileWriter::GenericFileWriter(const std::string& outputDir, const std::string& fileName)
	: FileWriter(outputDir, fileName)
{

}

GenericFileWriter::~GenericFileWriter()
{
}

void GenericFileWriter::addLine(const std::string& line)
{
	file << line << "\n";
}


////////////////////////

ReferenceFileWriter::ReferenceFileWriter(const std::string& outputDir, const std::string& fileName)
	: FileWriter(outputDir, fileName)
{

}

ReferenceFileWriter::~ReferenceFileWriter()
{
}

void ReferenceFileWriter::addLine(const std::string& line)
{
	if(!isFirstLine)
	{
		file << " ";
	}

	file << line;

	isFirstLine = false;
}

} /* namespace SLM */
