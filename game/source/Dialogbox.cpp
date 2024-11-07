#include "stdafx.h"
#include "DialogBox.h"

/*********** CONSTRUCTOR ***********/
Dialogbox::Dialogbox()
{
	dialogBoxTimer = 0;
	isDialogBoxOpen = false;
	dialogboxSprite.LoadImage("dialogboxSprite.png");
	dialogboxSprite.SetImage("dialogboxSprite.png");
	dialogboxSprite.SetSize(140, 50);
	textHolder = "";
}

/*********** UPDATE ***********/
void Dialogbox::Update(float gametime)
{
	if (isDialogBoxOpen && dialogBoxTimer < gametime)
		isDialogBoxOpen = false;
}

/*********** DRAW ***********/
void Dialogbox::Draw(CGraphics* g, CVector entityPos)
{
	if (isDialogBoxOpen)
	{
		dialogboxSprite.SetPos(entityPos);
		dialogboxSprite.Draw(g);
		*g << font("AFontPTSerif.ttf", 12) << color(CColor::Black()) << xy(entityPos.GetX() - 50, entityPos.GetY() - 5) << textHolder;
	}
}

/*********** SHOW ***********/
void Dialogbox::ShowDialogBox(string text, float gametime)
{
		isDialogBoxOpen = true;
		textHolder = text;
		dialogBoxTimer = gametime + 1500;
}