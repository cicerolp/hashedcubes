#pragma once

class FlightsLoader {
public:
	FlightsLoader() = default;
	~FlightsLoader() = default;

protected:	
	inline void loadMap(const std::string& path);
	std::unordered_map<std::string, std::pair<float, float>> map;
};

void FlightsLoader::loadMap(const std::string & pathStr) {
	boost::filesystem::path path(pathStr + "csv/");

	if (!exists(path) || !is_directory(path)) {
		std::cout << path << " is a invalid path." << std::endl;
		return;
	}

	for (auto& x : boost::filesystem::directory_iterator(path)) {

		if (is_directory(x)) continue;

		std::ifstream infile(x.path().string());

		std::string line;
		while (std::getline(infile, line)) {
			if (line == "") continue;

			auto record = util::split(line, ",");

			try {
				if (record[4] == "\"\"") continue;
				map[record[4].substr(1, record[4].size() - 2)] = { std::stof(record[6]), std::stof(record[7]) };
			}
			catch (std::invalid_argument) {
				continue;
			}
		}

		infile.close();
	}
}