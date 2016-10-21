#include "stdafx.h"
#include "TemporalSerializer.h"
#include "TemporalPivot.h"
#include "TemporalDimension.h"

TemporalSerializer& TemporalSerializer::getInstance() {
	static TemporalSerializer instance;
	return instance;
}

std::string TemporalSerializer::serialize(const TemporalDimension* hashing, const Response& response, std::chrono::milliseconds& duration) {

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	std::vector<ulong> aggregate(bins, 0);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	for (const auto& pivot : response.container()) {
		size_t index = util::linearScale(hashing->interval().lower(), hashing->interval().upper(), 0, bins - 1, ((TemporalPivot*)pivot)->value());
		aggregate[index] += pivot->size();
	}

	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);	

	writer.StartArray();
	for (ulong i = 0; i < aggregate.size(); ++i) {
		if (aggregate[i] == 0) continue;
		writer.StartArray();
		writer.Uint((i * (hashing->interval().days() / bins)) + hashing->interval().lower());
		writer.Uint(aggregate[i]);
		writer.EndArray();
	}
	writer.EndArray();
	return buffer.GetString();
}

std::string TemporalSerializer::serialize(const TemporalDimension* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) {

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	std::vector<ulong> aggregate(bins, 0);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	
	auto it = tuple.second.begin();
	for (const auto& pivot : tuple.first.container()) {
		int size = 0;

		while (it != tuple.second.end() && (*pivot) > (**it)) {
			size += (**it).size();
			it++;
		}

		size_t index = util::linearScale(hashing->interval().lower(), hashing->interval().upper(), 0, bins - 1, ((TemporalPivot*)pivot)->value());
		aggregate[index] += size;
	}

	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	writer.StartArray();
	for (ulong i = 0; i < aggregate.size(); ++i) {
		if (aggregate[i] == 0) continue;
		writer.StartArray();
		writer.Uint((i * (hashing->interval().days() / bins)) + hashing->interval().lower());
		writer.Uint(aggregate[i]);
		writer.EndArray();
	}
	writer.EndArray();
	return buffer.GetString();
}
