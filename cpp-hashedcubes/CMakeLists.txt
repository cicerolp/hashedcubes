# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (HELLO)

SET(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(AFTER include)
include_directories(AFTER include/SQLiteCpp)
include_directories(AFTER ./)

add_executable (main
  include/SQLiteCpp/Backup.cpp
  include/SQLiteCpp/Column.cpp
  include/SQLiteCpp/Database.cpp
  include/SQLiteCpp/Statement.cpp
  include/SQLiteCpp/Transaction.cpp
  include/SQLiteCpp/sqlite3.c
  include/meerkat/meerkat.c
  src/CategoricalDimension.cpp
  src/CategoricalNode.cpp
  src/CategoricalSerializer.cpp
  src/DMPLoader.cpp
  src/Data.cpp
  src/DataLoader.cpp
  src/FlightsDelayLoader.cpp
  src/FlightsPerformanceLoader.cpp
  src/HashedCube.cpp
  src/HashedCubeInstances.cpp
  src/InMemoryData.cpp
  src/InMemoryDataGenerator.cpp
  src/MySQLSerializer.cpp
  src/Query.cpp
  src/SQLData.cpp
  src/ScatterSerializer.cpp
  src/Server.cpp
  src/SnapLoader.cpp
  src/SpatialDimension.cpp
  src/SpatialSerializer.cpp
  src/SplomLoader.cpp
  src/StringUtil.cpp
  src/TemporalDimension.cpp
  src/TemporalNode.cpp
  src/TemporalSerializer.cpp
  src/TileNode.cpp
  src/TripLoader.cpp
  src/Types.cpp
  src/main.cpp
  )

target_link_libraries (main LINK_PUBLIC boost_filesystem boost_serialization boost_system boost_iostreams pthread)
