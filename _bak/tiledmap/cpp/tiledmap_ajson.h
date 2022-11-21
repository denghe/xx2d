#pragma once
#include <tiledmap.h>
#include <ajson.hpp>
AJSON(::tiledmap_original::Property, name, type, value);
AJSON(::tiledmap_original::Point, x, y);
AJSON(::tiledmap_original::Chunk, data, height, width, x, y);
AJSON(::tiledmap_original::Object, ellipse, gid, height, id, name, point, polygon, polyline, properties, rotation, template_, text, type, visible, width, x, y);
AJSON(::tiledmap_original::Layer, chunks, compression, data, draworder, encoding, height, id, image, layers, name, objects, offsetx, offsety, opacity, parallaxx, parallaxy, properties, startx, starty, tintcolor, transparentcolor, type, visible, width, x, y);
AJSON(::tiledmap_original::Frame, duration, tileid);
AJSON(::tiledmap_original::WangTile, tileId, wangid);
AJSON(::tiledmap_original::WangColor, color, name, probability, properties, tile);
AJSON(::tiledmap_original::Grid, height, orientation, width);
AJSON(::tiledmap_original::Terrain, name, properties, tile);
AJSON(::tiledmap_original::TileOffset, x, y);
AJSON(::tiledmap_original::Tile, animation, id, image, imageheight, imagewidth, objectgroup, probability, properties, terrain, type);
AJSON(::tiledmap_original::Transformations, hflip, vflip, rotate, preferuntransformed);
AJSON(::tiledmap_original::WangSet, colors, name, properties, tile, wangtiles);
AJSON(::tiledmap_original::Tileset, backgroundcolor, columns, firstgid, grid, image, imageheight, imagewidth, margin, name, objectalignment, properties, source, spacing, terrains, tilecount, tiledversion, tileheight, tileoffset, tiles, tilewidth, transformations, transparentcolor, type, version, wangsets);
AJSON(::tiledmap_original::Map, backgroundcolor, compressionlevel, height, hexsidelength, infinite, layers, nextlayerid, nextobjectid, orientation, properties, renderorder, staggeraxis, staggerindex, tiledversion, tileheight, tilesets, tilewidth, type, version, width);
