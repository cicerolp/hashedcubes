#include "stdafx.h"
#include "HashedCube.h"

HashedCube::HashedCube(ulong size){
	_root = std::make_unique<Pivot>(Pivot(size));
	_root_container.emplace_back(_root.get());
}

std::shared_ptr<SpatialDimension> HashedCube::setSpatial(const std::vector<ulong>& key, ulong leaf) {
	_spatial = std::make_shared<SpatialDimension>(key, leaf);
	return _spatial;
}

std::shared_ptr<CategoricalDimension> HashedCube::addCategorical(const std::string& key, ulong numberOfValues) {
	_categorical.emplace_back(std::make_shared<CategoricalDimension>(key, numberOfValues));
	return _categorical.back();
}

std::shared_ptr<TemporalDimension> HashedCube::addTemporal(const std::string& key, ulong bucketSize) {
	_temporal.emplace_back(std::make_shared<TemporalDimension>(key, bucketSize));
	return _temporal.back();
}


