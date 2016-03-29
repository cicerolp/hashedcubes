#pragma once

#include "Hashing.h"
#include "CategoricalNode.h"

class CategoricalDimension : public Hashing<CategoricalNode, std::string> {
public:
	CategoricalDimension(const std::string& key, ulong numberOfValues);
	CategoricalDimension(CategoricalDimension&&) = default;
	CategoricalDimension(const CategoricalDimension&) = delete;

	~CategoricalDimension() = default;

	CategoricalDimension& operator=(CategoricalDimension&&) = default;
	CategoricalDimension& operator=(const CategoricalDimension&) = delete;

	bool query(const Query& query, const Response& range, Response& response) const override;
	size_t hash(const response_container& container, response_container& response, Data& data) override;

	ulong numberOfValues() const;

protected:

	const ulong _numberOfValues{ 0 };
};