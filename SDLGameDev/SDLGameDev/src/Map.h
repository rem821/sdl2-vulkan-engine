#pragma once

#include "CustomTypes.h"
#include "TextureManager.h"
#include "IsoEngine.h"
#include "fmt/core.h"

#define NUM_ISOMETRIC_TILES 101
#define TILE_INPUT_SIZE 64

#define MAP_HEIGHT 20
#define MAP_WIDTH 20

class Map {
	public:
	Map(SDL_Renderer* renderer, bool textureSheet);
	~Map();

	void updateIsoMap(Point2DT cameraPos, int scrollAmount);
	void drawIsoMap();
	void drawCursor(SDL_Rect mouseRect);

	void drawIsoCursor(SDL_Rect mouseRect, Point2DT cameraPos);

	private:
	SDL_Renderer* renderer;
	IsoEngine* isoEngine;

	TextureT tilesTexSheet;
	SDL_Rect tilesRect[NUM_ISOMETRIC_TILES];

	TextureT tilesTex[NUM_ISOMETRIC_TILES];
	TileT tiles[MAP_WIDTH][MAP_HEIGHT];

	int TILE_OUTPUT_SIZE = 256;

	int level[MAP_WIDTH][MAP_HEIGHT] = {
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0},
	{1,1,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,1,0}
	};

	bool textureSheet;
};