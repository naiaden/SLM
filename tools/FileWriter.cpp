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

void FileWriter::addLine(const std::string& line)
{
	  file << line;
}

} /* namespace SLM */
