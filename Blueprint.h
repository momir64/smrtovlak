#pragma once
#include "WindowManager.h"
#include "InputListener.h"
#include "DataClasses.h"
#include "PulseEngine.h"
#include "LineEngine.h"
#include "Button.h"
#include <vector>
#include <string>

class Blueprint : public MouseListener, KeyboardListener, ButtonListener {
    int hoveringPulse = 0, startingPulse = 0;
    std::vector<PointStats>& points;
    std::vector<Coords>& drawing;
    bool drawingActive = true;
	std::string trackPath;
    WindowManager& window;
    PulseEngine pulse;
    LineEngine line;
    Button trash;

    Bounds platformBounds() const;
    Bounds contentBounds() const;

    Coords clipToContent(float mx, float my) const;
    bool insideContent(float mx, float my) const;
    int detectPulse(float mx, float my) const;
    Coords toLocal(float mx, float my) const;

    void drawPlatform();
    void drawDrawing();
    void drawPulses();
    void drawBorder();
    void drawGrid();

    void finalizeAndCloseLine();
    void computePoints();
    void saveTrack();

public:
    Blueprint(WindowManager& window, std::vector<Coords>& tracks, std::vector<PointStats>& points, const std::string& trackPath);

    void loadTrack();
    void draw();

    void keyboardCallback(GLFWwindow&, int key, int scancode, int action, int mods) override;
    void mouseCallback(double x, double y, int button, int action, int mods) override;
    void buttonChanged(Button&, int selected) override;
    void cursorCallback(double x, double y) override;
};
