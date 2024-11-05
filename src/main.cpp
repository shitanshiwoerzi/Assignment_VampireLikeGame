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
	hero h = hero(canvas.getWidth() / 2, canvas.getHeight() / 2, "Resources/hero.png");
	swarm s;
	items itm;
	string ss = "SavedData/test.dat";
	//load the game data from test.dat
	//LoadGame(ss, s, h, itm);
	while (running) {
		canvas.checkInput();
		canvas.clear();

		float dt = tim.dt();
		elapsedTime += dt;
		frameCount++;
		// 每秒更新一次 FPS
		if (elapsedTime >= 1.0f) {
			float fps = frameCount / elapsedTime;
			frameCount = 0;
			elapsedTime = 0.0f;

			// 输出FPS
			std::cout << " FPS: " << fps << std::endl;
		}

		int x = 0, y = 0;
		int move = static_cast<int>((500.f * dt));
		// Update game logic
		if (canvas.keyPressed(VK_ESCAPE)) break;
		if (canvas.keyPressed('W')) y -= move;
		if (canvas.keyPressed('S')) y += move;
		if (canvas.keyPressed('A')) x -= move;
		if (canvas.keyPressed('D')) x += move;

		s.swarmUpdate(canvas, dt, h);
		h.heroUpdate(canvas, x, y, dt, s, w.getMapWidth(), w.getMapHeight(), cm);
		itm.update(canvas, dt, h);

		w.draw(canvas, cm);
		itm.draw(canvas, cm);
		s.draw(canvas, cm);
		h.draw(canvas, cm);

		canvas.present();
		if (!running)
			break; // (待添加，弹出游戏结束的提示窗口)
	}

	// save the game data
	SaveGame(ss, s, h, itm);
}