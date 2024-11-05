#pragma once
#include <fstream>
#include <string>
#include "GamesEngineeringBase.h"
#include "LogicBase.h"

class tile {
	GamesEngineeringBase::Image sprite;
public:
	int value;

	tile();
	void load(std::string filename);
	void draw(GamesEngineeringBase::Window& canvas, int x, int y, Camera& cm);
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
	world(string filename);
	void draw(GamesEngineeringBase::Window& canvas, Camera& cm);
	int getMapWidth();
	int getMapHeight();
private:
	void skipLine(ifstream& infile, int lineNumber);
};