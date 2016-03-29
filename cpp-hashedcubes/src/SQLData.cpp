#include "stdafx.h"
#include "SQLData.h"

SQLData::SQLData(const Schema& schema, const InMemoryDataDescriptor& descriptor) : InMemoryData(descriptor) {
	try {
		_db = std::make_unique<SQLite::Database>(schema.path + "db3/log.db3", SQLITE_OPEN_READONLY);
		std::cout << "SQLite database file '" << _db->getFilename().c_str() << "' opened successfully\n";		
	} catch (std::exception& e) {
		std::cout << "SQLite exception: " << e.what() << std::endl;
		return;
	}
}

void SQLData::dispose() {
	InMemoryData::dispose();
	Data::_hash.clear();
}
