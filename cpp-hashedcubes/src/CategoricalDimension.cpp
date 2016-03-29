#include "stdafx.h"
#include "CategoricalDimension.h"

CategoricalDimension::CategoricalDimension(const std::string& key, ulong numberOfValues) 
	: Hashing(key), _numberOfValues(numberOfValues) { }

bool CategoricalDimension::query(const Query& query, const Response& range, Response& response) const {
	
	if (query.evalWhere(_key)) {

		auto& values = query.getWhere(_key);

		auto note_it = _nodes.begin();
		for (auto range_it = range.begin(); range_it != range.end(); ++range_it) {

			nextNode(*range_it, note_it);

			while ((**range_it).endAfter((*note_it)->front())) {

				(*note_it)->queryWhere(values, response);

				if (++note_it == _nodes.end()) return true;
			}
		}
		return true;

	} else if (query.evalGroup(_key)) {

		auto node_it = _nodes.begin();
		for (auto range_it = range.begin(); range_it != range.end(); ++range_it) {

			nextNode(*range_it, node_it);

			while ((**range_it).endAfter((*node_it)->front())) {

				(*node_it)->queryGroup(response);

				if (++node_it == _nodes.end()) return true;
			}
		}
		return true;

	} else {
		return false;
	}
}

size_t CategoricalDimension::hash(const response_container& container, response_container& response, Data& data) {
	std::cout << "\tHashing Categorical Dimension: " + _key + "... ";

	data.prepareKey(_key);

	size_t pivots_count = 0;

	for (const auto& ptr : container) {
		if (ptr == nullptr || (*ptr).empty()) continue;

		_nodes.emplace_back(std::make_unique<CategoricalNode>());

		pivots_count += _nodes.back()->hash(this, response, data, *(Pivot*)ptr);
	}
	_nodes.shrink_to_fit();

	std::cout << "Done." << std::endl;
	data.dispose();

	return pivots_count;
}

ulong CategoricalDimension::numberOfValues() const {
	return _numberOfValues;
}
