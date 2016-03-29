#pragma once

namespace util {
	struct Translate {
		Translate() = default;
		Translate(std::string _name, std::vector<std::string> _values) : name(_name), values(_values) {};

		ulong find(const std::string& str) const {
			auto iter = std::find(values.begin(), values.end(), str);
			return ulong(iter - values.begin());
		}

		std::string name;
		std::vector<std::string> values;
	};

	struct Nanocube {
		std::string instance;
		std::string tseries;
		ulong from_bin, to_bin, bucket_size;
		std::unordered_map<std::string, Translate> schemata;
	};

	inline void nanocubes_log_tseries(const Nanocube& cube, Query& query, const std::string& from, const std::string& to) {
		ulong to_days;
		ulong from_days = (cube.bucket_size * std::stoi(from)) + cube.from_bin;

		if (to == "10000000000") to_days = cube.to_bin;
		else to_days = (cube.bucket_size * (std::stoi(to) + std::stoi(from))) + cube.from_bin;

		query.tseries.emplace(cube.tseries, TemporalInterval(from_days, to_days));
	}

	inline void nanocubes_log_fields(const Nanocube& cube, Query& query, const std::string& str) {

		auto fields = util::split(str, ";");
		for (auto& clause : fields) {
			auto literals = util::split(clause, std::regex("=|\\|"));
			std::vector<int> values;
			for (size_t i = 1; i < literals.size(); i++) {
				values.emplace_back(cube.schemata.at(literals[0]).find(literals[i]));				
			}
            std::sort(values.begin(), values.end());
			query.emplaceWhere(cube.schemata.at(literals[0]).name, values);
		}		
	}

	inline std::vector<Query> nanocubes_log(const std::vector<std::string>& files) {
		Nanocube flights, brightkite, gowalla, twitter;

		// flights
		flights.instance = "performance";
		flights.tseries = "tseries";
		flights.from_bin = util::mkgmtime(1987, 1, 1);
		flights.to_bin = util::mkgmtime(2009, 1, 1);
		flights.bucket_size = 14400;
		flights.schemata.insert({ "ontime", Translate("dep_delay",{
			"61 _min_early",
			"31_60_min_early",
			"16_30_min_early",
			"6_15_min_early",
			"5_min_earlylate",
			"6_15_min_late",
			"16_30_min_late",
			"31_60_min_late",
			"61 _min_late" }) });
		flights.schemata.insert({ "carrier", Translate("carrier",{ "Pacific_Southwest", "TWA", "United", "Southwest", "Eastern", "America_West", "Northwest",
			"Pan_Am", "Piedmont", "Continental", "Delta", "American", "US_Air", "Alaska", "Midway", "Aloha", "American_Eagle", "Skywest", "Expressjet", "ATA", "Altantic_Southest", "AirTran", "JetBlue", "Independence", "Hawaiian", "Comair", "Frontier", "Mesa", "Pinnacle" }) });

		// brightkite
		brightkite.instance = "brightkite";
		brightkite.tseries = "tseries";
		brightkite.from_bin = util::mkgmtime(2008, 1, 1);
		brightkite.to_bin = util::mkgmtime(2010, 12, 1);
		brightkite.bucket_size = 604800;
		brightkite.schemata.insert({ "day_of_week", Translate("day_of_week",{ "Mon", "Tue", "Wed", "Thu", "Fri", "Sat","Sun" }) });
		brightkite.schemata.insert({ "hour_of_day", Translate("hour_of_day",{ "00", "01", "02", "03", "04", "05", "06",
			"07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23" }) });

		// gowalla
		gowalla.instance = "gowalla";
		gowalla.tseries = "tseries";
		gowalla.from_bin = util::mkgmtime(2008, 1, 1);
		gowalla.to_bin = util::mkgmtime(2010, 12, 1);
		gowalla.bucket_size = 604800;
		gowalla.schemata.insert({ "dayofweek", Translate("day_of_week",{ "Mon", "Tue", "Wed", "Thu", "Fri", "Sat","Sun" }) });
		gowalla.schemata.insert({ "hour", Translate("hour_of_day",{ "0", "1", "2", "3", "4", "5", "6",
			"7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23" }) });

		// twitter
		twitter.instance = "twitter-small";
		twitter.tseries = "tseries";
		twitter.from_bin = util::mkgmtime(2011, 11, 1);
		twitter.to_bin = util::mkgmtime(2012, 11, 1);
		twitter.bucket_size = 14400;
		twitter.schemata.insert({ "device", Translate("device", { "none", "iphone", "android", "ipad", "windows" }) });


		std::unordered_map<std::string, Nanocube> nanocubes{ { "13", flights },{ "14", brightkite },{ "12", gowalla },{ "15", twitter } };

		std::vector<Query> queries;

		for (auto& file : files) {
			std::ifstream infile(file);

			while (!infile.eof()) {

				std::string line;
				std::getline(infile, line);

				try {
					if (line.empty()) continue;

					auto record = util::split(line, ",");

					if (record.size() != 3) continue;

					auto url = util::split(record[0], "/");

					if (url[5] == "tile") {
						Query query(nanocubes[url[4]].instance, url[5]);

						query.zoom = std::stoul(url[6]);
						query.resolution = std::stoul(url[7]);

						query.emplaceTile(0, Tile(std::stoi(url[8]), std::stoi(url[9]), query.zoom));

						nanocubes_log_tseries(nanocubes[url[4]], query, url[10], url[11]);

						if (url.size() > 12) nanocubes_log_fields(nanocubes[url[4]], query, url[12]);

						queries.emplace_back(query);
					}
					else if (url[5] == "query") {
						Query query(nanocubes[url[4]].instance, url[6]);

						if (url[7] == "undefined") query.zoom = 1;
						else query.zoom = std::stoul(url[7]);

						query.emplaceRegion(0, TileBounds({ std::stoi(url[8]), std::stoi(url[9]), query.zoom }, { std::stoi(url[10]), std::stoi(url[11]), query.zoom }));

						if (url.size() == 18) {
							nanocubes_log_fields(nanocubes[url[4]], query, url[13]);
							nanocubes_log_tseries(nanocubes[url[4]], query, url[15], url[16]);
						}
						else if (url.size() > 13) {
							if (url[12] == "where") nanocubes_log_fields(nanocubes[url[4]], query, url[13]);
							else if (url[12] == "tseries") nanocubes_log_tseries(nanocubes[url[4]], query, url[13], url[14]);
						}

						queries.emplace_back(query);						
					}
				}
				catch (std::invalid_argument) {
				}
			}
			infile.close();
		}

		return queries;
	}
} // namespace util


