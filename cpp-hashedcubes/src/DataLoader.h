#pragma once

#include "HashedCube.h"

#include "Data.h"
#include "DataGenerator.h"

#include "SQLData.h"

#include "InMemoryData.h"
#include "InMemoryDataGenerator.h"

class DataLoader {
public:
	static std::tuple<std::shared_ptr<HashedCube>, long long, size_t> build(const Schema& schema);
	virtual void write(const std::string& path) = 0;
		
protected:
	static std::unique_ptr<InMemoryData> getInMemoryData(const Schema& schema);
	static std::unique_ptr<SQLData> getSQLData(const Schema& schema);

	virtual InMemoryDataGenerator getInMemoryGenerator(const std::string& path) = 0;
};
