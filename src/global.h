#pragma once
#include <fstream>
#include <string>
#include "GamesEngineeringBase.h"
using namespace GamesEngineeringBase;
using namespace std;

extern bool running;
extern bool aoeTriggered;
extern float heroProjectileSpeed;
extern float npcPorjectileSpeed;
extern int aoeNumber;

void saveString(ofstream& binout, const string& str);

string loadString(ifstream& infile);

bool checkImageCollision(Image& img1, unsigned int x1, unsigned int y1, Image& img2, unsigned int x2, unsigned int y2);