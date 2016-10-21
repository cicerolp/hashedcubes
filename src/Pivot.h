#pragma once

class Pivot {
public:
	Pivot() = default;

	Pivot(ulong size) : Pivot(0, size) { };
	Pivot(ulong first, ulong second) : _pivot({first, second}) { };

	Pivot(const Pivot&) = default;

	virtual ~Pivot() = default;

	inline bool empty() const;
	inline ulong size() const;

	inline ulong front() const;
	inline ulong back() const;

	inline bool operator< (const Pivot& other) const;
	inline bool operator> (const Pivot& other) const;

	// implicit conversion
	inline operator const Pivot*() const;

	inline bool endAfter(const Pivot& other) const;
	inline bool endAfter(const ulong other) const;
	inline bool endBefore(const Pivot& other) const;

protected:
	std::array<ulong, 2> _pivot;
};

using pivot_container = std::vector<Pivot*>;
using pivot_iterator = pivot_container::const_iterator;

using response_container = std::vector<const Pivot*>;
using response_iterator = response_container::const_iterator;


bool Pivot::empty() const {
	return (back() - front()) == 0;
}

ulong Pivot::size() const {
	return back() - front();
}

ulong Pivot::front() const {
	return _pivot[0];
}

ulong Pivot::back() const {
	return _pivot[1];
}

bool Pivot::operator<(const Pivot& other) const {
	return back() < other.front();
}

bool Pivot::operator>(const Pivot& other) const {
	return back() > other.front();
}

Pivot::operator const Pivot*() const {
	return this;
}

bool Pivot::endAfter(const ulong other) const {
	return back() > other;
}

bool Pivot::endAfter(const Pivot& other) const {
	return back() > other.front();
}

bool Pivot::endBefore(const Pivot& other) const {
	return back() <= other.front();
}