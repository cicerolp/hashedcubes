#pragma once

#include "Data.h"
#include "Query.h"
#include "Response.h"

template<class T> using node_container = std::vector<std::unique_ptr<T>>;
template<class T> using node_iterator = typename std::vector<std::unique_ptr<T>>::const_iterator;

template<class T, class KeyType>
class Hashing {
public:
	Hashing(const KeyType& key) : _key(key) { }

	Hashing(Hashing&&) = default;
	Hashing(const Hashing&) = delete;

	virtual ~Hashing() = default;

	Hashing& operator=(Hashing&&) = default;
	Hashing& operator=(const Hashing&) = delete;
	
	const KeyType& key() const { return _key; }
	
	virtual bool query(const Query& query, const Response& range, Response& response) const = 0;
	virtual size_t hash(const response_container& container, response_container& response, Data& data) = 0;

	inline void nextNode(const Pivot* range, node_iterator<T>& node) const {
		if (range->front() <= (*node)->front() && range->back() >= (*node)->front()) return;
		node = std::lower_bound(node, _nodes.end(), range->front(),
			[](const std::unique_ptr<T>& o1, const size_t& o2) { return o1->front() < o2; });
	}

protected:
	
	KeyType _key;
	node_container<T> _nodes;
};
