#pragma once
#include "TemporalInterval.h"

class HashedCube;

class Query {
public:
	enum QueryType { TILE, GROUP, TSERIES, SCATTER, MYSQL, REGION };

	QueryType type;
	std::string instance{ "brightkite" };
	std::shared_ptr<HashedCube> cube;

	ulong zoom;
	ulong resolution;

	std::unordered_map<std::string, TemporalInterval> tseries;
	std::unordered_set<std::string> group;
	
	Query(const std::string& url);
	Query(const std::vector<std::string>& tokens);
	Query(const std::string& instance, const std::string& type);

	friend std::ostream& operator<< (std::ostream& stream, const Query& query);
	
	inline const TemporalInterval& getTseries(const std::string& key) const {
		return tseries.at(key);
	}
	inline const std::vector<bool>& getWhere(const std::string& key) const {
		return _where.at(key);
	}


	inline bool evalTseries(const std::string& key) const {
		return tseries.find(key) != tseries.end();
	}
	inline bool evalGroup(const std::string& key) const {
		return group.find(key) != group.end();
	}
	

	// tile
	inline void emplaceTile(ulong key, const Tile& tile) {
		if (key < _tile.size()) {
			_tile[key] = tile;
		}
	}
	inline bool evalTile(ulong key) const {
		if (key < _tile.size()) {
			return _tile[key].isValid();
		} else return false;
	}
	inline bool evalAnyTile(const std::vector<ulong>& key) const {
		for (auto& k : key) {
			if (evalTile(k)) return true;
		}
		return false;
	}
	inline const Tile& getTile(ulong key) const {
		return _tile[key];
	}

	// region
	inline void emplaceRegion(ulong key, const TileBounds& region) {
		if (key < _region.size()) {
			_region[key] = region;
		}
	}
	inline bool evalRegion(ulong key) const {
		if (key < _region.size()) {
			return _region[key].isValid();
		}
		else return false;
	}
	inline bool evalAnyRegion(const std::vector<ulong>& key) const {
		for (auto& k : key) {
			if (evalRegion(k)) return true;
		}
		return false;
	}
	inline const TileBounds& getRegion(ulong key) const {
		return _region.at(key);
	}
	
	//where
	void emplaceWhere(const std::string& key, const std::vector<int>& values);

	inline bool evalWhere(const std::string& key) const {
		return _where.find(key) != _where.end();
	}

protected:
	// tile
	std::vector<Tile> _tile;

	// region
	std::vector<TileBounds> _region;

	// where
	std::unordered_map<std::string, std::vector<bool>> _where;

	inline std::string nextToken(std::vector<std::string>::const_iterator& it) { return  *(++it); }
};