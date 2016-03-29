#pragma once

#include "Types.h"
#include "TilePivot.h"

#include "Data.h"
#include "Query.h"
#include "Response.h"

class SpatialDimension;

class TileNode {
public:
	TileNode(const TilePivot& pivot);
	TileNode(const Pivot& pivot, const tile_t& value);

	TileNode(TileNode&&) = default;
	TileNode(const TileNode&) = delete;

	~TileNode() = default;

	TileNode& operator=(TileNode&&) = default;
	TileNode& operator=(const TileNode&) = delete;

	void queryTile(const SpatialDimension* hashing, const Query& query, Response& response, ulong level) const;
	void queryRegion(const SpatialDimension* hashing, const Query& query, Response& response, ulong level) const;

	size_t hash(const SpatialDimension* hashing, response_container& response, Data& data);

	inline const ulong front() const;
	inline const ulong back() const;
	
private:
	struct zoom_level {
		zoom_level() : first(0), second(0), z(0) {}
		ulong first, second, z;
	};

	void aggregateTile(const SpatialDimension* hashing, const Query& query, Response& response, ulong level) const;

	inline ulong getIndex(ulong x, ulong y) const;

	inline bool last() const;
	inline void setLast();

	size_t hashSingle(const SpatialDimension* hashing, Data& data, response_container& response, ulong zoom);
	size_t hashMultiple(const SpatialDimension* hashing, Data& data, response_container& response, std::vector<zoom_level> zoom, ulong level);

	TilePivot _pivot;
	std::array<std::unique_ptr<TileNode>, 4> _container{};
};

ulong TileNode::getIndex(ulong x, ulong y) const {
	if (x % 2 == 0) {
		if (y % 2 == 0) return 0;
		else return 1;
	} else {
		if (y % 2 == 0) return 2;
		else return 3;
	}
}

bool TileNode::last() const {
	return _pivot.last();
}

void TileNode::setLast() {
	_pivot.setLast();
}

const ulong TileNode::front() const {
	return _pivot.front();
}

const ulong TileNode::back() const {
	return _pivot.back();
}