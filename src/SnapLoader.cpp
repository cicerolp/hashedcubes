#include "stdafx.h"
#include "SnapLoader.h"

SnapLoader& SnapLoader::getInstance() {
	static SnapLoader instance;
	return instance;
}

void SnapLoader::write(const std::string& path) {

	auto generator = getInMemoryGenerator(path);

	boost::filesystem::path boost_path(generator.path());

	if (!exists(boost_path) || !is_directory(boost_path)) {
		std::cout << boost_path << " is a invalid path." << std::endl;
		return;
	}

	for (auto& x : boost::filesystem::directory_iterator(boost_path)) {

		if (is_directory(x)) continue;

		std::ifstream infile(x.path().string());

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

			if (line == "") continue;

			auto record = util::split(line, "[\t]+");

			if (record.size() != 5) continue;

			float lat = std::stof(record[2]);
			float lon = std::stof(record[3]);

			if (lat < -90.f || lat > 90.f) continue;
			if (lon < -180.f || lat > 180.f) continue;

			if (lat == 0.f && lon == 0.f) continue;

			auto datetime = util::split(record[1], "T");
			
			if (datetime.size() != 2) continue;

			auto date = util::split(datetime[0], "-");
			auto hour = util::split(datetime[1], ":");

			if (date.size() != 3) continue;

			int days = util::days_from_civil(std::stoi(date[0]), std::stoi(date[1]), std::stoi(date[2]));

			std::tm time = { 0 };
			time.tm_sec = std::stoi(hour[2]);
			time.tm_min = std::stoi(hour[1]);
			time.tm_hour = std::stoi(hour[0]);

			time.tm_mday = std::stoi(date[2]);
			time.tm_mon = std::stoi(date[1]) - 1;
			time.tm_year = std::stoi(date[0]) - 1900;

			#pragma omp critical (write_snap)
			{
				generator.addFloat("lat0", lat);
				generator.addFloat("lon0", lon);

				generator.addInt("hour_of_day", std::stoi(hour[0]));
				generator.addInt("day_of_week", (util::weekday_from_days(days) + 6) % 7);
				generator.addInt("month_of_year", std::stoi(date[1]) - 1);
				generator.addInt("day_of_month", std::stoi(date[2]) - 1);
				
				generator.addInt("tseries", util::mkgmtime(&time));

				generator.execute();
			}
		}

		infile.close();
	}

	generator.commit();
}

InMemoryDataGenerator SnapLoader::getInMemoryGenerator(const std::string& path) {
	std::vector<tuple_t> tokens;

	tokens.emplace_back("lat0", "lat0", Spatial);
	tokens.emplace_back("lon0", "lon0", Spatial);


	tokens.emplace_back("hour_of_day", "cat0", Categorical);
	tokens.emplace_back("day_of_week", "cat1", Categorical);

	tokens.emplace_back("month_of_year", "cat2", Categorical);
	tokens.emplace_back("day_of_month", "cat3", Categorical);
	
	tokens.emplace_back("tseries", "tseries", Temporal);

	return InMemoryDataGenerator(path, tokens);
}
