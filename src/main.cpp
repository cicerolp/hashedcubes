#include "stdafx.h"
#include "Server.h"
#include "HashedCubeInstances.h"

#include "Nanocubes.h"
#include "TwitterSQL.h"

Schema loadConfig(std::string fileName) {

   if (!boost::filesystem::exists("myfile.txt")) {
      std::cerr << "error: " << fileName << " does not exist." << std::endl;
      std::abort();
   }

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

   for (auto& v : pt.get_child("config.schema")) {
      if (v.first == "spatial") {
         e.spatial.emplace_back(v.second.get<ulong>("key"));
      } else if (v.first == "categorical") {
         std::string key = v.second.get<std::string>("key");
         ulong size = v.second.get<ulong>("size");
         e.categorical.emplace_back(key, size);
      } else if (v.first == "temporal") {
         std::string key = v.second.get<std::string>("key");
         ulong bin = v.second.get<ulong>("bin");
         e.temporal.emplace_back(key, bin);
      }
   }

   return e;
}

int main(int argc, char* argv[]) {
   bool to_sql = false;
   bool to_hcf = false;

   bool no_server = false;
   uint32_t server_port = 8000;

   bool telemetry = false;
   std::string telemetry_file = "telemetry";

   std::vector<std::string> input_files;

   bool benchmark = false;
   std::vector<std::string> benchmark_files;

   namespace po = boost::program_options;

   // declare the supported options
   po::options_description desc("\nCommand Line Arguments");
   desc.add_options()
         ("help,h", "produce help message")

         ("sql,d", "generate sql db")
         ("hcf,b", "generate hcf binary file")

         ("no-server,s", "disable server")
         ("port,p", po::value<uint32_t>(&server_port)->default_value(server_port), "server port")

         ("telemetry,t", po::value<std::string>(&telemetry_file)->default_value(telemetry_file), "telemetry output file")

         ("xml,x", po::value<std::vector<std::string>>(&input_files)
          ->default_value(std::vector<std::string>(1, "./xml/brightkite.nds.xml"), "./xml/brightkite.nds.xml")->composing(), "input files")
         ("log,l", po::value<std::vector<std::string>>(&benchmark_files)
          ->default_value(std::vector<std::string>(1, "./csv/brightkite.csv"), "./csv/brightkite.csv")->composing(), "benchmark input files");

   po::positional_options_description p;
   p.add("xml", -1);

   po::variables_map vm;
   po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
   po::notify(vm);

   if (vm.count("help")) {
      std::cout << desc << std::endl;
      exit(0);
   }

   if (vm.count("telemetry")) {
      telemetry = true;
   }

   if (vm.count("no-server")) {
      no_server = true;
   }

   if (vm.count("log")) {
      benchmark = true;
   }

   if (vm.count("sql")) {
      to_sql = true;
   }

   if (vm.count("hcf")) {
      to_hcf = true;
   }

   std::vector<Schema> args;
   std::cout << "XML Files:" << std::endl;

   for (auto& str : input_files) {
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
   if (benchmark) {
      for (auto& str : benchmark_files) {
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

   if (benchmark) {
      std::vector<Query> queries = util::nanocubes_log(benchmark_files);

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
         std::cout << "Done." << std::endl;
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
