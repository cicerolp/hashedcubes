#include "stdafx.h"
#include "SpatialSerializer.h"

#include "TilePivot.h"

SpatialSerializer& SpatialSerializer::getInstance() {
	static SpatialSerializer instance;
	return instance;
}

std::string SpatialSerializer::serialize(const SpatialDimension* hashing, const Response& response, std::chrono::milliseconds& duration) {

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	std::unordered_map<tile_t, ulong> aggregate;

	for (const auto& pivot : response.container()) {
		aggregate[((TilePivot*)pivot)->value()] += pivot->size();
	}

	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	writer.StartArray();
	for (const auto& pair : aggregate) {
		writer.StartArray();
		writer.Uint(pair.first.x);
		writer.Uint(pair.first.y);
		writer.Uint(pair.first.z);
		writer.Uint(pair.second);
		writer.EndArray();
	}
	writer.EndArray();
	return buffer.GetString();
}

std::string SpatialSerializer::serialize(const SpatialDimension* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) {
	
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	std::unordered_map<tile_t, ulong> aggregate;

	auto it = tuple.second.begin();
	for (const auto& pivot : tuple.first.container()) {
		ulong size = 0;

		while (it != tuple.second.end() && (*pivot) > (**it)) {
			size += (**it).size();
			it++;
		}

		aggregate[((TilePivot*)pivot)->value()] += size;
	}

	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	writer.StartArray();
	for (const auto& pair : aggregate) {
		if (pair.second == 0) continue;
		writer.StartArray();
		writer.Uint(pair.first.x);
		writer.Uint(pair.first.y);
		writer.Uint(pair.first.z);
		writer.Uint(pair.second);
		writer.EndArray();
	}
	writer.EndArray();
	return buffer.GetString();
}
