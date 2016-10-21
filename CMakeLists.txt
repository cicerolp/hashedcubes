project (hashedcubes)
cmake_minimum_required (VERSION 2.8.11)

set(BOOST_INCLUDEDIR "C:/Boost/include/boost-1_62/")
set(BOOST_LIBRARYDIR "C:/Boost/lib/")

if(MSVC)
   set(CMAKE_CXX_FLAGS "/EHsc")
endif()

if(UNIX)
   set(CMAKE_CXX_FLAGS "-std=c++14 ")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(cotire)

aux_source_directory(. SRC_LIST)

include_directories(AFTER include)
include_directories(AFTER include/sqlite)
include_directories(AFTER src)
include_directories(AFTER ./)

add_executable (${PROJECT_NAME} ${SRC_LIST}
   include/sqlite/sqlite3.c
   include/meerkat/meerkat.c   
   include/SQLiteCpp/Backup.cpp
   include/SQLiteCpp/Column.cpp
   include/SQLiteCpp/Database.cpp
   include/SQLiteCpp/Statement.cpp
   include/SQLiteCpp/Transaction.cpp   
   src/CategoricalDimension.cpp
   src/CategoricalNode.cpp
   src/CategoricalSerializer.cpp
   src/Data.cpp
   src/DataLoader.cpp
   src/DMPLoader.cpp
   src/FlightsDelayLoader.cpp
   src/FlightsPerformanceLoader.cpp
   src/HashedCube.cpp
   src/HashedCubeInstances.cpp
   src/InMemoryData.cpp
   src/InMemoryDataGenerator.cpp
   src/main.cpp
   src/MySQLSerializer.cpp
   src/Query.cpp
   src/ScatterSerializer.cpp
   src/Server.cpp
   src/SnapLoader.cpp
   src/SpatialDimension.cpp
   src/SpatialSerializer.cpp
   src/SplomLoader.cpp
   src/SQLData.cpp
   src/StringUtil.cpp
   src/TemporalDimension.cpp
   src/TemporalNode.cpp
   src/TemporalSerializer.cpp
   src/TileNode.cpp
   src/TripLoader.cpp
   src/Types.cpp
)

find_package(Threads REQUIRED)

set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.62 COMPONENTS system filesystem serialization iostreams REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/stdafx.h")

cotire(${PROJECT_NAME})
