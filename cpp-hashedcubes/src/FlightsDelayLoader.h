#pragma once

#include "DataLoader.h"
#include "InMemoryDataGenerator.h"

#include "FlightsLoader.h"

class FlightsDelayLoader : public DataLoader, public FlightsLoader {
public:
	static FlightsDelayLoader& getInstance();
	virtual void write(const std::string& path) override;

protected:
	virtual InMemoryDataGenerator getInMemoryGenerator(const std::string& path) override;

private:
	FlightsDelayLoader() = default;
	~FlightsDelayLoader() = default;
};
