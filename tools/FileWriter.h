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

	void addLine(const std::string& line);
protected:
	std::string outputDir;
	std::string fileName;
	std::ofstream file;
};

} /* namespace SLM */

#endif /* FILEWRITER_H_ */
