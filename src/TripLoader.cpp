#include "stdafx.h"
#include "TripLoader.h"

TripLoader& TripLoader::getInstance() {
	static TripLoader instance;
	return instance;
}

void TripLoader::write(const std::string& path) {

	auto generator = getInMemoryGenerator(path);

	boost::filesystem::path path_green(generator.path() + "green/");

	if (!exists(path_green) || !is_directory(path_green)) {
		std::cout << path_green << " is a invalid path." << std::endl;
		return;
	}

	for (auto& x : boost::filesystem::directory_iterator(path_green)) {

		std::cout << "File: " << x.path().string() << std::endl;
		std::cout << "\n\t Counting lines... ";

		if (is_directory(x)) continue;

		boost::iostreams::mapped_file_source mmap(x.path().string());
		boost::iostreams::stream<boost::iostreams::mapped_file_source> infile(mmap, std::ios::binary);

		std::string line;

		infile.unsetf(std::ios_base::skipws);

		size_t line_count = std::count(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), '\n') + 1;
		infile.seekg(0, infile.beg);

		//std::string line;
		std::getline(infile, line);

		std::cout << "Done! \n\t Parsing lines... ";

		#pragma omp parallel for private(line)
		for (int l = 0; l < (int)line_count; ++l) {
			#pragma omp critical (read_trips)
			{
				std::getline(infile, line);
			}

			if (line == "") continue;

			auto record = util::split(line, ",");

			try {
				//////////////////////////////////////////////////////////////////////////

				auto datetime_pickup = util::split(record[0], " ");
				if (datetime_pickup.size() != 2) continue;

				auto date_pickup = util::split(datetime_pickup[0], "-");
				auto hour_pickup = util::split(datetime_pickup[1], ":");

				auto datetime_dropoff = util::split(record[1], " ");
				if (datetime_dropoff.size() != 2) continue;

				auto date_dropoff = util::split(datetime_dropoff[0], "-");
				auto hour_dropoff = util::split(datetime_dropoff[1], ":");

				auto day = util::days_from_civil(std::stoi(date_pickup[0]), std::stoi(date_pickup[1]), std::stoi(date_pickup[2]));

				std::tm time = { 0 };
				time.tm_sec = std::stoi(hour_pickup[2]);
				time.tm_min = std::stoi(hour_pickup[1]);
				time.tm_hour = std::stoi(hour_pickup[0]);

				time.tm_mday = std::stoi(date_pickup[2]);
				time.tm_mon = std::stoi(date_pickup[1]) - 1;
				time.tm_year = std::stoi(date_pickup[0]) - 1900;

				auto day_of_week = (util::weekday_from_days(day) + 6) % 7;

				//////////////////////////////////////////////////////////////////////////

				if (record[4].empty() || record[5].empty() || record[6].empty() || record[7].empty()) continue;

				auto lon0 = std::stof(record[4]);
				auto lat0 = std::stof(record[5]);

				auto lon1 = std::stof(record[6]);
				auto lat1 = std::stof(record[7]);

				if (lat0 == 0.f || lon0 == 0.f || lat1 == 0.f || lon1 == 0.f) continue;

				//////////////////////////////////////////////////////////////////////////

				char passegner_bin;
				switch (std::stoi(record[8]))
				{
				case 0:
					continue;
					break;
				case 1:
					passegner_bin = 0;
					break;
				case 2:
					passegner_bin = 1;
					break;
				case 3:
					passegner_bin = 2;
					break;
				default:
					passegner_bin = 3;
					break;
				}

				//////////////////////////////////////////////////////////////////////////

				float distance = std::stof(record[9]);
				if (distance == 0.f) continue;

				char distance_bin;
				if (distance <= 3.f) {
					distance_bin = 0;
				}
				else if (distance <= 6.f) {
					distance_bin = 1;
				}
				else if (distance <= 12.f) {
					distance_bin = 2;
				}
				else {
					distance_bin = 3;
				}

				//////////////////////////////////////////////////////////////////////////

				float total = std::stof(record[16]);
				if (total == 0.f) continue;

				char total_bin;
				if (total <= 10.f) {
					total_bin = 0;
				}
				else if (total <= 20.f) {
					total_bin = 1;
				}
				else if (total <= 40.f) {
					total_bin = 2;
				}
				else {
					total_bin = 3;
				}

				#pragma omp critical (write_trips)
				{
					generator.addFloat("lat0", lat0);
					generator.addFloat("lon0", lon0);
					generator.addFloat("lat1", lat1);
					generator.addFloat("lon1", lon1);
					generator.addInt("day_of_week", day_of_week);
					generator.addInt("hour_of_day", std::stoi(hour_pickup[0]));
					generator.addInt("passenger", passegner_bin);
					generator.addInt("distance", distance_bin);
					generator.addInt("total", total_bin);
					generator.addInt("day", util::mkgmtime(&time));

					generator.execute();
				}
			}
			catch (std::invalid_argument) {
				continue;
			}
		}

		infile->close();
		mmap.close();
		std::cout << "Done!\n" << std::endl;
	}
	

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	boost::filesystem::path path_yellow(generator.path() + "yellow/");

	if (!exists(path_yellow) || !is_directory(path_yellow)) {
		std::cout << path_yellow << " is a invalid path." << std::endl;
		return;
	}

	for (auto& x : boost::filesystem::directory_iterator(path_yellow)) {

		std::cout << "File: " << x.path().string() << std::endl;
		std::cout << "\n\t Counting lines... ";

		if (is_directory(x)) continue;
		
		boost::iostreams::mapped_file_source mmap(x.path().string());
		boost::iostreams::stream<boost::iostreams::mapped_file_source> infile(mmap, std::ios::binary);

		std::string line;

		infile.unsetf(std::ios_base::skipws);

		size_t line_count = std::count(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), '\n') + 1;
		infile.seekg(0, infile.beg);

		//std::string line;
		std::getline(infile, line);

		std::cout << "Done! \n\t Parsing lines... ";

		#pragma omp parallel for private(line)
		for (int l = 0; l < (int)line_count; ++l) {
			#pragma omp critical (read_trips)
			{
				std::getline(infile, line);
			}

			if (line == "") continue;

			auto record = util::split(line, ",");

			if (record.size() < 18) continue;

			try {
				//////////////////////////////////////////////////////////////////////////

				auto datetime_pickup = util::split(record[1], " ");
				if (datetime_pickup.size() != 2) continue;

				auto date_pickup = util::split(datetime_pickup[0], "-");
				auto hour_pickup = util::split(datetime_pickup[1], ":");

				auto datetime_dropoff = util::split(record[2], " ");
				if (datetime_dropoff.size() != 2) continue;

				auto date_dropoff = util::split(datetime_dropoff[0], "-");
				auto hour_dropoff = util::split(datetime_dropoff[1], ":");

				auto day = util::days_from_civil(std::stoi(date_pickup[0]), std::stoi(date_pickup[1]), std::stoi(date_pickup[2]));

				auto day_of_week = (util::weekday_from_days(day) + 6) % 7;

				std::tm time = { 0 };
				time.tm_sec = std::stoi(hour_pickup[2]);
				time.tm_min = std::stoi(hour_pickup[1]);
				time.tm_hour = std::stoi(hour_pickup[0]);

				time.tm_mday = std::stoi(date_pickup[2]);
				time.tm_mon = std::stoi(date_pickup[1]) - 1;
				time.tm_year = std::stoi(date_pickup[0]) - 1900;

				//////////////////////////////////////////////////////////////////////////

				char passegner_bin;
				switch (std::stoi(record[3]))
				{
				case 0:
					continue;
					break;
				case 1:
					passegner_bin = 0;
					break;
				case 2:
					passegner_bin = 1;
					break;
				case 3:
					passegner_bin = 2;
					break;
				default:
					passegner_bin = 3;
					break;
				}

				//////////////////////////////////////////////////////////////////////////

				float distance = std::stof(record[4]);
				if (distance == 0.f) continue;

				char distance_bin;
				if (distance <= 3.f) {
					distance_bin = 0;
				}
				else if (distance <= 6.f) {
					distance_bin = 1;
				}
				else if (distance <= 12.f) {
					distance_bin = 2;
				}
				else {
					distance_bin = 3;
				}

				//////////////////////////////////////////////////////////////////////////

				if (record[5].empty() || record[6].empty() || record[9].empty() || record[10].empty()) continue;

				auto lon0 = std::stof(record[5]);
				auto lat0 = std::stof(record[6]);

				auto lon1 = std::stof(record[9]);
				auto lat1 = std::stof(record[10]);

				if (lat0 == 0.f || lon0 == 0.f || lat1 == 0.f || lon1 == 0.f) continue;

				//////////////////////////////////////////////////////////////////////////

				float total = std::stof(record[17]);
				if (total == 0.f) continue;

				char total_bin;
				if (total <= 10.f) {
					total_bin = 0;
				}
				else if (total <= 20.f) {
					total_bin = 1;
				}
				else if (total <= 40.f) {
					total_bin = 2;
				}
				else {
					total_bin = 3;
				}

				#pragma omp critical (write_trips)
				{
					generator.addFloat("lat0", lat0);
					generator.addFloat("lon0", lon0);
					generator.addFloat("lat1", lat1);
					generator.addFloat("lon1", lon1);
					generator.addInt("day_of_week", day_of_week);
					generator.addInt("hour_of_day", std::stoi(hour_pickup[0]));
					generator.addInt("passenger", passegner_bin);
					generator.addInt("distance", distance_bin);
					generator.addInt("total", total_bin);
					generator.addInt("day", util::mkgmtime(&time));

					generator.execute();
				}
			}
			catch (...) {
				continue;
			}			
		}

		infile->close();
		mmap.close();
		std::cout << "Done!\n" << std::endl;
	}

	generator.commit();
}

InMemoryDataGenerator TripLoader::getInMemoryGenerator(const std::string& path) {
	std::vector<tuple_t> tokens;

	tokens.emplace_back("lat0", "lat0", Spatial);
	tokens.emplace_back("lon0", "lon0", Spatial);
	tokens.emplace_back("lat1", "lat1", Spatial);
	tokens.emplace_back("lon1", "lon1", Spatial);
	tokens.emplace_back("day_of_week", "cat0", Categorical);
	tokens.emplace_back("hour_of_day", "cat1", Categorical);
	tokens.emplace_back("passenger", "cat2", Categorical);
	tokens.emplace_back("distance", "cat3", Categorical);
	tokens.emplace_back("total", "cat4", Categorical);
	tokens.emplace_back("day", "day", Temporal);

	return InMemoryDataGenerator(path, tokens);
}
