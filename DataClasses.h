#pragma once
class Color {
public:
	float red;
	float green;
	float blue;

	Color(float red = 0, float green = 0, float blue = 0);
	Color(int red, int green, int blue);
};

class Bounds {
public:
	float x;
	float y;
	float width;
	float height;

	Bounds(float x, float y, float width, float height = 0);
};

class Coords {
public:
	float x;
	float y;

	Coords(float x = 0, float y = 0);
};

class Size {
public:
	float width;
	float height;

	Size(float width = 0, float height = 0);
};