#pragma once

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#pragma warning( disable : 4250)
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

// stl
#include <iostream>
#include <fstream>
#include <sstream>

#include <forward_list>
#include <string>
#include <deque>
#include <vector>
#include <array>

#include <regex>

#include <random>
#include <chrono>
#include <thread>

#include <cstdint>

#include <memory>
#include <algorithm>

#include <utility>
#include <numeric>
#include <limits>

#define _USE_MATH_DEFINES
#include <cmath>

#include <random>
#include <iostream>
#include <memory>
#include <functional>

#include <iterator>

#include <map>
#include <unordered_set>
#include <unordered_map>

#include <bitset>

// meerkat
#include <meerkat/meerkat.h>

// rapidjson
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

// boost
#include <boost/filesystem.hpp>

#include <boost/program_options.hpp>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/tuple.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

// util
#include "Trunc.h"
#include "Date.h"
#include "StringUtil.h"

#include "Types.h"
#include "Mercator.h"

#include "ResidentSetSize.h"

#include <sqlite3.h>
#include "SQLiteCpp/SQLiteCpp.h"

typedef std::tuple<std::string, DimensionType> token_t;
typedef std::tuple<std::string, std::string, DimensionType> tuple_t;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete;

template<int N> struct bits;
template<> struct bits<1> { enum { value = 1 }; };
template<int N> struct bits { enum { value = 1 + bits<(N >> 1)>::value }; };
