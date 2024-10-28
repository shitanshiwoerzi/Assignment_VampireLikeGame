#include "GamesEngineeringBase.h"
#include "LogicBase.h"
using namespace GamesEngineeringBase;
using namespace LogicBase;

int main() {
	Window canvas;
	canvas.create(1024, 768, "Vampire Survivor");
	Image background;
	background.load("Resources/grass.jpeg");
	Timer tim;
	hero h = hero(canvas.getWidth() / 2, canvas.getHeight() / 2, "Resources/hero.png");
	swarm s;
	Camera cm = Camera(canvas.getWidth(), canvas.getHeight());
	bool running = true;
	int start_x = 0;
	int start_y = 0;
	while (running) {
		canvas.checkInput();
		canvas.clear();

		float dt = tim.dt();
		int x = 0, y = 0;
		int move = static_cast<int>((500.f * dt));
		// Update game logic
		if (canvas.keyPressed(VK_ESCAPE)) break;
		if (canvas.keyPressed('W')) y -= move;
		if (canvas.keyPressed('S')) y += move;
		if (canvas.keyPressed('A')) x -= move;
		if (canvas.keyPressed('D')) x += move;

		s.update(canvas, dt, h);
		h.update(canvas, x, y, dt);
		cm.update(h.pos.x, h.pos.y);

		// npc与hero的碰撞检测
		s.checkCollision(canvas, h);

		// hero发射的projectile碰撞检测
		h.checkCollision(canvas, s);

		// Draw(); 
		for (int i = 0; i < canvas.getWidth(); i++)
			for (int j = 0; j < canvas.getHeight(); j++)
				canvas.draw(i, j, background.at(i, j));

		s.draw(canvas);
		h.draw(canvas);

		canvas.present();
	}
}