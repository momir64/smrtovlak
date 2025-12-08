#pragma once
class Color {
public:
	float red;
	float green;
	float blue;

	Color(float red, float green, float blue);
	Color(int red, int green, int blue);
};

class Bounds {
public:
	float x;
	float y;
	float width;

	Bounds(float x, float y, float width);
};

