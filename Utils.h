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

#include <sstream>

std::vector<std::string> whitespaceTokeniser(std::string const &input);
std::vector<std::string> delimiterTokeniser(const std::string& input, char delimiter);

bool startsWith(const std::string& s1, const std::string& s2);
bool endsWith(std::string const & value, std::string const & ending);

std::string trim(const std::string &s);

template <typename T>
std::string join(const T& v, const std::string& delim) {
	std::ostringstream s;
    for (const auto& i : v) {
        if (&i != &v[0]) {
            s << delim;
        }
        s << i;
    }
    return s.str();
}

#endif
