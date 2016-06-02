#include "stdafx.h"
#include "Query.h"
#include "HashedCubeInstances.h"

Query::Query(const std::string& url) : Query(util::split(url, "[/]+")) { }

Query::Query(const std::string& instance, const std::string& type) {
	this->instance = instance;
	cube = HashedCubeInstances::getInstance().get(instance);

	if (cube != nullptr && cube->spatial() != nullptr) {
		_tile = std::vector<Tile>(cube->spatial()->key().size());
		_region = std::vector<TileBounds>(cube->spatial()->key().size());
	}

	if (type == "tile") {
		this->type = TILE;
	}
	else if (type == "group") {
		this->type = GROUP;
	}
	else if (type == "tseries") {
		this->type = TSERIES;
	}
	else if (type == "scatter") {
		this->type = SCATTER;
	}
	else if (type == "region") {
		this->type = REGION;
	}
	else if (type == "mysql") {
		this->type = MYSQL;
	}
}

Query::Query(const std::vector<std::string>& tokens) : Query(tokens[3], tokens[4]) {
	for (auto it = tokens.begin() + 5; it != tokens.end(); ++it) {
		if ((*it) == "tile") {

			const auto key = nextToken(it);

			const auto x = std::stoi(nextToken(it));
			const auto y = std::stoi(nextToken(it));

			ulong z = std::stoi(nextToken(it));
			zoom = z;
			resolution = std::stoi(nextToken(it));

			emplaceTile(std::stoul(key), Tile(x, y, z));

		}
		else if ((*it) == "region") {

			const auto key = nextToken(it);

			ulong z = std::stoi(nextToken(it));
			zoom = z;

			int x0 = (int)std::stof(nextToken(it));
			int y0 = (int)std::stof(nextToken(it));
			int x1 = (int)std::stof(nextToken(it));
			int y1 = (int)std::stof(nextToken(it));

			emplaceRegion(std::stoul(key), TileBounds(x0, y0, x1, y1, z));
		}
		else if ((*it) == "field") {
			group.emplace(nextToken(it));

		}
		else if ((*it) == "where") {
			std::vector<std::string> uri = util::split(nextToken(it), std::regex("&"));

			for (auto clause : uri) {
				std::vector<std::string> literals = util::split(clause, std::regex("=|:"));

				if ((literals.size() - 1) <= 0) continue;

				std::vector<int> values;

				for (size_t i = 1; i < literals.size(); i++) {
					values.emplace_back(std::stoi(literals[i]));
				}
                std::sort(values.begin(), values.end());

				emplaceWhere(literals[0], values);
			}

		}
		else if ((*it) == "tseries") {
			auto key = nextToken(it);

			temporal_t lower = std::stoul(nextToken(it));
			temporal_t upper = std::stoul(nextToken(it));

			tseries.emplace(key, TemporalInterval(lower, upper));
		}
	}
}



std::ostream & operator<<(std::ostream & stream, const Query & query) {

	stream << "/" + query.instance;

	switch (query.type) {
		case Query::TILE: stream << "/tile"; break;
		case Query::GROUP: stream << "/group"; break;
		case Query::TSERIES: stream << "/tseries"; break;
		case Query::SCATTER: stream << "/scatter"; break;
		case Query::REGION: stream << "/region"; break;
		case Query::MYSQL: stream << "/mysql"; break;
	}

	// /tile/key/x/y/z/r
	for (size_t i = 0; i < query._tile.size(); ++i) {
		if (query._tile[i].isValid()) {
			std::string key = std::to_string(i);
			std::string r = std::to_string(query.resolution);

			stream << "/tile/" + key + "/" << query._tile[i] << "/" + r;
		}
	}

	// /region/key/z/x0/y0/x1/y1
	for (size_t i = 0; i < query._region.size(); ++i) {
		if (query._region[i].isValid()) {
			std::string key = std::to_string(i);
			std::string z = std::to_string(query.zoom);

			stream << "/region/" + key + "/" + z + "/" << query._region[i];
		}
	}


	// /field/<category>
	for (auto& group : query.group) {
		stream << "/field/" + group;
	}

	// /where/<category>=<[value]:[value]...:[value]>&<category>=<[value]:[value]...:[value]>
	if (query._where.size() != 0) {
		std::string where_stream = "/where/";

		for (auto& where : query._where) {
			where_stream += where.first + "=";

			for (size_t i = 0; i < where.second.size(); ++i) {
				if (where.second[i]) where_stream += std::to_string(i) + ":";
			}
			where_stream = where_stream.substr(0, where_stream.size() - 1);
			where_stream += "&";
		}

		where_stream = where_stream.substr(0, where_stream.size() - 1);
		stream << where_stream;
	}

	// /tseries/key/
	for (auto& tseries : query.tseries) {
		stream << "/tseries/" + tseries.first + "/" + std::to_string(tseries.second.lower()) + "/" + std::to_string(tseries.second.upper());
	}	
	
	return stream;
}

void Query::emplaceWhere(const std::string& key, const std::vector<int>& values) {
	if (cube == nullptr) return;

	ulong size = cube->getCategoricalValues(key);
	if (size != 0) {
		std::vector<bool> values_ext(size, false);
		for (auto& v : values) values_ext[v] = true;
		_where.emplace(key, values_ext);
	}
}
