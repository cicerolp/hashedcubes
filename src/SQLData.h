#pragma once

#include "Data.h"
#include"Types.h"

#include "InMemoryData.h"

class SQLData : public InMemoryData {
public:
	SQLData(const Schema& schema, const InMemoryDataDescriptor& descriptor);
	virtual ~SQLData() = default;

	virtual void dispose() override;

	SQLData(const SQLData&) = default;
	SQLData& operator=(const SQLData&) = default;

	inline SQLite::Database* getDb() const;
	inline ulong getSQLIndex(ulong id) const;

protected:
	std::unique_ptr<SQLite::Database> _db;	
};

SQLite::Database* SQLData::getDb() const {
	return _db.get();
}

ulong SQLData::getSQLIndex(ulong id) const {
	return _index[id];
}

