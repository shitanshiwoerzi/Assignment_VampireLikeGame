#pragma once
#include <fstream>
#include <string>
#include "GamesEngineeringBase.h"
#include "camera.h"
#include "LogicBase.h"
#include "global.h"

class tile {
public:
	int x;
	int y;
	GamesEngineeringBase::Image sprite;
	int value;

	tile();
	void load(std::string filename);
	void draw(GamesEngineeringBase::Window& canvas, int x, int y, Camera& cm);
	bool collide(LogicBase::hero& h, float newx, float newy);
	unsigned int getHeight();
	unsigned int getWidth();
	GamesEngineeringBase::Image& getSprite();
};

class world {
	int tileswide;
	int tileshigh;
	int tilewidth;
	int tileheight;
	tile** tiles;

public:
	world(std::string filename);
	void update(LogicBase::hero& h, float x, float y, float mapWidth, float mapHeight, Camera& cm);
	void draw(GamesEngineeringBase::Window& canvas, Camera& cm);
	void draw(GamesEngineeringBase::Window& canvas, Camera& cm, int wx, int wy);
	void drawTiles(GamesEngineeringBase::Window& canvas, Camera& cm, int n, int r, int xOffset, int yOffset,
		int viewportLeft, int viewportRight, int viewportTop, int viewportBottom);
	int getMapWidth();
	int getMapHeight();
private:
	void skipLine(std::ifstream& infile, int lineNumber);
};