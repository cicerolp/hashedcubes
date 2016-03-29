#include "stdafx.h"
#include "Types.h"

#include "Mercator.h"

Tile::Tile(int _x, int _y, uint _z) : Tile::Tile((uint)_x, (uint)_y, _z) {}

Tile::Tile(uint _x, uint _y, uint _z) : x(_x), y(_y), z(_z) {
	lat = util::tiley2lat(y, z);
	lon = util::tilex2lon(x, z);
}