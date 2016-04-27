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

TileBounds::TileBounds(ulong _x0, ulong _y0, ulong _x1, ulong _y1, ulong _z) 
		: x0(_x0), y0(_y0), x1(_x1), y1(_y1), z(_z) {

	lat0 = util::tiley2lat(y0, z);
	lon0 = util::tilex2lon(x0, z);

	lat1 = util::tiley2lat(y1 + 1, z);
	lon1 = util::tilex2lon(x1 + 1, z);
}
