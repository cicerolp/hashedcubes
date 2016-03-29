#pragma once

#include "Pivot.h"
#include "TemporalInterval.h"

class TemporalPivot : public Pivot {
public:
	TemporalPivot(ulong first, ulong second, const temporal_t& value) : Pivot(first, second), _value(value) { }
	TemporalPivot(const Pivot& pivot, const temporal_t& value) : Pivot(pivot), _value(value) { }
	~TemporalPivot() = default;

	inline const temporal_t& value() const;

protected:
	temporal_t _value;
};

const temporal_t& TemporalPivot::value() const {
	return _value;
}
