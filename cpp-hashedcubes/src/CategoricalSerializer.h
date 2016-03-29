#pragma once

#include "Serializer.h"
#include "Response.h"

class CategoricalDimension;

class CategoricalSerializer : public Serializer<CategoricalDimension> {
public:
	static CategoricalSerializer& getInstance();

	virtual std::string serialize(const CategoricalDimension* hashing, const Response& response, std::chrono::milliseconds& duration) override;
	virtual std::string serialize(const CategoricalDimension* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) override;

protected:
	
private:
};
