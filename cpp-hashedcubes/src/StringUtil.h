#pragma once

namespace util {
	std::vector<std::string> split(const std::string& input, const std::regex& regex);
	std::vector<std::string> split(const std::string& input, const std::string& str_regex);
}
