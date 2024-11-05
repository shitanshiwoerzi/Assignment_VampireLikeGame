#include "camera.h"

Camera::Camera(int w, int h) : m_Position(Vector2D()), width(w), height(h) {}

void Camera::update(float targetX, float targetY, float mapWidth, float mapHeight, GamesEngineeringBase::Image& sprite) {
	// Center the camera on the target (player)
	m_Position.x = targetX + sprite.width - width / 2;
	m_Position.y = targetY + sprite.height - height / 2;

	if (m_Position.x < 0) m_Position.x = 0;
	if (m_Position.y < 0) m_Position.y = 0;
	if (m_Position.x + width > mapWidth) m_Position.x = mapWidth - width;
	if (m_Position.y + height > mapHeight) m_Position.y = mapHeight - height;
}