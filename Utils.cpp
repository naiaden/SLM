/*
 * Utils.cpp
 *
 *  Created on: Dec 11, 2016
 *      Author: louis
 */




std::vector<std::string> whitespaceTokeniser(std::string const &input) {
    std::istringstream buffer(input);
    std::vector<std::string> ret{std::istream_iterator<std::string>(buffer),
                                 std::istream_iterator<std::string>()};
    return ret;
}
