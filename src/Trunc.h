#pragma once

namespace util {
	inline double trunc(double d) {
		return std::trunc(d * 100000.0) / 100000.0;
	}

	inline int linearScale(float min, float max, float a, float b, float x) {
		return std::floor((((b - a) * (x - min)) / (max - min)) + a);
		//return value >= b ? (b - 1) : (value < a ? a : value);
	}
} // namespace util