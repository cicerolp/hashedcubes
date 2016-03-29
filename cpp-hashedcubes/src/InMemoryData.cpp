#include "stdafx.h"
#include "InMemoryData.h"

InMemoryData::InMemoryData(const InMemoryDataDescriptor& descriptor) : _descriptor(descriptor) {
	initialize(_descriptor.size());
}

void InMemoryData::dispose() {
	_data.clear();
}
	
void InMemoryData::prepareKey(const std::string& key) {	
	auto tuple = _descriptor.get(key);

	std::ifstream is(_descriptor.schema().path + "hcf/" + std::get<0>(tuple), std::ios::in | std::ifstream::binary);
		
	if (is.is_open()) {
		try {
			switch (std::get<1>(tuple)) {
			case Spatial:
				_data.emplace_back(size() * sizeof(float));
				is.read(&_data.back()[0], size() * sizeof(float));
				break;
			case Categorical:
			case Temporal:
			default:
				_data.emplace_back(size() * sizeof(int));
				is.read(&_data.back()[0], size() * sizeof(int));
				break;
			}

			is.close();
		} catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			exit(0);
		}		
	} else {
		std::cout << "!error opening file" << std::endl;
		exit(0);
	}
}

void InMemoryData::prepareKey(const std::string& str, const std::vector<ulong>& key) {
	for (auto& k : key) {
		prepareKey(str + std::to_string(k));
	}
}

int InMemoryData::getIntProperty(ulong id, ulong key) {
	return *((int*)&_data[key][_index[id] * sizeof(int)]);
}

float InMemoryData::getFloatProperty(ulong id, ulong key) {
	return *((float*)&_data[key][_index[id] * sizeof(float)]);
}

ulong InMemoryData::size() const {
	return _descriptor.size();
}

