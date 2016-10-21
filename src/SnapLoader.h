#pragma once

#include "DataLoader.h"
#include "InMemoryDataGenerator.h"

class SnapLoader : public DataLoader {
public:
	static SnapLoader& getInstance();
	virtual void write(const std::string& path) override;

protected:
	virtual InMemoryDataGenerator getInMemoryGenerator(const std::string& path) override;

private:
	SnapLoader() = default;
	~SnapLoader() = default;
};
