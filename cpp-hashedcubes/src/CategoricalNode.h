#pragma once

#include "CategoricalPivot.h"

#include "Data.h"
#include "Query.h"
#include "Response.h"

class CategoricalDimension;

class CategoricalNode {
public:
	CategoricalNode() = default;

	CategoricalNode(CategoricalNode&&) = default;
	CategoricalNode(const CategoricalNode&) = delete;

	~CategoricalNode() = default;

	CategoricalNode& operator=(CategoricalNode&&) = delete;
	CategoricalNode& operator=(const CategoricalNode&) = delete;

	void queryGroup(Response& response) const;
	void queryWhere(const std::vector<bool>& values, Response& response) const;

	size_t hash(const CategoricalDimension* hashing, response_container& response, Data& data, const Pivot& pivot);

	inline const ulong front() const;
	inline const ulong back() const;

protected:
	ulong value;
	std::vector<CategoricalPivot> _container;
};

const ulong CategoricalNode::front() const {
	return _container.front().front();
}

const ulong CategoricalNode::back() const {
	return _container.back().back();
}
