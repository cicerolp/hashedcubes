#include "stdafx.h"
#include "TemporalDimension.h"
#include "TemporalNode.h"
#include "TemporalPivot.h"

TemporalDimension::TemporalDimension(const std::string& key, size_t bucketSize) 
	: Hashing(key), _bucketSize(static_cast<float>(bucketSize)) { }

bool TemporalDimension::query(const Query& query, const Response& range, Response& response) const {
	
	if (query.evalTseries(_key)) {
		if (query.type == Query::TSERIES) {

			auto& interval = query.getTseries(_key);

			auto node_it = _nodes.begin();
			for (auto range_it = range.begin(); range_it != range.end(); ++range_it) {

				nextNode(*range_it, node_it);

				while ((**range_it).endAfter((*node_it)->front())) {

					(*node_it)->queryTseries(interval, response);

					if (++node_it == _nodes.end()) return true;
				}
			}

		} else if (!query.getTseries(_key).contains(_interval.lower(), _interval.upper())) {

			auto& interval = query.getTseries(_key);

			auto node_it = _nodes.begin();
			for (auto range_it = range.begin(); range_it != range.end(); ++range_it) {

				nextNode(*range_it, node_it);

				while ((**range_it).endAfter((*node_it)->front())) {

					(*node_it)->query(interval, response);

					if (++node_it == _nodes.end()) return true;
				}
			}
		} else {
			return false;
		}
		return true;

	} else {
		return false;
	}
}

size_t TemporalDimension::hash(const response_container& container, response_container& response, Data& data) {
	std::cout << "\tHashing Temporal Dimension: " + _key + "... ";

	data.prepareKey(_key);

	size_t pivots_count = 0;

	for (const auto& ptr : container) {
		
		if (ptr == nullptr || (*ptr).empty()) continue;

		_nodes.emplace_back(std::make_unique<TemporalNode>(*ptr));

		pivots_count += _nodes.back()->hash(this, response, data, *ptr);
	}
	_nodes.shrink_to_fit();

	std::cout << "Done." << std::endl;
	data.dispose();

	return pivots_count;
}

float TemporalDimension::bucketSize() const {
	return _bucketSize;
}

TemporalInterval& TemporalDimension::interval() {
	return _interval;
}

const TemporalInterval& TemporalDimension::interval() const {
	return _interval;
}

unsigned int TemporalDimension::intervalSize() const {
	return _interval.days();
}


