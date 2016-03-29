#pragma once

#include "DataLoader.h"
#include "InMemoryDataGenerator.h"

#include "FlightsLoader.h"

class FlightsPerformanceLoader : public DataLoader, public FlightsLoader {
public:
	static FlightsPerformanceLoader& getInstance();
	virtual void write(const std::string& path) override;

protected:
	virtual InMemoryDataGenerator getInMemoryGenerator(const std::string& path) override;

private:
	FlightsPerformanceLoader() = default;
	~FlightsPerformanceLoader() = default;
};
