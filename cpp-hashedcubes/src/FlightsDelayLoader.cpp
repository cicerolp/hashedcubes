#include "stdafx.h"
#include "FlightsDelayLoader.h"

FlightsDelayLoader& FlightsDelayLoader::getInstance() {
	static FlightsDelayLoader instance;
	return instance;
}

void FlightsDelayLoader::write(const std::string& path) {

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

				if (record.size() < 17) continue;

				int year_bin = std::stoi(record[0]) - 1987;
				int day_of_week_bin = std::stoi(record[3]) - 1;

				int arr_delay = std::stoi(record[14]);
				int dep_delay = std::stoi(record[15]);

				if (arr_delay < 0 || dep_delay < 0) continue;

				int arr_delay_bin = std::floor(arr_delay / 15.f);
				arr_delay_bin = arr_delay_bin > 173 ? 173 : arr_delay_bin;

				int dep_delay_bin = std::floor(dep_delay / 15.f);
				dep_delay_bin = dep_delay_bin > 173 ? 173 : dep_delay_bin;

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

				#pragma omp critical (write_snap)
				{
					generator.addInt("carrier"    , carrier_bin);
					generator.addInt("year"       , year_bin);
					generator.addInt("day_of_week", day_of_week_bin);
					generator.addInt("dep_delay"  , dep_delay_bin);
					generator.addInt("arr_delay"  , arr_delay_bin);
					
					generator.execute();
				}

			}
			catch (std::invalid_argument) {
				continue;
			}
		}

		infile.close();
	}

	generator.commit();
}

InMemoryDataGenerator FlightsDelayLoader::getInMemoryGenerator(const std::string& path) {
	std::vector<tuple_t> tokens;

	tokens.emplace_back("carrier"    , "cat0", Categorical);
	tokens.emplace_back("year"       , "cat1", Categorical);
	tokens.emplace_back("day_of_week", "cat2", Categorical);
	tokens.emplace_back("dep_delay"  , "cat3", Categorical);
	tokens.emplace_back("arr_delay"  , "cat4", Categorical);

	return InMemoryDataGenerator(path, tokens);
}
