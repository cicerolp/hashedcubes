#pragma once

#include "Data.h"
#include "InMemoryDataDescriptor.h"

class InMemoryData : public Data {
public:
	InMemoryData(const InMemoryDataDescriptor& descriptor);
	virtual ~InMemoryData() = default;

	virtual void dispose() override;

	virtual void prepareKey(const std::string& key) override;
	virtual void prepareKey(const std::string& str, const std::vector<ulong>& key) override;

	virtual int getIntProperty(ulong id, ulong key) override;
	virtual float getFloatProperty(ulong id, ulong key) override;

	virtual ulong size() const override;

protected:
	std::vector<std::vector<char>> _data;
	InMemoryDataDescriptor _descriptor;
};
