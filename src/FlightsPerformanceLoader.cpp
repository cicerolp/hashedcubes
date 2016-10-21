#include "stdafx.h"
#include "FlightsPerformanceLoader.h"

FlightsPerformanceLoader& FlightsPerformanceLoader::getInstance() {
	static FlightsPerformanceLoader instance;
	return instance;
}

void FlightsPerformanceLoader::write(const std::string& path) {

	auto generator = getInMemoryGenerator(path);

	boost::filesystem::path boost_path(generator.path());

	if (!exists(boost_path) || !is_directory(boost_path)) {
		std::cout << boost_path << " is a invalid path." << std::endl;
		return;
	}

	loadMap(generator.path());

	for (auto& x : boost::filesystem::directory_iterator(boost_path)) {

		if (is_directory(x)) continue;

		std::ifstream infile(x.path().string());

		std::cout << x.path().string() << std::endl;

		infile.unsetf(std::ios_base::skipws);

		size_t line_count = std::count(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), '\n') + 1;
		infile.seekg(0, infile.beg);

		#pragma omp parallel for
		for (int l = 0; l < (int)line_count; ++l) {
			std::string line;

			#pragma omp critical (read_snap)
			{
				std::getline(infile, line);
			}
			try {
				if (line == "") continue;

				auto record = util::split(line, ",");

				if (record.size() < 18) continue;

				if (map.find(record[16]) == map.end() || map.find(record[17]) == map.end()) continue;

				int dep_delay = std::stoi(record[15]);
				int dep_delay_bin = 0;

				if (dep_delay >= 61) {
					dep_delay_bin = 0;
				}
				else if (dep_delay >= 31) {
					dep_delay_bin = 1;
				}
				else if (dep_delay >= 16) {
					dep_delay_bin = 2;
				}
				else if (dep_delay >= 6) {
					dep_delay_bin = 3;
				}
				else if (dep_delay >= -5) {
					dep_delay_bin = 4;
				}
				else if (dep_delay >= -15) {
					dep_delay_bin = 5;
				}
				else if (dep_delay >= -30) {
					dep_delay_bin = 6;
				}
				else if (dep_delay >= -60) {
					dep_delay_bin = 7;
				}
				else {
					dep_delay_bin = 8;
				}

				auto carrier = record[8];
				int carrier_bin = 0;

				if (carrier.compare("9E") == 0) {
					carrier_bin = 28;
				}
				else if (carrier.compare("AA") == 0) {
					carrier_bin = 11;
				}
				else if (carrier.compare("AQ") == 0) {
					carrier_bin = 15;
				}
				else if (carrier.compare("AS") == 0) {
					carrier_bin = 13;
				}
				else if (carrier.compare("B6") == 0) {
					carrier_bin = 22;
				}
				else if (carrier.compare("CO") == 0) {
					carrier_bin = 9;
				}
				else if (carrier.compare("DH") == 0) {
					carrier_bin = 23;
				}
				else if (carrier.compare("DL") == 0) {
					carrier_bin = 10;
				}
				else if (carrier.compare("EA") == 0) {
					carrier_bin = 4;
				}
				else if (carrier.compare("EV") == 0) {
					carrier_bin = 20;
				}
				else if (carrier.compare("F9") == 0) {
					carrier_bin = 26;
				}
				else if (carrier.compare("FL") == 0) {
					carrier_bin = 21;
				}
				else if (carrier.compare("HA") == 0) {
					carrier_bin = 24;
				}
				else if (carrier.compare("HP") == 0) {
					carrier_bin = 5;
				}
				else if (carrier.compare("ML (1)") == 0) {
					carrier_bin = 14;
				}
				else if (carrier.compare("MQ") == 0) {
					carrier_bin = 16;
				}
				else if (carrier.compare("NW") == 0) {
					carrier_bin = 6;
				}
				else if (carrier.compare("OH") == 0) {
					carrier_bin = 25;
				}
				else if (carrier.compare("OO") == 0) {
					carrier_bin = 17;
				}
				else if (carrier.compare("PA (1)") == 0) {
					carrier_bin = 7;
				}
				else if (carrier.compare("PI") == 0) {
					carrier_bin = 8;
				}
				else if (carrier.compare("PS") == 0) {
					carrier_bin = 0;
				}
				else if (carrier.compare("TW") == 0) {
					carrier_bin = 1;
				}
				else if (carrier.compare("TZ") == 0) {
					carrier_bin = 19;
				}
				else if (carrier.compare("UA") == 0) {
					carrier_bin = 2;
				}
				else if (carrier.compare("US") == 0) {
					carrier_bin = 12;
				}
				else if (carrier.compare("WN") == 0) {
					carrier_bin = 3;
				}
				else if (carrier.compare("XE") == 0) {
					carrier_bin = 18;
				}
				else if (carrier.compare("YV") == 0) {
					carrier_bin = 27;
				}

				std::tm time = { 0 };
				time.tm_mday = std::stoi(record[2]);
				time.tm_mon = std::stoi(record[1]) - 1;
				time.tm_year = std::stoi(record[0]) - 1900;

				#pragma omp critical (write_snap)
				{
					generator.addFloat("lat0", map.at(record[16]).first);
					generator.addFloat("lon0", map.at(record[16]).second);
					generator.addFloat("lat1", map.at(record[17]).first);
					generator.addFloat("lon1", map.at(record[17]).second);

					generator.addInt("dep_delay", dep_delay_bin);
					generator.addInt("carrier"  , carrier_bin);

					generator.addInt("tseries", util::mkgmtime(&time));

					generator.execute();
				}

			} catch (std::invalid_argument) {
				continue;
			}
		}

		infile.close();
	}

	generator.commit();
}

InMemoryDataGenerator FlightsPerformanceLoader::getInMemoryGenerator(const std::string& path) {
	std::vector<tuple_t> tokens;

	tokens.emplace_back("lat0", "lat0", Spatial);
	tokens.emplace_back("lon0", "lon0", Spatial);
	tokens.emplace_back("lat1", "lat1", Spatial);
	tokens.emplace_back("lon1", "lon1", Spatial);

	tokens.emplace_back("dep_delay", "cat0", Categorical);
	tokens.emplace_back("carrier"  , "cat1", Categorical);

	tokens.emplace_back("tseries", "tseries", Temporal);

	return InMemoryDataGenerator(path, tokens);
}
