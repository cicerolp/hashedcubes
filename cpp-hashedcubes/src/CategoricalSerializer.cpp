#include "stdafx.h"
#include "CategoricalSerializer.h"
#include "CategoricalPivot.h"
#include "CategoricalDimension.h"

CategoricalSerializer& CategoricalSerializer::getInstance() {
	static CategoricalSerializer instance;
	return instance;
}

std::string CategoricalSerializer::serialize(const CategoricalDimension* hashing, const Response& response, std::chrono::milliseconds& duration) {

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	std::vector<ulong> aggregate((hashing)->numberOfValues(), 0);
	
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	for (const auto& pivot : response.container()) {
		aggregate[((CategoricalPivot*)pivot)->value()] += pivot->size();
	}

	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	
	writer.StartArray();
	for (ulong i = 0; i < aggregate.size(); ++i) {
		if (aggregate[i] == 0) continue;

		writer.StartArray();
		writer.Uint(i);
		writer.Uint(aggregate[i]);
		writer.EndArray();
	}
	writer.EndArray();
	return buffer.GetString();
}

std::string CategoricalSerializer::serialize(const CategoricalDimension* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) {

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	std::vector<ulong> aggregate((hashing)->numberOfValues(), 0);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	
	auto it = tuple.second.begin();
	for (const auto& pivot : tuple.first.container()) {
		int size = 0;
		
		while (it != tuple.second.end() && (*pivot) > (**it) ) {
			size += (**it).size();
			it++;
		}

		aggregate[((CategoricalPivot*)pivot)->value()] += size;
	}

	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	writer.StartArray();
	for (ulong i = 0; i < aggregate.size(); ++i) {
		if (aggregate[i] == 0) continue;

		writer.StartArray();
		writer.Uint(i);
		writer.Uint(aggregate[i]);
		writer.EndArray();
	}
	writer.EndArray();
	return buffer.GetString();
}
