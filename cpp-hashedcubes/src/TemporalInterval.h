#pragma once

class TemporalInterval {
public:
	TemporalInterval() : TemporalInterval(std::numeric_limits<temporal_t>::max(), std::numeric_limits<temporal_t>::min()) {};
	TemporalInterval(temporal_t lower, temporal_t upper) : _bound{lower, upper} {};
	virtual ~TemporalInterval() = default;

	inline temporal_t lower() const {
		return _bound[0];
	}
	inline temporal_t upper() const {
		return _bound[1];
	}

	inline void setLower(temporal_t value) {
		_bound[0] = value;
	}
	inline void setUpper(temporal_t value) {
		_bound[1] = value;
	}

	inline temporal_t days() const {
		return _bound[1] - _bound[0] + 1;
	}

	inline bool intersects(size_t lower, size_t upper) const {
		return (_bound[0] <= upper) && (_bound[1] >= lower);
	}

	inline bool contains(size_t lower, size_t upper) const {
		return (_bound[0] <= lower) && (_bound[1] >= upper);
	}

	friend std::ostream& operator<< (std::ostream& stream, const TemporalInterval& interval) {
		stream << interval.lower() << "/" << interval.upper();
		return stream;
	}

protected:
	std::array<temporal_t, 2>_bound{};
};
