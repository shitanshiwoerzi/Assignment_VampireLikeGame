#include "world.h"

tile::tile() {}

void tile::load(std::string filename) {
	sprite.load(filename);
}
void tile::draw(GamesEngineeringBase::Window& canvas, int x, int y, Camera& cm) {
	Vector2D pos = Vector2D(x, y);
	renderImg(canvas, sprite, pos, cm);
}

bool tile::collide(LogicBase::hero& h, float newx, float newy) {
	auto& t = *this;
	if (checkImageCollision(h.sprite, h.pos.x + newx, h.pos.y + newy, t.sprite, t.x, t.y))
		return true;
	return false;
}

unsigned int tile::getHeight() { return sprite.height; }
unsigned int tile::getWidth() { return sprite.width; }
GamesEngineeringBase::Image& tile::getSprite() { return sprite; }



world::world(std::string filename) { // load from file name
	std::ifstream infile(filename);
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

			std::string filename = "Resources/tiles/" + std::to_string(num) + ".png";
			tiles[i][j].load(filename);
			pos++;
		}
	}

	infile.close();
}

void world::update(LogicBase::hero& h, float x, float y, float mapWidth, float mapHeight, Camera& cm) {
	bool collisionXDetected = false;
	bool collisionYDetected = false;

	for (int i = 0; i < tileshigh; i++) {
		for (int j = 0; j < tileswide; j++) {
			if (tiles[i][j].value <= 22 && tiles[i][j].value >= 14) {
				if (tiles[i][j].collide(h, x, 0)) {
					collisionXDetected = true;
				}
				if (tiles[i][j].collide(h, 0, y)) {
					collisionYDetected = true;
				}
				if (collisionXDetected && collisionYDetected) break;
			}
		}
		if (collisionXDetected && collisionYDetected) break;
	}

	if (!collisionXDetected) h.update(x, 0, mapWidth, mapHeight, cm);
	if (!collisionYDetected) h.update(0, y, mapWidth, mapHeight, cm);
}

// draw fixed map
void world::draw(GamesEngineeringBase::Window& canvas, Camera& cm) {
	for (int i = 0; i < tileshigh; i++) {
		if (i >= 0 && i < tileshigh) {
			for (int j = 0; j < tileswide; j++) {
				if (j >= 0 && j < tileswide) {
					tiles[i][j].draw(canvas, tilewidth * j, tileheight * i, cm);
					tiles[i][j].x = tilewidth * j;
					tiles[i][j].y = tileheight * i;
				}
			}
		}
	}
}

// draw infinite map
void world::draw(GamesEngineeringBase::Window& canvas, Camera& cm, int wx, int wy) {
	int r = wy % getMapHeight();
	int n = wx % getMapWidth();

	// get the offsets
	int xOffsets[3] = { -getMapWidth(), 0, getMapWidth() };
	int yOffsets[3] = { -getMapHeight(), 0, getMapHeight() };

	// get the hero's view range
	int viewportLeft = cm.m_Position.x - canvas.getWidth();
	int viewportRight = cm.m_Position.x + canvas.getWidth();
	int viewportTop = cm.m_Position.y - canvas.getHeight();
	int viewportBottom = cm.m_Position.y + canvas.getHeight();

	for (int yOffset : yOffsets) {
		for (int xOffset : xOffsets) {
			drawTiles(canvas, cm, n, r, xOffset, yOffset, viewportLeft, viewportRight, viewportTop, viewportBottom);
		}
	}
}

void world::drawTiles(GamesEngineeringBase::Window& canvas, Camera& cm, int n, int r, int xOffset, int yOffset,
	int viewportLeft, int viewportRight, int viewportTop, int viewportBottom) {
	for (int i = 0; i < tileshigh; ++i) {
		for (int j = 0; j < tileswide; ++j) {
			int tileX = tilewidth * j + n + xOffset;
			int tileY = tileheight * i + r + yOffset;
			if (tileX + tilewidth > viewportLeft && tileX < viewportRight && tileY + tileheight > viewportTop && tileY < viewportBottom) {
				tiles[i][j].draw(canvas, tileX, tileY, cm);
				tiles[i][j].x = tileX;
				tiles[i][j].y = tileY;
			}
		}
	}
}

int world::getMapWidth() {
	return tileswide * tilewidth;
}

int world::getMapHeight() {
	return tileshigh * tileheight;
}

void world::skipLine(std::ifstream& infile, int lineNumber) {
	std::string line;
	for (int i = 0; i < lineNumber; i++) {
		std::getline(infile, line);
	}
}