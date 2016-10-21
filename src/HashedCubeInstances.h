#pragma once

#include "HashedCube.h"
#include "Query.h"

class HashedCubeInstances {
public:
	
	static HashedCubeInstances& getInstance();

	static void run(const std::vector<Schema>& args);
	static void write(const std::vector<Schema>& args);

	~HashedCubeInstances();
	
	inline bool ready() const { return _ready; };
	inline void setReady() { _ready = true; };

	inline std::shared_ptr<HashedCube> get(std::string instance) const {
		auto it = _container.find(instance);
		if (it != _container.end()) {
			return (*it).second;
		} else {
			return nullptr;
		}
	}

	std::string query(const Query& query);
	std::string schema(const std::string& instance);

	inline void enableTelemetry(const std::string& file) {
		_telemetry = true;

		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::stringstream stream;
		stream << std::put_time(std::localtime(&time), "-%Y-%m-%d-%H-%M-%S.csv");

		_telemetry_file = std::make_shared<std::ofstream>(file + stream.str(), std::ofstream::out);
	}
	inline bool telemetry() { return _telemetry; }

	std::unordered_map<std::string, std::shared_ptr<HashedCube>> _container;
	
protected:
	bool _ready{ false };

	bool _telemetry{ false };
	std::shared_ptr<std::ofstream> _telemetry_file;
};
