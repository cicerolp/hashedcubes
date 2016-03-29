#include "stdafx.h"
#include "CategoricalNode.h"

#include "CategoricalDimension.h"

void CategoricalNode::queryGroup(Response& response) const {
	response.insert(_container.begin(), _container.end());
}

void CategoricalNode::queryWhere(const std::vector<bool>& values, Response& response) const {
	for (auto& ptr : _container) {
		if (values[ptr.value()]) response.addElement(&ptr);
	}
}

size_t CategoricalNode::hash(const CategoricalDimension* hashing, response_container& response, Data& data, const Pivot& pivot) {
	auto size = ((CategoricalDimension*)hashing)->numberOfValues();
	
	std::vector<ulong> used(size, 0);

	for (auto i = pivot.front(); i < pivot.back(); ++i) {
		int value = data.getIntProperty(i);
		
		data.setHash(i, value);
		used[value]++;
	}

	ulong accum = pivot.front();
	for (ulong i = 0; i < size; ++i) {

		if (used[i] == 0) continue;

		int first = accum;
		accum += used[i];
		int second = accum;

		_container.emplace_back(first, second, static_cast<categorical_t>(i));
	}
	_container.shrink_to_fit();

	for (auto& ptr : _container) {
		response.emplace_back(&ptr);
	}

	data.sortHash(pivot.front(), pivot.back());

	return _container.size();
}
