#include "stdafx.h"
#include "Server.h"
#include "HashedCubeInstances.h"

#include "Nanocubes.h"
#include "TwitterSQL.h"

Schema loadConfig(std::string fileName) {
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml(fileName, pt);

	Schema e;

	boost::optional<ulong> leaf = pt.get_optional<ulong>("config.leaf");
	if (leaf.is_initialized()) e.leaf = leaf.value();
	
	boost::optional<ulong> sql_threshold = pt.get_optional<ulong>("config.sql-threshold");
	if (sql_threshold.is_initialized()) e.sql_threshold = sql_threshold.value();
	
	boost::optional<float> fraction = pt.get_optional<float>("config.fraction");
	if (fraction.is_initialized()) e.fraction = fraction.value() / 100.f;

	e.name = pt.get<std::string>("config.name");
	e.path = pt.get<std::string>("config.path");
	e.loader = pt.get<std::string>("config.loader");

	for (auto &v : pt.get_child("config.schema")) {
		if (v.first == "spatial") {
			e.spatial.emplace_back(v.second.get<ulong>("key"));
		}
		else if (v.first == "categorical") {
			std::string key = v.second.get<std::string>("key");
			ulong size = v.second.get<ulong>("size");
			e.categorical.emplace_back(key, size);
		}
		else if (v.first == "temporal") {
			std::string key = v.second.get<std::string>("key");
			ulong bin = v.second.get<ulong>("bin");
			e.temporal.emplace_back(key, bin);
		}
	}

	return e;
}

int main(int argc, char *argv[]) {

	bool to_sql = false;
	bool to_hcf = false;
	bool telemetry = false;
	std::string telemetry_file = "telemetry";
	bool no_server = false;
	bool nanocube_log = false;
	
	ulong server_port = 8000;

	std::vector<Schema> args;
	std::vector<std::string> xml_files;
	std::vector<std::string> nanocube_files;
	
	try {
		if (argc < 2) {
			xml_files.emplace_back("./xml/brightkite-nanocubes.xml");
		}
		else {
			for (int i = 1; i < argc; i++) {
				if (argv[i][0] != '-') {
					xml_files.emplace_back(argv[i]);
				}
				else {
					std::string arg = argv[i];
					if (arg == std::string("-telemetry")) {
						telemetry = true;
						telemetry_file = argv[++i];
					}
					else if (arg == "-no-server") {
						no_server = true;
					}
					else if (arg == "-log") {
						nanocube_log = true;
						nanocube_files.emplace_back(argv[++i]);
					}
					else if (arg == "-hcf") {
						to_hcf = true;
					}
					else if (arg == "-sql") {
						to_sql = true;
					}
					else if (arg == "-port") {
						try {
							server_port = std::stoul(argv[++i]);
						} catch (...) {
							server_port = 8000;
						}
					}
				}
			}
		}
	} catch (...) {
		std::cout << "error: invalid arguments" << std::endl;
		exit(0);
	}	

	std::cout << "XML Files:" << std::endl;

	for (auto& str : xml_files) {
		std::cout << "\t" + str << std::endl;
		args.emplace_back(loadConfig(str));
	}
	
	std::cout << "Options:" << std::endl;

	std::cout << "\t-port " + std::to_string(server_port) << std::endl;

	if (telemetry) {
		std::cout << "\t-telemetry" << std::endl;
		HashedCubeInstances::getInstance().enableTelemetry(telemetry_file);
	}		
	if (no_server) {
		std::cout << "\t-no-server" << std::endl;
	}
	if (nanocube_log) {
		for (auto& str : nanocube_files) {
			std::cout << "\t-log " + str << std::endl;
		}
	}
	if (to_hcf) {
		std::cout << "\t-hcf" << std::endl;
	}
	if (to_sql) {
		std::cout << "\t-sql" << std::endl;
	}
	
	if (to_sql) {
		util::twitter_sql(args);
		exit(0);
	} else if (to_hcf) {
		std::thread t1(HashedCubeInstances::write, args);
		t1.join();
		exit(0);
	} else {
		std::thread t1(HashedCubeInstances::run, args);
		t1.join();
	}
	
	if (nanocube_log) {
		std::vector<Query> queries = util::nanocubes_log(nanocube_files);
		
		ulong passes = 10;
		for (ulong pass = 0; pass < passes; ++pass) {
			std::cout << "[" 
				+ std::to_string(pass + 1) 
				+ " of " 
				+ std::to_string(passes) 
				+ "] Running " 
				+ std::to_string(queries.size())
				+ " queries... ";

			for (auto& query : queries) {
				HashedCubeInstances::getInstance().query(query);
			}
			std::cout << "Done."<< std::endl;
		}		
	}

	if (!no_server) {
		std::thread server(Server::run, telemetry, server_port);

		getchar();

		Server::getInstance().stop();
		server.join();
	}	
	
	return 0;
}
