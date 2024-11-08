#pragma once
#include <fstream>

class Vector2D {
public:
	float x;
	float y;

	Vector2D();

	Vector2D(float _x, float _y);

	float distance(Vector2D pos);

	Vector2D operator+(Vector2D const& vec); 

	Vector2D operator-(Vector2D const& vec);

	void save(std::ofstream& binout);

	void load(std::ifstream& infile);
};