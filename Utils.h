/*
 * Utils.cpp
 *
 *  Created on: Dec 11, 2016
 *      Author: louis
 */


#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <string>

std::vector<std::string> whitespaceTokeniser(std::string const &input);

std::vector<std::string> delimiterTokeniser(const std::string& input, char delimiter);

#endif
