#pragma once

#include "Serializer.h"
#include "Response.h"

class CategoricalDimension;

class ScatterSerializer : public Serializer<CategoricalDimension> {
public:

	static ScatterSerializer& getInstance();

	std::string serialize(const CategoricalDimension* hashing, const Response& response, std::chrono::milliseconds& duration) override;
	std::string serialize(const CategoricalDimension* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) override;

	std::string serialize(const std::vector<std::pair<const CategoricalDimension*, Response>>& tuples, std::chrono::milliseconds& duration);
};