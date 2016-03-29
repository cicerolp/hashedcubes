#include "stdafx.h"
#include "InMemoryDataGenerator.h"
#include "InMemoryDataDescriptor.h"

InMemoryDataGenerator::InMemoryDataGenerator(const std::string& path, const std::vector<tuple_t> tokens)
	: DataGenerator(path, tokens) {

	for (auto token : _tokens) {
          std::ofstream *s = new std::ofstream(_path + "hcf/" + std::get<1>(token), std::ios::out | std::ios::binary);
          _output.push_back(std::shared_ptr<std::ofstream>(s));
	}
}

void InMemoryDataGenerator::addInt(const std::string& key, ulong value) {
	_output[find_if(key)]->write(reinterpret_cast<char*>(&value), sizeof(ulong));
}

void InMemoryDataGenerator::addFloat(const std::string& key, float value) {
	_output[find_if(key)]->write(reinterpret_cast<char*>(&value), sizeof(float));
}

void InMemoryDataGenerator::execute() {
	_size++;
}

void InMemoryDataGenerator::commit() {
	for (auto& output : _output) {
		output->close();
	}

	InMemoryDataDescriptor descriptor(_size);

	for (auto token : _tokens) {
		descriptor.set(token);
	}

	descriptor.save(_path + "hcf/descriptor.hcf");
}
