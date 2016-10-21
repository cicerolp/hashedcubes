#include "stdafx.h"
#include "HashedCubeInstances.h"

#include "InMemoryDataGenerator.h"

#include "SnapLoader.h"
#include "TripLoader.h"

#include "DMPLoader.h"

#include "FlightsDelayLoader.h"
#include "FlightsPerformanceLoader.h"

#include "SplomLoader.h"

#include "SpatialSerializer.h"
#include "CategoricalSerializer.h"
#include "TemporalSerializer.h"

#include "ScatterSerializer.h"

#include "MySQLSerializer.h"

#include "SpatialDimension.h"
#include "CategoricalDimension.h"
#include "TemporalDimension.h"

HashedCubeInstances& HashedCubeInstances::getInstance() {
	static HashedCubeInstances instance;
	return instance;
}

void HashedCubeInstances::run(const std::vector<Schema>& args) {
	size_t pivots_counts = 0;
	long long duration = 0;

	for (auto& schema : args) {
		auto tuple = DataLoader::build(schema);		

		duration += std::get<1>(tuple);
		pivots_counts += std::get<2>(tuple);

		HashedCubeInstances::getInstance()._container.emplace(schema.name, std::get<0>(tuple));
	}

	if (HashedCubeInstances::getInstance().telemetry()) {
		(*HashedCubeInstances::getInstance()._telemetry_file) << "memory" << "," << "time2build" << "," << "pivots" << std::endl;

		(*HashedCubeInstances::getInstance()._telemetry_file) << getCurrentRSS() / (1024 * 1024) << "," << duration << "," << pivots_counts << std::endl;

		(*HashedCubeInstances::getInstance()._telemetry_file) << "total" << "," << "spatial" << "," << "categorical" << ","
			<< "temporal" << "," << "aggregation" << "," << "query" << std::endl;
	}

	std::cout << "Current Resident Size: " << getCurrentRSS() / (1024 * 1024) << " MB" << std::endl;

	HashedCubeInstances::getInstance().setReady();
}

void HashedCubeInstances::write(const std::vector<Schema>& args) {
	for (auto& schema : args) {
		std::cout << "\nWriting HCF files: " << std::endl;
		std::cout << "\tName: " << schema.name << std::endl;

		if (schema.loader == "trips") {
			TripLoader::getInstance().write(schema.path);
		}
		else if (schema.loader == "snap") {
			SnapLoader::getInstance().write(schema.path);
		}
		else if (schema.loader == "performance") {
			FlightsPerformanceLoader::getInstance().write(schema.path);
		}
		else if (schema.loader == "delay") {
			FlightsDelayLoader::getInstance().write(schema.path);
		}
		else if (schema.loader == "splom") {
			SplomLoader::getInstance().write(schema.path);
		}
		else if (schema.loader == "dmp") {
			DMPLoader::getInstance().write(schema.path);
		}
		else return;
	}
}

HashedCubeInstances::~HashedCubeInstances() {
	_ready = false;
	if (telemetry()) _telemetry_file->close();
}

std::string HashedCubeInstances::query(const Query& query) {
	if (query.cube == nullptr) return "[]";

	std::vector<Response> response(2);
	response[0] = query.cube->root();

	std::string json("[]");

	std::chrono::milliseconds duration_aggregation(0);
	std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> start(3), end(3);

	////////////////////////////////////////////////////////
	Response target;
	bool target_equal = false;

	SpatialDimension* spatial_dimension = nullptr;
	CategoricalDimension* categorical_dimension = nullptr;
	TemporalDimension* temporal_dimension = nullptr;

	std::vector<std::pair<const CategoricalDimension*, Response>> scatter_tuples;

	start[Spatial] = std::chrono::high_resolution_clock::now();
	if (query.cube->spatial() != nullptr && query.cube->spatial()->query(query, response[0], response[1])) {
		if (query.type == Query::TILE) {
			target_equal = true;
			target = response[1];
			spatial_dimension = query.cube->spatial().get();
		} else {
			target_equal = false;
		}
		response[0].swap(response[1]);
	}
	end[Spatial] = std::chrono::high_resolution_clock::now();

	start[Categorical] = std::chrono::high_resolution_clock::now();
	for (const auto& hashing : query.cube->categorical()) {
		if (hashing->query(query, response[0], response[1])) {
			if (query.evalGroup(hashing->key())) {
				if (query.type == Query::GROUP) {
					target_equal = true;
					target = response[1];
					categorical_dimension = hashing.get();
				}
				else if (query.type == Query::SCATTER) {
					target_equal = true;
					scatter_tuples.emplace_back(hashing.get(), response[1]);
				}
			} else {
				target_equal = false;
			}
			response[0].swap(response[1]);
		}
	}
	end[Categorical] = std::chrono::high_resolution_clock::now();

	if (query.type != Query::SCATTER) {
		start[Temporal] = std::chrono::high_resolution_clock::now();
		for (const auto& hashing : query.cube->temporal()) {
			if (hashing->query(query, response[0], response[1])) {
				if (query.evalTseries(hashing->key()) && query.type == Query::TSERIES) {
					target_equal = true;
					target = response[1];
					temporal_dimension = hashing.get();
				}
				else {
					target_equal = false;
				}
				response[0].swap(response[1]);
			}
		}
		end[Temporal] = std::chrono::high_resolution_clock::now();
	}
	////////////////////////////////////////////////////////

	if (query.type == Query::GROUP) {
		if (categorical_dimension == nullptr) {
			json = "[]";
		}
		else if (target_equal) {
			json = CategoricalSerializer::getInstance().serialize(categorical_dimension, response[0], duration_aggregation);
		}
		else {
			json = CategoricalSerializer::getInstance().serialize(categorical_dimension, { target, response[0] }, duration_aggregation);
		}
	}
	else if (query.type == Query::TSERIES) {
		if (temporal_dimension == nullptr) {
			json = "[]";
		}
		else if (target_equal) {
			json = TemporalSerializer::getInstance().serialize(temporal_dimension, response[0], duration_aggregation);
		}
		else {
			json = TemporalSerializer::getInstance().serialize(temporal_dimension, { target, response[0] }, duration_aggregation);
		}
	}
	else if (query.type == Query::TILE) {
		if (spatial_dimension == nullptr) {
			json = "[]";
		}
		else if (target_equal) {
			json = SpatialSerializer::getInstance().serialize(spatial_dimension, response[0], duration_aggregation);
		}
		else {
			json = SpatialSerializer::getInstance().serialize(spatial_dimension, { target, response[0] }, duration_aggregation);
		}
	}
	else if (query.type == Query::MYSQL) {
		json = MySQLSerializer::getInstance().serialize(query.cube->getSQLData(), response[0], duration_aggregation);
	}
	else if (query.type == Query::REGION) {
		std::chrono::time_point<std::chrono::high_resolution_clock> start_aggr, end_aggr;
		start_aggr = std::chrono::high_resolution_clock::now();
		ulong count = 0;
		for (const auto& pivot : response[0]) {
			count += pivot->size();
		}
		end_aggr = std::chrono::high_resolution_clock::now();
		duration_aggregation = std::chrono::duration_cast<std::chrono::milliseconds>(end_aggr - start_aggr);

		json = "[" + std::to_string(count) + "]";
	}
	else if (query.type == Query::SCATTER) {
		if (scatter_tuples.size() != 2) {
			json = "[]";
		}
		else {
			json = ScatterSerializer::getInstance().serialize(scatter_tuples, duration_aggregation);
		}
	}

	if (telemetry()) {
		long long duration_spatial = std::chrono::duration_cast<std::chrono::milliseconds>(end[Spatial] - start[Spatial]).count();
		long long duration_categorical = std::chrono::duration_cast<std::chrono::milliseconds>(end[Categorical] - start[Categorical]).count();
		long long duration_temporal = std::chrono::duration_cast<std::chrono::milliseconds>(end[Temporal] - start[Temporal]).count();

		long long duration = duration_spatial + duration_categorical + duration_temporal + duration_aggregation.count();

		(*_telemetry_file) << duration << "," << duration_spatial << "," << duration_categorical << ","
			<< duration_temporal << "," << duration_aggregation.count() << "," << query << std::endl;
	}

	return json;
}

std::string HashedCubeInstances::schema(const std::string& instance) {
	auto cube = get(instance);

	if (cube == nullptr) return "[null]";

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	temporal_t lower = 0;
	temporal_t upper = 0;

	if (cube->temporal().size() != 0) {
		// TODO get temporalinfo from key
		auto interval = cube->temporal().front()->interval();

		lower = interval.lower();
		upper = interval.upper();
	}

	writer.StartObject();
	writer.String("totalcount");
	writer.Int(cube->size());
	writer.String("mindate");
	writer.Uint(lower);
	writer.String("maxdate");
	writer.Uint(upper);
	writer.EndObject();

	return buffer.GetString();
}

/* Local Variables:  */
/* mode: c++         */
/* c-basic-offset: 8 */
/* indent-tabs-mode: t */
/* End:              */
