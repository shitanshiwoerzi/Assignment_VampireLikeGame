#pragma once
#include <fstream>

class Health {
	float maxHp;
	float currentHp;
public:
	Health();
	Health(float _max);
	void takeDamage(float damage);
	void heal(float amount);
	bool isDead() const;
	float getHp() const;
	float getMaxHp() const;
	void save(std::ofstream& binout);
	void load(std::ifstream& infile);
};