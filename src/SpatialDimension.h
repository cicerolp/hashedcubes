#pragma once

#include "Hashing.h"
#include "TileNode.h"

class SpatialDimension : public Hashing<TileNode, std::vector<ulong>> {
public:
	SpatialDimension(const std::vector<ulong>& key, ulong leaf);
	SpatialDimension(SpatialDimension&&) = default;
	SpatialDimension(const SpatialDimension&) = delete;

	~SpatialDimension() = default;

	SpatialDimension& operator=(SpatialDimension&&) = default;
	SpatialDimension& operator=(const SpatialDimension&) = delete;

	bool query(const Query& query, const Response& range, Response& response) const override;
	size_t hash(const response_container& container, response_container& response, Data& data) override;

	ulong maxZoom() const;
	ulong min_per_node() const;

protected:

	ulong _max_zoom;
	ulong _min_per_node;
};
