#pragma once

#include "DataLoader.h"
#include "InMemoryDataGenerator.h"

class DMPLoader : public DataLoader {
public:
	static DMPLoader& getInstance();
	virtual void write(const std::string& path) override;

protected:
	virtual InMemoryDataGenerator getInMemoryGenerator(const std::string& path) override;

private:
	DMPLoader() = default;
	~DMPLoader() = default;
};
