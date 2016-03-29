#include "stdafx.h"
#include "Data.h"

void Data::initialize(ulong size) {
	_hash = data_container<unsigned short>::result(size);
	_index = data_container<ulong>::result(size);

	for (ulong i = 0; i < _index.size(); i++) {
		_hash[i] = 0;
		_index[i] = i;
	}
}

void Data::sortHash(ulong fromIndex, ulong toIndex) {
	const auto functor = std::bind(&Data::comparator, this, std::placeholders::_1, std::placeholders::_2);
	std::sort(_index.begin() + fromIndex, _index.begin() + toIndex, functor);
}

void Data::setHash(ulong id, unsigned short value) {
	_hash[_index[id]] = value;
}

unsigned long Data::getHash(ulong index) const {
	return _hash[index];
}


/* Local Variables:    */
/* mode: c             */
/* indent-tabs-mode: t */
/* c-basic-offset: 8   */
/* tab-width: 8        */
/* End:                */
