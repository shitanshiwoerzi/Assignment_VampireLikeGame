#include "GamesEngineeringBase.h"
#include "LogicBase.h"
#include "global.h"
#include <string>
#include "SavedGameHandling.h"
#include "world.h"

using namespace GamesEngineeringBase;
using namespace LogicBase;

int main() {
	Window canvas;
	canvas.create(1024, 768, "Vampire Survivor");
	Timer tim;
	int frameCount = 0;
	float elapsedTime = 0.0f;
	world w("Resources/tiles.txt");
	Camera cm = Camera(canvas.getWidth(), canvas.getHeight());
	hero h = hero(canvas.getWidth() / 2, canvas.getHeight() / 2, "Resources/hero_1.png");
	swarm s;
	items itm;
	string ss = "SavedData/test.dat";
	// start a new game or load game
	int choice;
	do {
		cout << "1.Start a new game   2.Load game" << endl;
		cin >> choice;
	} while (choice != 1 && choice != 2);

	if (choice == 2) {
		if (!fileExists(ss)) {
			do {
				cout << "There's no saved data." << endl;
				do {
					cout << "1.Start new game   2.Load game" << endl;
					cin >> choice;
				} while (choice != 1 && choice != 2);
			} while (choice != 1);
			level = getLevel();
		}
		else loadGame(ss, s, h, itm);
	}
	else if (choice == 1) level = getLevel();
	int wx = 0, wy = 0;

	// fixed map game loop
	if (level == 1) {
		while (running) {
			canvas.checkInput();
			canvas.clear();

			float dt = tim.dt();
			elapsedTime += dt;
			frameCount++;
			// update FPS each second
			if (elapsedTime >= 1.0f) {
				float fps = frameCount / elapsedTime;
				frameCount = 0;
				elapsedTime = 0.0f;

				std::cout << " FPS: " << fps << "  Score: " << score << std::endl;
			}

			int x = 0, y = 0;
			int move = static_cast<int>((400.f * dt));
			// update game logic
			if (canvas.keyPressed(VK_ESCAPE)) break;
			if (canvas.keyPressed('W')) y -= move;
			if (canvas.keyPressed('S')) y += move;
			if (canvas.keyPressed('A')) x -= move;
			if (canvas.keyPressed('D')) x += move;
			if (canvas.keyPressed('C')) {
				if (!saveTriggered) {
					saveGame(ss, s, h, itm);// save the game data
					std::cout << "Save success" << std::endl;
					saveTriggered = true;
				}
			}
			else saveTriggered = false;

			wx -= x; wy -= y;
			w.draw(canvas, cm);
			w.update(h, x, y, w.getMapWidth(), w.getMapHeight(), cm);

			s.update(canvas, dt, h, w.getMapWidth(), w.getMapHeight());
			h.hUpdate(canvas, x, y, dt, s, w.getMapWidth(), w.getMapHeight(), cm);
			itm.update(canvas, dt, h);

			itm.draw(canvas, cm);
			s.draw(canvas, cm);
			h.draw(canvas, cm);

			canvas.present();
			if (!running) {
				remove("SavedData/test.dat");
				break; // if game is over, quit the exe and remove the save data
			}
		}
	}

	// infinite map game loop(wait for updating)
	else if (level == 2) {
		while (running) {
			canvas.checkInput();
			canvas.clear();

			float dt = tim.dt();
			elapsedTime += dt;
			frameCount++;
			// update FPS each second
			if (elapsedTime >= 1.0f) {
				float fps = frameCount / elapsedTime;
				frameCount = 0;
				elapsedTime = 0.0f;

				std::cout << " FPS: " << fps << "  Score: " << score << std::endl;
			}

			int x = 0, y = 0;
			int move = static_cast<int>((400.f * dt));
			// update game logic
			if (canvas.keyPressed(VK_ESCAPE)) break;
			if (canvas.keyPressed('W')) y -= move;
			if (canvas.keyPressed('S')) y += move;
			if (canvas.keyPressed('A')) x -= move;
			if (canvas.keyPressed('D')) x += move;
			if (canvas.keyPressed('C')) {
				if (!saveTriggered) {
					saveGame(ss, s, h, itm);// save the game data
					std::cout << "Save success" << std::endl;
					saveTriggered = true;
				}
			}
			else saveTriggered = false;

			wx -= x; wy -= y;
			w.draw(canvas, cm, wx, wy);
			/*w.update(h, x, y, w.getMapWidth(), w.getMapHeight(), cm);*/

			s.update(canvas, dt, h, w.getMapWidth(), w.getMapHeight());
			h.hUpdate(canvas, x, y, dt, s, w.getMapWidth(), w.getMapHeight(), cm);
			itm.update(canvas, dt, h);

			itm.draw(canvas, cm);
			s.draw(canvas, cm);
			h.draw(canvas, cm);

			canvas.present();
			if (!running) {
				remove("SavedData/test.dat");
				break; // if game is over, quit the exe and remove the save data
			}
		}
	}
}