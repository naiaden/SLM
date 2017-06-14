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

#include <cctype>
#include <algorithm>

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


std::string trim(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}
