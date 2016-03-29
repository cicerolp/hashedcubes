#include "stdafx.h"
#include "Types.h"
#include "Mercator.h"

Tile::Tile(int _x, int _y, ulong _z) : Tile::Tile((ulong)_x, (ulong)_y, _z) {}

Tile::Tile(ulong _x, ulong _y, ulong _z) : x(_x), y(_y), z(_z) {
	lat0 = util::tiley2lat(y, z);
	lon0 = util::tilex2lon(x, z);

	lat1 = util::tiley2lat(y + 1, z);
	lon1 = util::tilex2lon(x + 1, z);
}
