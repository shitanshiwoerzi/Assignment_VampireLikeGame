#include "health.h"

Health::Health() {}

Health::Health(float _max) {
	maxHp = _max;
	currentHp = maxHp;
}

void Health::takeDamage(float damage) {
	currentHp -= damage;
	if (currentHp < 0) currentHp = 0;
}

void Health::heal(float amount) {
	currentHp += amount;
	if (currentHp > maxHp) currentHp = maxHp;
}

bool Health::isDead() const {
	return currentHp <= 0;
}

float Health::getHp() const {
	return currentHp;
}

float Health::getMaxHp() const {
	return maxHp;
}

void Health::save(std::ofstream& binout) {
	binout.write((char*)&maxHp, sizeof(maxHp));
	binout.write((char*)&currentHp, sizeof(currentHp));
}

void Health::load(std::ifstream& infile) {
	infile.read((char*)&maxHp, sizeof(maxHp));
	infile.read((char*)&currentHp, sizeof(currentHp));
}