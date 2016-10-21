#pragma once

#include "InMemoryDataGenerator.h"

namespace util {
	void twitter_sql(const std::vector<Schema>& args) {

		for (auto& schema : args) {
			if (schema.loader != "sql") continue;

			std::vector<tuple_t> tokens;

			tokens.emplace_back("lat0", "lat0", Spatial);
			tokens.emplace_back("lon0", "lon0", Spatial);
			tokens.emplace_back("app", "cat0", Categorical);
			tokens.emplace_back("time", "time", Temporal);

			InMemoryDataGenerator generator(schema.path, tokens);

			std::cout << "SQlite3 version " << SQLITE_VERSION << std::endl;
			std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;

			SQLite::Database    db(schema.path + "db3/log.db3", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
			std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";

			db.exec("PRAGMA synchronous = OFF");
			db.exec("PRAGMA journal_mode = OFF");

			db.exec("DROP TABLE IF EXISTS data");
			db.exec("CREATE TABLE data (id INTEGER PRIMARY KEY, lat0 REAL, lon0 REAL, text TEXT, time INTEGER, app INTEGER)");

			SQLite::Statement query(db, "INSERT INTO data VALUES(?,?,?,?,?,?)");

			boost::filesystem::path boost_path(schema.path);

			if (!exists(boost_path) || !is_directory(boost_path)) {
				std::cout << boost_path << " is a invalid path." << std::endl;
				return;
			}

			std::regex coordinates_regex(R"((-)?[0-9]+\.[0-9]*,(-)?[0-9]+\.[0-9]*)");
			std::regex text_regex(R"("text":"[\w\W]+,"created_at":)");
			std::regex created_regex(R"([\w]{3}\s[\w]{3}\s[\d]{2}\s[\d]{2}:[\d]{2}:[\d]{2}\s\+[\d]{4}\s[\d]{4})");
			std::regex source_regex(R"(\\"nofollow\\">[\w\s]+<\/a>)");

			std::smatch smatch;

			int id = 0;
			for (auto& x : boost::filesystem::directory_iterator(boost_path)) {

				if (is_directory(x)) continue;

				std::ifstream infile(x.path().string());
				std::cout << x.path().string() << std::endl;

				while (!infile.eof()) {
					std::string line;
					std::getline(infile, line);

					try {
						float lat, lon;
						int app;
						std::string text;

						// coordinates
						std::regex_search(line, smatch, coordinates_regex);

						lon = std::stof(util::split(smatch[0].str(), ",")[0]);
						lat = std::stof(util::split(smatch[0].str(), ",")[1]);

						// text
						std::regex_search(line, smatch, text_regex);
						text = smatch[0].str().substr(8, smatch[0].str().size() - 23);

						// time
						std::regex_search(line, smatch, created_regex);
						auto date = util::split(smatch[0], " ");

						ulong d, m, y;
						d = std::stoi(date[2]);
						y = std::stoi(date[5]);

						if (date[1] == "Jan") m = 1;
						else if (date[1] == "Feb") m = 2;
						else if (date[1] == "Mar") m = 3;
						else if (date[1] == "Apr") m = 4;
						else if (date[1] == "May") m = 5;
						else if (date[1] == "Jun") m = 6;
						else if (date[1] == "Jul") m = 7;
						else if (date[1] == "Aug") m = 8;
						else if (date[1] == "Sep") m = 9;
						else if (date[1] == "Oct") m = 10;
						else if (date[1] == "Nov") m = 11;
						else if (date[1] == "Dec") m = 12;

						auto time_ext = util::split(date[3], ":");

						time_t time = util::mkgmtime(y, m, d, std::stoi(time_ext[0]), std::stoi(time_ext[1]), std::stoi(time_ext[2]));

						// app
						std::regex_search(line, smatch, source_regex);

						if (smatch[0].str().empty()) {
							app = 0;
						}
						else {
							std::string app_str(smatch[0].str().substr(13, smatch[0].str().size() - 17));

							if (app_str == "Twitter for Android") app = 1;
							else if (app_str == "Twitter for iPhone") app = 2;
							else if (app_str == "Mobile Web") app = 3;
							else if (app_str == "TweetCaster for Android") app = 4;
							else if (app_str == "TweetCaster for iOS") app = 5;
							else if (app_str == "Twitter for Windows Phone") app = 6;
							else app = 0;
						}


						query.bind(1, id);

						generator.addFloat("lat0", lat);
						query.bind(2, lat);

						generator.addFloat("lon0", lon);
						query.bind(3, lon);

						query.bind(4, text);

						generator.addInt("time", time);
						query.bind(5, (int)time);

						generator.addInt("app", app);
						query.bind(6, app);

						generator.execute();

						query.exec();
						query.reset();
						id += 1;

					}
					catch (...) {
						continue;
					}
				}

				infile.close();
			}
			generator.commit();
		}		
	}
} // namespace util