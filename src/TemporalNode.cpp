#include "stdafx.h"
#include "TemporalNode.h"

#include "TemporalDimension.h"

TemporalNode::TemporalNode(const Pivot& pivot) : _pivot(pivot) { }

void TemporalNode::query(const TemporalInterval& interval, Response& response) const {
	if (interval.contains(lower(), upper())) {
		response.addElement(&_pivot);
	} else {
		queryInterval(interval, response);
	}
}

void TemporalNode::queryTseries(const TemporalInterval& interval, Response& response) const {
	if (interval.contains(lower(), upper())) {
		response.insert(_container.begin(), _container.end());
	} else {
		queryInterval(interval, response);
	}
}

void TemporalNode::queryInterval(const TemporalInterval& interval, Response& response) const {
	if (!interval.intersects(lower(), upper())) return;

	auto lower = std::lower_bound(_container.begin(), _container.end(), interval.lower(),
		[](const TemporalPivot& o1, const temporal_t& o2) { return o2 > o1.value(); }
	);

	auto upper = std::lower_bound(lower, _container.end(), interval.upper(),
		[](const TemporalPivot& o1, const temporal_t& o2) { return o2 > o1.value(); }
	);

	response.insert(lower, upper);
}

size_t TemporalNode::hash(const TemporalDimension* hashing, response_container& response, Data& data, const Pivot& pivot) {

	std::map<int, int> map{};

	const float bucketSize = ((TemporalDimension*)hashing)->bucketSize();

	for (auto i = pivot.front(); i < pivot.back(); ++i) {

		ulong value = std::floor(data.getIntProperty(i) / bucketSize) * bucketSize;
		
		// update TemporalInterval
		auto& interval = ((TemporalDimension*)hashing)->interval();
		if (value > interval.upper()) interval.setUpper(value);
		if (value < interval.lower()) interval.setLower(value);

		map[value]++;

		data.setHash(i, value);
	}

	int accum = pivot.front();
	for (auto entry : map) {

		int first = accum;
		accum += entry.second;
		int second = accum;

		_container.emplace_back(first, second, entry.first);
	}
	_container.shrink_to_fit();

	for (auto& ptr : _container) {
		response.emplace_back(&ptr);
	}

	data.sortHash(pivot.front(), pivot.back());

	return _container.size();
}




