#pragma once

#include "SQLData.h"
#include "SpatialDimension.h"
#include "TemporalDimension.h"
#include "CategoricalDimension.h"

class HashedCube {
public:
	HashedCube(ulong size);

	HashedCube(HashedCube&&) = default;
	HashedCube(const HashedCube&) = delete;

	~HashedCube() = default;

	HashedCube& operator=(HashedCube&&) = default;
	HashedCube& operator=(const HashedCube&) = delete;

	std::shared_ptr<SpatialDimension> setSpatial(const std::vector<ulong>& key, ulong leaf);
	std::shared_ptr<CategoricalDimension> addCategorical(const std::string& key, ulong numberOfValues);
	std::shared_ptr<TemporalDimension> addTemporal(const std::string& key, ulong bucketSize);

	inline std::shared_ptr<SpatialDimension>& spatial();
	inline std::vector<std::shared_ptr<CategoricalDimension>>& categorical();
	inline std::vector<std::shared_ptr<TemporalDimension>>& temporal();

	inline void setSQLData(std::unique_ptr<Data>& data);
	inline SQLData* getSQLData(void) const;

	inline const response_container& root() const;
	inline ulong size() const;

	inline ulong getCategoricalValues(const std::string& key) const;
	
protected:	
	std::unique_ptr<Pivot> _root;
	response_container _root_container;
		
	std::unique_ptr<Data> _data{ nullptr };

	std::shared_ptr<SpatialDimension> _spatial { nullptr };
	std::vector<std::shared_ptr<CategoricalDimension>> _categorical;
	std::vector<std::shared_ptr<TemporalDimension>> _temporal;
};

ulong HashedCube::getCategoricalValues(const std::string& key) const {
	for (const auto& hashing : _categorical) {
		if (hashing->key() == key) {
			return hashing->numberOfValues();
		}
	}
	return 0;
}

void HashedCube::setSQLData(std::unique_ptr<Data>& data) {
	_data = std::move(data);
}

SQLData* HashedCube::getSQLData(void) const {
	return (_data != nullptr)? (SQLData*)_data.get() : nullptr;
}

const response_container& HashedCube::root() const {
	return _root_container;
}

ulong HashedCube::size() const {
	return _root->size();
}

std::shared_ptr<SpatialDimension>& HashedCube::spatial() {
	return _spatial;
}

std::vector<std::shared_ptr<CategoricalDimension>>& HashedCube::categorical() {
	return _categorical;
}

std::vector<std::shared_ptr<TemporalDimension>>& HashedCube::temporal() {
	return _temporal;
}