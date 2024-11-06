#pragma once
#include <iostream>
#include <fstream>
#include "LogicBase.h"
#include "GamesEngineeringBase.h"
#include "global.h"
using namespace LogicBase;

void SaveGame(string& filename, swarm& s, hero& h, items& itm) {
	ofstream binout(filename, ios::binary);
	h.save(binout);
	itm.save(binout);
	s.save(binout);
	binout.close();

}

void LoadGame(string& filename, swarm& s, hero& h, items& itm) {
	ifstream infile(filename, ios::binary);
	h.load(infile);
	itm.load(infile);
	s.load(infile);
	infile.close();
}