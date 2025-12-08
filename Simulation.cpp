#include "Simulation.h"
#include "DataClasses.h"
#include "WindowManager.h"

Simulation::Simulation(WindowManager& window) : background(window, "assets/background.png") {}

void Simulation::draw() {
	background.draw(Bounds(0.5f, 0.f, 1.f));
}
