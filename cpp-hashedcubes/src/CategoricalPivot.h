#pragma once

#include "Pivot.h"
#include "Types.h"

class CategoricalPivot : public Pivot {
public:
	CategoricalPivot(ulong first, ulong second, const categorical_t& value) : Pivot(first, second), _value(value) { }
	CategoricalPivot(const Pivot& pivot, const categorical_t& value) : Pivot(pivot), _value(value) { }
	~CategoricalPivot() = default;

	inline const categorical_t& value() const;

protected:
	categorical_t _value;
};

const categorical_t& CategoricalPivot::value() const {
	return _value;
}
