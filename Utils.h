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

bool startsWith(const std::string& s1, const std::string& s2);
bool endsWith(std::string const & value, std::string const & ending);


#endif
