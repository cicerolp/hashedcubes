#pragma once

#include "Serializer.h"
#include "Response.h"

class TemporalDimension;

class TemporalSerializer : public Serializer<TemporalDimension> {
public:

	static TemporalSerializer& getInstance();
	
	std::string serialize(const TemporalDimension* hashing, const Response& response, std::chrono::milliseconds& duration) override;
	std::string serialize(const TemporalDimension* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) override;

	static const ulong bins { 256 };
};
