#include "world.h"
#include "global.h"

tile::tile() {}

void tile::load(std::string filename) {
	sprite.load(filename);
}
void tile::draw(GamesEngineeringBase::Window& canvas, int x, int y, Camera& cm) {
	Vector2D pos = Vector2D(x,y);
	renderImg(canvas, sprite, pos, cm);
}
unsigned int tile::getHeight() { return sprite.height; }
unsigned int tile::getWidth() { return sprite.width; }
GamesEngineeringBase::Image& tile::getSprite() { return sprite; }



world::world(string filename) { // load from file name
	ifstream infile(filename);
	std::string line;
	infile >> line;
	if (line == "tileswide") infile >> tileswide;
	infile >> line;
	if (line == "tileshigh") infile >> tileshigh;
	infile >> line;
	if (line == "tilewidth") infile >> tilewidth;
	infile >> line;
	if (line == "tileheight") infile >> tileheight;

	tiles = new tile * [tileshigh];
	for (int i = 0; i < tileshigh; ++i) {
		tiles[i] = new tile[tileswide];
	}

	skipLine(infile, 3);

	for (int i = 0; i < tileshigh; i++) {
		getline(infile, line);
		int pos = 0;
		for (int j = 0; j < tileswide; j++) {
			int num = 0;
			while (line[pos] != ',' && line[pos] != '\0') {
				num = num * 10 + line[pos] - '0';
				tiles[i][j].value = num;
				pos++;
			}

			string filename = "Resources/tiles/" + to_string(num) + ".png";
			tiles[i][j].load(filename);
			pos++;
		}
	}

	for (int i = 0; i < tileshigh; ++i) {
		for (int j = 0; j < tileswide; ++j) {
			std::cout << tiles[i][j].value << " ";
		}
		std::cout << std::endl;
	}

	infile.close();
}

void world::draw(GamesEngineeringBase::Window& canvas, Camera& cm) {
	for (int i = 0; i < tileshigh; ++i) {
		for (int j = 0; j < tileswide; ++j) {
			if (i >= 0 && i < tileshigh && j >= 0 && j < tileswide)
				tiles[i][j].draw(canvas, tilewidth * j, tileheight * i, cm);
		}
	}
}

int world::getMapWidth() {
	return tileswide * tilewidth;
}

int world::getMapHeight() {
	return tileshigh * tileheight;
}

void world::skipLine(ifstream& infile, int lineNumber) {
	string line;
	for (int i = 0; i < lineNumber; i++) {
		getline(infile, line);
	}
}