#pragma once
#include <iostream>
#include <fstream>
#include "LogicBase.h"
#include "GamesEngineeringBase.h"
#include "global.h"
using namespace LogicBase;

void saveGame(string& filename, swarm& s, hero& h, items& itm) {
	ofstream binout(filename, ios::binary);
	h.save(binout);
	itm.save(binout);
	s.save(binout);
	binout.write((char*)&score, sizeof(score));
	binout.close();
}

void loadGame(string& filename, swarm& s, hero& h, items& itm) {
	ifstream infile(filename, ios::binary);
	if (!infile.is_open()) return;
	h.load(infile);
	itm.load(infile);
	s.load(infile);
	infile.read((char*)&score, sizeof(score));
	infile.close();
}