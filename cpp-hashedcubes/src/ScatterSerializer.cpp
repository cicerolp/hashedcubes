#include "stdafx.h"
#include "ScatterSerializer.h"

#include "CategoricalDimension.h"

ScatterSerializer& ScatterSerializer::getInstance() {
	static ScatterSerializer instance;
	return instance;
}

std::string ScatterSerializer::serialize(const CategoricalDimension* hashing, const Response& response, std::chrono::milliseconds& duration) { return "[]"; }

std::string ScatterSerializer::serialize(const CategoricalDimension* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) { return "[]"; }

std::string ScatterSerializer::serialize(const std::vector<std::pair<const CategoricalDimension*, Response>>& tuples, std::chrono::milliseconds& duration) {
	if (tuples.size() != 2) return "[]";

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	size_t size = (tuples[0].first)->numberOfValues() * (tuples[1].first)->numberOfValues();

	std::vector<ulong> aggregate(size, 0);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	auto it = tuples[1].second.begin();
	for (const auto& pivot : tuples[0].second.container()) {
		int size = 0;

		while (it != tuples[1].second.end() && (*pivot) > (**it)) {
			aggregate[((CategoricalPivot*)pivot)->value() * (tuples[1].first)->numberOfValues() + ((CategoricalPivot*)*it)->value()] += ((CategoricalPivot*)*it)->size();
			it++;
		}		
	}

	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	writer.StartArray();
	for (ulong i = 0; i < aggregate.size(); ++i) {
		if (aggregate[i] == 0) continue;

		writer.StartArray();
		writer.Uint(i - ((i / ((tuples[1].first)->numberOfValues()) * ((tuples[1].first)->numberOfValues()))));
		writer.Uint(i / (tuples[1].first)->numberOfValues());
		writer.Uint(aggregate[i]);
		writer.EndArray();
	}
	writer.EndArray();
	return buffer.GetString();
}
