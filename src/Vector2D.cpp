#include "Vector2D.h"

Vector2D::Vector2D() {
	x = 0;
	y = 0;
}

Vector2D::Vector2D(float _x, float _y) {
	x = _x;
	y = _y;
}

// return the distance between two points
float Vector2D::distance(Vector2D pos) {
	return sqrtf(pow(x - pos.x, 2) + pow(y - pos.y, 2));
}

Vector2D Vector2D::operator+(Vector2D const& vec) {
	Vector2D res;
	res.x = x + vec.x;
	res.y = y + vec.y;
	return res;
}

Vector2D Vector2D::operator-(Vector2D const& vec) {
	Vector2D res;
	res.x = x - vec.x;
	res.y = y - vec.y;
	return res;
}

void Vector2D::save(std::ofstream& binout) {
	binout.write((char*)&x, sizeof(x));
	binout.write((char*)&y, sizeof(y));
}

void Vector2D::load(std::ifstream& infile) {
	infile.read((char*)&x, sizeof(x));
	infile.read((char*)&y, sizeof(x));
}