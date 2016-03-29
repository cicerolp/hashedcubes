#pragma once

#include "Types.h"
#include "Pivot.h"

class TilePivot : public Pivot {
public:
	TilePivot() = default;

	TilePivot(ulong first, ulong second, const tile_t& value) : Pivot(first, second), _value(value) { }
	TilePivot(const Pivot& pivot, const tile_t& value) : Pivot(pivot), _value(value) { }
	~TilePivot() = default;

	inline const tile_t& value() const;

	inline bool last() const;
	inline void setLast();

protected:

	tile_t _value;
};

const tile_t& TilePivot::value() const {
	return _value;
}

bool TilePivot::last() const {
	return _value.l == 1;
}

void TilePivot::setLast() {
	_value.l = 1;
}
