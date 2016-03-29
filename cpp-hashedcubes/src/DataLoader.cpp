#include "stdafx.h"
#include "DataLoader.h"

std::tuple<std::shared_ptr<HashedCube>, long long, size_t> DataLoader::build(const Schema& schema) {

	size_t pivots_count = 0;

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	
	std::unique_ptr<Data> data = nullptr;

	if (schema.loader == "sql") {
		data = getSQLData(schema);
	} else {
		data = getInMemoryData(schema);
	}
	
	std::cout << "\nBuildind HashedCube: " << std::endl;
	std::cout << "\tName: " << schema.name << std::endl;
	std::cout << "\tSize: " << data->size() << std::endl;
	std::cout << "\tLeaf Threshold: " << schema.leaf << "\n" << std::endl;

	std::shared_ptr<HashedCube>cube = std::make_shared<HashedCube>(data->size());

	response_container current, response;
	current = cube->root();

	start = std::chrono::high_resolution_clock::now();

	// spatial
	pivots_count += cube->setSpatial(schema.spatial, schema.leaf)->hash(current, response, *data.get());
	current.swap(response);
	response.clear();
	std::cout << "\t\tNumber of Pivots: " << current.size() << std::endl;

	// categorical
	for (auto& pair : schema.categorical) {
		pivots_count += cube->addCategorical(pair.first, pair.second)->hash(current, response, *data.get());
		current.swap(response);
		response.clear();
		std::cout << "\t\tNumber of Pivots: " << current.size() << std::endl;
	}
	
	// temporal
	for (auto& pair : schema.temporal) {
		pivots_count += cube->addTemporal(pair.first, pair.second)->hash(current, response, *data.get());
		current.swap(response);
		response.clear();
		std::cout << "\t\tNumber of Pivots: " << current.size() << std::endl;	
	}

	end = std::chrono::high_resolution_clock::now();
	long long duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

	std::cout << "\n\tDuration: "+ std::to_string(duration) + "s\n" << std::endl;

	data->dispose();

	if (schema.loader == "sql") {
		cube->setSQLData(data);
	}
	return std::make_tuple(cube , duration, pivots_count);
}

std::unique_ptr<InMemoryData> DataLoader::getInMemoryData(const Schema& schema) {
	return std::make_unique<InMemoryData>(InMemoryDataDescriptor(schema));
}

std::unique_ptr<SQLData> DataLoader::getSQLData(const Schema& schema) {
	return std::make_unique<SQLData>(schema, InMemoryDataDescriptor(schema));
}
