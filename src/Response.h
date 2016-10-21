#pragma once

#include "stdafx.h"
#include "Pivot.h"

class Response {
public:
	Response() = default;
	Response(const response_container& container) : _container(container) {};

	~Response() = default;

	inline const response_container& container() const;

	inline void addElement(const Pivot* const pivot);

	template<class _Iter>
	inline void insert(_Iter begin, _Iter last);

	inline size_t size() const;

	inline response_iterator begin() const;
	inline response_iterator end() const;

	inline void swap(Response& right);

protected:
	response_container _container;
};

response_iterator Response::begin() const {
	return _container.begin();
}

response_iterator Response::end() const {
	return _container.end();
}

void Response::swap(Response& right) {
	_container.swap(right._container);
	right._container.clear();
}

const response_container& Response::container() const {
	return _container;
}

void Response::addElement(const Pivot* const pivot) {
	_container.emplace_back(pivot);
}

template<class _Iter>
void Response::insert(_Iter begin, _Iter last) {
	_container.insert(_container.end(), begin, last);
}

size_t Response::size() const {
	return _container.size();
}

