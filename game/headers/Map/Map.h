#pragma once

class Map
{
public:
	Map();
	void Draw(CGraphics* g, CVector playerPos);

	//Getters
	bool GetGlobalLightOn();
	CVector GetCurrentScrollOffset();
	CSpriteList GetAllObstacles();

	//Setters
	void SetGlobalLight(bool state);
private:
	//limits and map scrolling
	int scrollOffset = 0;
	int leftScreenLimit = 300;
	int rightScreenLimit = 1238;
	int topScrollLimit = 1488;
	int bottomScrollLimit = 300;

	//Sprites
	CSprite gameBg;
	CSprite gameBgNoLight;

	//Global Light
	bool globalLight;

	//Map Offsets
	CVector currentScrollOffset;

	//All obstacles
	CSpriteList checkObects;
};

//___________ STRUCTS___________
//*** Creates struct by given coordinates, which are  used as a obstacles in the game ***
struct ObjCoord
{
	float x_min;
	float x_max;
	float y_min;
	float y_max;
	CVector pivotPoint;
	float w;
	float h;
	ObjCoord(float x1, float x2, float y1, float y2)
	{
		x_min = x1;
		x_max = x2;
		y_min = y1;
		y_max = y2;

		pivotPoint = CVector(x1 + (x2 - x1) / 2, y1 + (y2 - y1) / 2);
		w = x2 - x1;
		h = y2 - y1;
	}
};