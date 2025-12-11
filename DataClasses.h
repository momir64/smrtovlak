#pragma once
class Color {
public:
	float red;
	float green;
	float blue;

	Color(float red = 0, float green = 0, float blue = 0);
	Color(int red, int green, int blue);
};

class Coords {
public:
	float x;
	float y;

	Coords(float x = 0, float y = 0);
};

class Bounds : public Coords {
public:
	float x;
	float y;
	float width;
	float height;
	float angle;
	bool flip;

	Bounds(float x, float y, float width, float height = 0, float angle = 0, bool flip = false);
	Bounds(Coords coords);
};

class Size {
public:
	float width;
	float height;

	Size(float width = 0, float height = 0);
};

class PointStats {
public:
	bool firstHalf = false;
	float distance = 0;
	float angle = 0;
	Coords crds;
};