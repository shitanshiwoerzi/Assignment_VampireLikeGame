#include "GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

int main() {
	Window canvas;
	canvas.create(1920,1080, "Vampire Survivor");
	bool running = true;
	while (running) {
		canvas.checkInput();


		canvas.clear();
	}

	canvas.present();
}