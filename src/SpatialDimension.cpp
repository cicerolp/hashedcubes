#include "stdafx.h"
#include "SpatialDimension.h"

#include "TileNode.h"

SpatialDimension::SpatialDimension(const std::vector<ulong>& key, ulong leaf) : Hashing(key) {
	_max_zoom = 20 * (ulong)key.size();
	_min_per_node = leaf;
}

bool SpatialDimension::query(const Query& query, const Response& range, Response& response) const {
	if (!query.evalAnyRegion(_key) && !query.evalAnyTile(_key)) return false;

	auto node_it = _nodes.begin();
	for (auto range_it = range.begin(); range_it != range.end(); ++range_it) {

		nextNode(*range_it, node_it);

		while ((**range_it).endAfter((*node_it)->front())) {

			if (query.type == Query::TILE) {
				(*node_it)->queryTile(this, query, response, 0);
			} else {
				(*node_it)->queryRegion(this, query, response, 0);
			}
						
			if (++node_it == _nodes.end()) return true;
		}
	}

	return true;
}

size_t SpatialDimension::hash(const response_container& container, response_container& response, Data& data) {
	std::cout << "\tHashing " + std::to_string(key().size()) + " SpatialDimension(s)...";

	data.prepareKey("lat", key());
	data.prepareKey("lon", key());

	size_t pivots_count = 0;

	for (const auto& ptr : container) {
		if (ptr == nullptr || (*ptr).empty()) continue;

		_nodes.emplace_back(std::make_unique<TileNode>(*ptr, tile_t()));
		
		pivots_count += _nodes.back()->hash(this, response, data);
	}
	_nodes.shrink_to_fit();

	std::cout << "Done." << std::endl;
	data.dispose();

	return pivots_count;
}

ulong SpatialDimension::maxZoom() const {
	return _max_zoom;
}


ulong SpatialDimension::min_per_node() const {
	return _min_per_node;
}


