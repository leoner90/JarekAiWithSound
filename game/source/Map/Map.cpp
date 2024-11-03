#include "stdafx.h"
#include "Map/Map.h"

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


Map::Map()
{
	globalLight = true;
	gameBg.LoadImage("mainMapbg.jpg");
	gameBg.SetImage("mainMapbg.jpg");
	gameBg.SetSize(2000, 2000);
	gameBg.SetPosition(1000, 1000);

	gameBgNoLight.LoadImage("mainMapbgNoLight.jpg");
	gameBgNoLight.SetImage("mainMapbgNoLight.jpg");
	gameBgNoLight.SetSize(2000, 2000);
	gameBgNoLight.SetPosition(1000, 1000);

	
 

	ObjCoord allObj[] =
	{
		ObjCoord(230,330,85,225),//toilet
		ObjCoord(483,760,312,456), // bath
		ObjCoord(455,565,155,245),// table toilet
		ObjCoord(270,390,666,968), // sofa first room
		ObjCoord(480,650,688,968), // desk first room
		ObjCoord(810,1062,0,766),
		ObjCoord(810,1062,890,1500),
		ObjCoord(1294,1430,430,640),
		ObjCoord(1755,1885,430,640),
		ObjCoord(1455,1740,236,380),
		ObjCoord(800,1666,0,190),
		ObjCoord(1305,2000,680,1340), //kitchen wall
		ObjCoord(414,814,455,540), //samll wall

		//main map side walls
		ObjCoord(0,220,0,2000), //left
		ObjCoord(1900,2000,0,2000), //right
		ObjCoord(0,1050,1076,2000), //top left
		ObjCoord(1050,2000,1477,2000), //top right
		ObjCoord(0,2000,0,50), //bottom
	};

	for (ObjCoord x : allObj)
	{
		CSprite* newObj = new CSprite();
		newObj->LoadImage("testobstacles.jpg");
		newObj->SetImage("testobstacles.jpg");
		newObj->SetSize(x.w, x.h);
		newObj->SetPosition(x.pivotPoint);
		checkObects.push_back(newObj);
	}
 
}


Map::~Map()
{
}

void Map::Draw(CGraphics* g, CVector playerPos)
{
	//map Scrolling
	if (playerPos.GetX() < leftScreenLimit)
	{
		scrollOffset = 0;
		g->SetScrollPos(0, 130);
	}

	else if (playerPos.GetX() >= leftScreenLimit && playerPos.GetX() <= rightScreenLimit)
	{
		scrollOffset = leftScreenLimit - playerPos.GetX();
		g->SetScrollPos(leftScreenLimit - playerPos.GetX(), 130);
	}
	else if (playerPos.GetX() > rightScreenLimit - leftScreenLimit - 800)
	{
		scrollOffset = leftScreenLimit - rightScreenLimit;
		g->SetScrollPos(leftScreenLimit - rightScreenLimit, 130);
	}

	if (playerPos.GetY() < bottomScrollLimit)
	{
		scrollOffset = 0;
		g->SetScrollPos(g->GetScrollPos().X(), 130);
	}
	else if (playerPos.GetY() >= bottomScrollLimit && playerPos.GetY() <= topScrollLimit)
	{
		scrollOffset = bottomScrollLimit - playerPos.GetY();
		g->SetScrollPos(g->GetScrollPos().X(), bottomScrollLimit - playerPos.GetY() + 130);
	 
	}
	else if (playerPos.GetY() > topScrollLimit - bottomScrollLimit - 800)
	{
		scrollOffset = bottomScrollLimit - topScrollLimit;
		g->SetScrollPos(g->GetScrollPos().X(), bottomScrollLimit - topScrollLimit + 130);
	}

	currentScrollOffset = g->GetScrollPos();
	if(globalLight)
		gameBg.Draw(g);
	else
		gameBgNoLight.Draw(g);
	//for (auto obj : checkObects) obj->Draw(g);
	
}
 



