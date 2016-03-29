#pragma once

#include "Hashing.h"
#include "TemporalNode.h"

class TemporalDimension : public Hashing<TemporalNode, std::string> {
public:
	TemporalDimension(const std::string& key, size_t bucketSize);
	TemporalDimension(TemporalDimension&&) = default;
	TemporalDimension(const TemporalDimension&) = delete;

	~TemporalDimension() = default;

	TemporalDimension& operator=(TemporalDimension&&) = default;
	TemporalDimension& operator=(const TemporalDimension&) = delete;

	bool query(const Query& query, const Response& range, Response& response) const override;
	size_t hash(const response_container& container, response_container& response, Data& data) override;

	float bucketSize() const;
	TemporalInterval& interval();
	const TemporalInterval& interval() const;

	unsigned int intervalSize() const;

protected:

	const float _bucketSize;
	TemporalInterval _interval;
private:
};

