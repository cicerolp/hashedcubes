#include "stdafx.h"
#include "DMPLoader.h"

DMPLoader& DMPLoader::getInstance() {
	static DMPLoader instance;
	return instance;
}

void DMPLoader::write(const std::string& path) {

	auto generator = getInMemoryGenerator(path);
	
	struct tweet {
		float latitude;
		float longitude;
		uint64_t time;
		uint8_t language;
		uint8_t device;
		uint8_t app;
	};
	
	boost::filesystem::path boost_path(generator.path());

	if (!exists(boost_path) || !is_directory(boost_path)) {
		std::cout << boost_path << " is a invalid path." << std::endl;
		return;
	}

	for (auto& x : boost::filesystem::directory_iterator(boost_path)) {

		if (is_directory(x)) continue;

		std::ifstream infile(x.path().string(), std::ios::binary);

		std::cout << x.path().string() << std::endl;

		infile.unsetf(std::ios_base::skipws);

		for (int i = 0; i < 32; ++i) {
			infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		while (!infile.eof()) {
			try {
				tweet record;
				infile.read((char*)&record, 19);

				time_t time(record.time);
				struct tm * ptm = std::gmtime(&time);

				generator.addFloat("lat0", record.latitude);
				generator.addFloat("lon0", record.longitude);

				generator.addInt("language", record.language);
				generator.addInt("device", record.device);
				generator.addInt("app", record.app);

				generator.addInt("tseries", util::mkgmtime(ptm));

				generator.execute();
			} catch (...) {
				break;
			}						
		}
		
		infile.close();
	}

	generator.commit();
}

InMemoryDataGenerator DMPLoader::getInMemoryGenerator(const std::string& path) {
	std::vector<tuple_t> tokens;

	tokens.emplace_back("lat0", "lat0", Spatial);
	tokens.emplace_back("lon0", "lon0", Spatial);

	tokens.emplace_back("language", "cat0", Categorical);
	tokens.emplace_back("device"  , "cat1", Categorical);
	tokens.emplace_back("app"     , "cat2", Categorical);

	tokens.emplace_back("tseries", "tseries", Temporal);

	return InMemoryDataGenerator(path, tokens);
}
