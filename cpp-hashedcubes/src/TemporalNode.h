#pragma once

#include "TemporalPivot.h"

#include "Data.h"
#include "Query.h"
#include "Response.h"

class TemporalDimension;

class TemporalNode {
public:
	TemporalNode(const Pivot& pivot);
	TemporalNode(TemporalNode&&) = default;
	TemporalNode(const TemporalNode&) = delete;

	~TemporalNode() = default;

	TemporalNode& operator=(TemporalNode&&) = default;
	TemporalNode& operator=(const TemporalNode&) = delete;

	void query(const TemporalInterval& interval, Response& response) const;
	void queryTseries(const TemporalInterval& interval, Response& response) const;

	size_t hash(const TemporalDimension* hashing, response_container& response, Data& data, const Pivot& pivot);

	inline ulong lower() const;
	inline ulong upper() const;

	inline const ulong front() const;
	inline const ulong back() const;

protected:
	void queryInterval(const TemporalInterval& interval, Response& response) const;

	Pivot _pivot;
	std::vector<TemporalPivot> _container;
};

const ulong TemporalNode::front() const {
	return _pivot.front();
}

const ulong TemporalNode::back() const {
	return _pivot.back();
}

ulong TemporalNode::lower() const {
	return _container.front().value();
}

ulong TemporalNode::upper() const {
	return _container.back().value();
}
