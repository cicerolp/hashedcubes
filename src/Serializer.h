#pragma once

#include "Response.h"

template<typename T>
class Serializer {
public:
	virtual std::string serialize(const T* hashing, const Response& response, std::chrono::milliseconds& duration) = 0;
	virtual std::string serialize(const T* hashing, const std::pair<Response, Response>& tuple, std::chrono::milliseconds& duration) = 0;
protected:
private:
};
