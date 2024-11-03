#include "stdafx.h"
#include "DialogBox.h"

Dialogbox::Dialogbox()
{
	dialogBoxTimer = 0;
	isDialogBoxOpen = false;
	dialogboxSprite.LoadImage("dialogboxSprite.png");
	dialogboxSprite.SetImage("dialogboxSprite.png");
	dialogboxSprite.SetSize(140, 50);
	textHolder = "";

	//dialogboxSprite.SetPosition(512, 80);
	
}

Dialogbox::~Dialogbox()
{
}

void Dialogbox::Update(float gametime)
{
	if (isDialogBoxOpen && dialogBoxTimer < gametime)
		isDialogBoxOpen = false;
}

void Dialogbox::Draw(CGraphics* g, CVector entityPos)
{
	if (isDialogBoxOpen)
	{
		dialogboxSprite.SetPos(entityPos);
		dialogboxSprite.Draw(g);
		*g << font("AFontPTSerif.ttf", 12) << color(CColor::Black()) << xy(entityPos.GetX() - 50, entityPos.GetY() - 5) << textHolder;
	}
}

void Dialogbox::ShowDialogBox(string text, float gametime)
{
	if (textHolder != text)
	{
		isDialogBoxOpen = true;
		textHolder = text;
		dialogBoxTimer = gametime + 1500;
	}
	
}
