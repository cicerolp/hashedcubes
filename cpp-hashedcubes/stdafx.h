#pragma once

#if !defined(_MSC_VER)

////////////////////////////////////////////////////////////////////////////////
// NOTE: this is *not* vc++'s stdafx.h - it's something we hacked together
// for unix compiles.
////////////////////////////////////////////////////////////////////////////////

#define NOMINMAX

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

// openmp
// #include <omp.h>

// meerkat
#include <meerkat/meerkat.h>

// rapidjson
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/tuple.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

// mysql
#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>

// util
#include "src/Trunc.h"
#include "src/Date.h"
#include "src/StringUtil.h"
#include "src/Mercator.h"

#include "src/Types.h"

#include "ResidentSetSize.h"

typedef std::tuple<std::string, DimensionType> token_t;
typedef std::tuple<std::string, std::string, DimensionType> tuple_t;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete;

template<int N> struct bits;
template<> struct bits<1> { enum { value = 1 }; };
template<int N> struct bits { enum { value = 1 + bits<(N >> 1)>::value }; };

namespace std {

	template<typename T, typename ...Args>
	std::unique_ptr<T> make_unique(Args&& ...args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}

};

// #pragma once

// /* we cheat and put all the includes that windows auto-adds here */

// #include <string>
// #include <sys/types.h>
// #include <vector>
// #include <regex>
// #include <unordered_map>
// #include <unordered_set>
// #include <iostream>
// #include <array>
// #include <boost/dynamic_bitset/dynamic_bitset.hpp>
// #include "Types.h"
// #include "StringUtil.h"
// #include <rapidjson/rapidjson.h>
// #include <rapidjson/stringbuffer.h>
// #include <rapidjson/writer.h>

// typedef std::tuple<std::string, std::string, DimensionType> tuple_t;
// stdafx.h:typedef std::tuple<std::string, DimensionType> token_t;

#else

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

// openmp
#include <omp.h>

// meerkat
#include <meerkat/meerkat.h>

// rapidjson
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

#include <boost/filesystem.hpp>

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
#include "Mercator.h"

#include "Types.h"

#include "ResidentSetSize.h"

#include "SQLiteCpp\SQLiteCpp.h"

typedef std::tuple<std::string, DimensionType> token_t;
typedef std::tuple<std::string, std::string, DimensionType> tuple_t;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete;

template<int N> struct bits;
template<> struct bits<1> { enum { value = 1 }; };
template<int N> struct bits { enum { value = 1 + bits<(N >> 1)>::value }; };

#endif // !defined(_MSC_VER)



