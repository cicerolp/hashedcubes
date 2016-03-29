#pragma once

#include "DataLoader.h"
#include "InMemoryDataGenerator.h"

class TripLoader : public DataLoader {
public:
	static TripLoader& getInstance();
	virtual void write(const std::string& path) override;

protected:
	virtual InMemoryDataGenerator getInMemoryGenerator(const std::string& path) override;

private:
	TripLoader() = default;
	~TripLoader() = default;
};
