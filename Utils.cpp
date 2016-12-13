/*
 * Utils.cpp
 *
 *  Created on: Dec 13, 2016
 *      Author: onrust
 */

#include "Utils.h"

#include <vector>
#include <string>
#include <sstream>

#include <iterator>

std::vector<std::string> whitespaceTokeniser(std::string const &input) {
    std::istringstream buffer(input);
    std::vector<std::string> ret{std::istream_iterator<std::string>(buffer),
                                 std::istream_iterator<std::string>()};
    return ret;
}

std::vector<std::string> delimiterTokeniser(const std::string& input, char delimiter)
{
	std::vector<std::string> strings;
	std::istringstream buffer(input);
	std::string token;
	while (getline(buffer, token, delimiter)) {
		strings.push_back(token);
	}
	return strings;
}

