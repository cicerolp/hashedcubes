#pragma once

#include "Serializer.h"
#include "Response.h"

#include "SQLData.h"

class MySQLSerializer {
public:
	static MySQLSerializer& getInstance();
	std::string serialize(const Response& response, std::chrono::milliseconds& duration);
	std::string serialize(SQLData* data, Response& response, std::chrono::milliseconds& duration);
protected:
	
private:
};