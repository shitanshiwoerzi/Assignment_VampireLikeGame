#pragma once
#include "Vector2D.h"
#include "GamesEngineeringBase.h"

class Camera {
public:
	Vector2D m_Position;

	float width;
	float height;

	Camera(int w, int h);
	void update(float targetX, float targetY, float mapWidth, float mapHeight, GamesEngineeringBase::Image& sprite);
};