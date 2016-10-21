#include "stdafx.h"
#include "MySQLSerializer.h"

MySQLSerializer& MySQLSerializer::getInstance() {
	static MySQLSerializer instance;
	return instance;
}

std::string MySQLSerializer::serialize(const Response& response, std::chrono::milliseconds& duration) {
	return "[]";
}

std::string MySQLSerializer::serialize(SQLData* data, Response& response, std::chrono::milliseconds& duration) {
	static const ulong max_count = 300;
	ulong count = 0;

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	SQLite::Statement statement(*data->getDb(), "SELECT * FROM data WHERE ROWID==?");

	writer.StartArray();

	for (auto it = response.container().begin(); count < max_count && it != response.container().end(); ++it) {		
		for (ulong id = (*it)->front(); count < max_count && id != (*it)->back(); ++id, ++count) {
			try {
				statement.reset();
				statement.bind(1, (int)data->getSQLIndex(id));
				if (statement.executeStep()) {
					writer.StartObject();
					for (int i = 0; i < statement.getColumnCount(); ++i) {
						writer.String(statement.getColumnName(i));
						switch (statement.getColumn(i).getType()) {
						case SQLITE_INTEGER:
							writer.Int(statement.getColumn(i).getInt());
							break;
						case SQLITE_FLOAT:
							writer.Double(statement.getColumn(i).getDouble());
							break;
						case SQLITE_TEXT:
							writer.String(statement.getColumn(i).getText());
							break;
						case SQLITE_BLOB:
							writer.String((char*)statement.getColumn(i).getBlob());
							break;
						case SQLITE_NULL:
							writer.String("null");
							break;
						}
					}
					writer.EndObject();
				}
			}
			catch (std::exception& e) {
				std::cout << "SQLite exception: " << e.what() << std::endl;
				return "[]";
			}
		}
	}

	writer.EndArray();
	return buffer.GetString();
}