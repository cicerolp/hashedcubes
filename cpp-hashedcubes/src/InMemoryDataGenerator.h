#pragma once

#include "DataGenerator.h"
#include <memory>

class InMemoryDataGenerator : public DataGenerator {
public:
	InMemoryDataGenerator(const std::string& path, const std::vector<tuple_t> tokens);
	virtual ~InMemoryDataGenerator() = default;

	virtual void addInt(const std::string& key, ulong value) override;

	virtual void addFloat(const std::string& key, float value) override;

	virtual void execute() override;

	virtual void commit() override;

protected:

	ulong _size{ 0 };
    std::vector<std::shared_ptr<std::ofstream>> _output;
	
private:
	inline size_t find_if(const std::string& key) {
		auto it = std::find_if(_tokens.begin(), _tokens.end(), [key](tuple_t tuple) { return (key == std::get<0>(tuple)); });

		return it - _tokens.begin();
	}

};
