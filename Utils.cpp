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

bool startsWith(const std::string& s1, const std::string& s2)
{
    return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
}

bool endsWith(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
