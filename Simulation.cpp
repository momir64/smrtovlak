#include "Simulation.h"
#include "WindowManager.h"
#include "DataClasses.h"

Simulation::Simulation(WindowManager& window) :
	background(window, "assets/background.png"), line(window), window(window) {
}

void Simulation::draw() {
	background.draw(Bounds(0.5f, 0.f, 1.f));
}
