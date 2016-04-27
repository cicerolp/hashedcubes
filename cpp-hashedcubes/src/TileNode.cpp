#include "stdafx.h"
#include "TileNode.h"

#include "SpatialDimension.h"

TileNode::TileNode(const TilePivot& pivot) : _pivot(pivot) { } 

TileNode::TileNode(const Pivot& pivot, const tile_t& value) : TileNode(TilePivot(pivot, value)) { }

void TileNode::queryTile(const SpatialDimension* hashing, const Query& query, Response& response, ulong level) const {
	const ulong d = level % hashing->key().size();

	if (query.evalTile(d)) {
		if (query.getTile(d) == _pivot.value() || (last() && util::intersects(_pivot.value(), query.getTile(d)))) {
			aggregateTile(hashing, query, response, level);

		} else if (_pivot.value().z < query.zoom) {
			if (_container[0] != nullptr) _container[0]->queryTile(hashing, query, response, level + 1);
			if (_container[1] != nullptr) _container[1]->queryTile(hashing, query, response, level + 1);
			if (_container[2] != nullptr) _container[2]->queryTile(hashing, query, response, level + 1);
			if (_container[3] != nullptr) _container[3]->queryTile(hashing, query, response, level + 1);
		} 		
	} else {
		if (_container[0] != nullptr) _container[0]->queryTile(hashing, query, response, level + 1);
		if (_container[1] != nullptr) _container[1]->queryTile(hashing, query, response, level + 1);
		if (_container[2] != nullptr) _container[2]->queryTile(hashing, query, response, level + 1);
		if (_container[3] != nullptr) _container[3]->queryTile(hashing, query, response, level + 1);
	}
}

void TileNode::aggregateTile(const SpatialDimension* hashing, const Query& query, Response& response, ulong level) const {
	const ulong d = level % hashing->key().size();

	if (query.evalTile(d) && (last() || _pivot.value().z >= query.zoom + query.resolution)) {
		response.addElement(&_pivot);

	} else {
		if (_container[0] != nullptr) _container[0]->aggregateTile(hashing, query, response, level + 1);
		if (_container[1] != nullptr) _container[1]->aggregateTile(hashing, query, response, level + 1);
		if (_container[2] != nullptr) _container[2]->aggregateTile(hashing, query, response, level + 1);
		if (_container[3] != nullptr) _container[3]->aggregateTile(hashing, query, response, level + 1);
	}
}

void TileNode::queryRegion(const SpatialDimension* hashing, const Query& query, Response& response, ulong level) const {
	const ulong d = level % hashing->key().size();

	if (query.evalRegion(d)) {
		/*std::cout << "eval" << std::endl;
		std::cout << query.getRegion(d).lon0 << std::endl;
		std::cout << util::tilex2lon(_pivot.value().x, 0)   << std::endl;
		std::cout << query.getRegion(d).lon1 << std::endl;
		std::cout << util::tilex2lon(_pivot.value().x + 1, 0) << std::endl;
		
		std::cout << query.getRegion(d).lat0 << std::endl;
		std::cout << util::tiley2lat(_pivot.value().y, 0) << std::endl;
		std::cout << query.getRegion(d).lat1 << std::endl;
		std::cout << util::tiley2lat(_pivot.value().y + 1, 0) << std::endl;*/
		
		if (util::intersectsTile(_pivot.value(), query.getRegion(d))) {
			//std::cout << "intersects" << std::endl; 
			if (d == query.getLastValidRegion() && (last() || _pivot.value().z == query.zoom)) {
				response.addElement(&_pivot);
			} else {
				if (_container[0] != nullptr) _container[0]->queryRegion(hashing, query, response, level + 1);
				if (_container[1] != nullptr) _container[1]->queryRegion(hashing, query, response, level + 1);
				if (_container[2] != nullptr) _container[2]->queryRegion(hashing, query, response, level + 1);
				if (_container[3] != nullptr) _container[3]->queryRegion(hashing, query, response, level + 1);
			}
		} else {
			return;
		}
	} else {
		if (_container[0] != nullptr) _container[0]->queryRegion(hashing, query, response, level + 1);
		if (_container[1] != nullptr) _container[1]->queryRegion(hashing, query, response, level + 1);
		if (_container[2] != nullptr) _container[2]->queryRegion(hashing, query, response, level + 1);
		if (_container[3] != nullptr) _container[3]->queryRegion(hashing, query, response, level + 1);
	}
}

size_t TileNode::hash(const SpatialDimension* hashing, response_container& response, Data& data) {
	SpatialDimension* spatial = (SpatialDimension*)hashing;

	if (spatial->key().size() > 1) {
		std::vector<zoom_level> zoom(spatial->key().size());
		zoom[0].z = 1;
		return hashMultiple(spatial, data, response, zoom, 0) + 1;
	} else {
		return hashSingle(spatial, data, response, 0) + 1;
	}
}

size_t TileNode::hashSingle(const SpatialDimension* hashing, Data& data, response_container& response, ulong zoom) {

	size_t count = 0;

	if (zoom < hashing->maxZoom() && _pivot.size() > hashing->min_per_node()) {

		std::map <std::pair<uint32_t, uint32_t>, uint32_t> used;

		zoom += 1;

		for (auto i = _pivot.front(); i < _pivot.back(); ++i) {

			ulong y = util::lat2tiley(data.getFloatProperty(i, 0), zoom);
			ulong x = util::lon2tilex(data.getFloatProperty(i, 1), zoom);
			
			used[std::make_pair(x, y)]++;
			data.setHash(i, getIndex(x, y));
		}

		// sorting
		data.sortHash(_pivot.front(), _pivot.back());

		int accum = _pivot.front();
		int first, second;

		for (auto& pair : used) {

			if (pair.second == 0) continue;

			first = accum;
			accum += pair.second;
			second = accum;
            
			ulong x = pair.first.first;
            ulong y = pair.first.second;
                        
			ulong index = getIndex(x, y);

			count++;

			_container[index] = (std::make_unique<TileNode>(TileNode(TilePivot(first, second, tile_t(x, y, zoom)))));
			count += _container[index]->hashSingle(hashing, data, response, zoom);			
		}		
	} else {
		response.emplace_back(&_pivot);
		setLast();
	}

	return count;
}

size_t TileNode::hashMultiple(const SpatialDimension* hashing, Data& data, response_container& response, std::vector<zoom_level> zoom, ulong level) {

	ulong d = (level + 1) % hashing->key().size();

	size_t count = 0;

	if (level < hashing->maxZoom() && _pivot.size() > hashing->min_per_node()) {

		std::map <std::pair<uint32_t, uint32_t>, uint32_t> used;

		for (auto i = _pivot.front(); i < _pivot.back(); ++i) {

			ulong y = util::lat2tiley(data.getFloatProperty(i, d), zoom[d].z);
			ulong x = util::lon2tilex(data.getFloatProperty(i, d + (ulong)hashing->key().size()), zoom[d].z);

			used[std::make_pair(x, y)]++;
			data.setHash(i, getIndex(x, y));
		}

		// sorting
		data.sortHash(_pivot.front(), _pivot.back());

		int accum = _pivot.front();
		int first, second;

		ulong curr_zoom = zoom[d].z;
		zoom[d].z += 1;

		for (auto& pair : used) {

			if (pair.second == 0) continue;

			first = accum;
			accum += pair.second;
			second = accum;

			ulong index = getIndex(pair.first.first, pair.first.second);
			
			count++;

			_container[index] = (std::make_unique<TileNode>(TileNode(TilePivot(first, second, tile_t(pair.first.first, pair.first.second, curr_zoom)))));

			zoom[d].first = pair.first.first;
			zoom[d].second = pair.first.second;

			count += _container[index]->hashMultiple(hashing, data, response, zoom, level + 1);
		}

	} else {
		response.emplace_back(&_pivot);
		setLast();

		TileNode* ptr = this;

		ulong k = level % hashing->key().size();
		while (d != k) {
		
			count++;

			ptr->_container[0] = (std::make_unique<TileNode>(TileNode(TilePivot(ptr->_pivot.front(), ptr->_pivot.back(), tile_t(zoom[d].first, zoom[d].second, zoom[d].z - 1)))));
			ptr->_container[0]->setLast();

			ptr = ptr->_container[0].get();

			d = (d + 1) % hashing->key().size();
		}
	}

	return count;
}
