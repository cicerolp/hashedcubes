#pragma once

#include "Serializer.h"
#include "Response.h"

class SpatialDimension;

class SpatialSerializer : public Serializer<SpatialDimension> {
public:

	static SpatialSerializer& getInstance();

	std::string serialize(const SpatialDimension* hashing, const Response& response, std::chrono::milliseconds& duration) override;
	std::string serialize(const SpatialDimension* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) override;

protected:	
	
private:
};
