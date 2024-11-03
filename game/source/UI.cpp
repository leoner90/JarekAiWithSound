#include "stdafx.h"
#include "UI.h"

UI::UI()
{
	actionBarSprite.LoadImage("actionBar.png");
	actionBarSprite.SetImage("actionBar.png");
	actionBarSprite.SetSize(1024, 160);
	actionBarSprite.SetPosition(512, 80);


	//hpBar 130/65 origin
	hpBarSprite.LoadImage("hpBar.png");
	hpBarSprite.SetImage("hpBar.png");
	hpBarSprite.SetSize(160, 80);
	hpBarSprite.SetPosition(80, 40);

	//mpBar
	mpBarSprite.LoadImage("mpBar.png");
	mpBarSprite.SetImage("mpBar.png");
	mpBarSprite.SetSize(160, 80);
	mpBarSprite.SetPosition(950, 40);

	//under  Layer
	underLayerSprite.LoadImage("underLayer.png");
	underLayerSprite.SetImage("underLayer.png");
	underLayerSprite.SetSize(1024, 120);
	underLayerSprite.SetPosition(512, 60);

	//bufs

	hideBuffIcon.LoadImage("hiddenBuff.png");
	hideBuffIcon.SetImage("hiddenBuff.png");
	hideBuffIcon.SetSize(45, 45);
	hideBuffIcon.SetPosition(90, 120);

	speedbuffIcon.LoadImage("speedUpBuff.png");
	speedbuffIcon.SetImage("speedUpBuff.png");
	speedbuffIcon.SetSize(45, 45);
	speedbuffIcon.SetPosition(145, 120);

	cheeseObtainedIcon.LoadImage("Cheese.png");
	cheeseObtainedIcon.SetImage("Cheese.png");

	cheeseObtainedIcon.SetSize(30, 25);
	cheeseObtainedIcon.SetPosition(960, 98);
}


UI::~UI()
{
}

void UI::SetHpBar(float remainingHpPerc)
{
	float currentSize = 80.f * remainingHpPerc;
	hpBarSprite.SetSize(160, currentSize);
	hpBarSprite.SetPos(80, currentSize / 2);
}

void UI::SetMpBar(float remainingMpPerc)
{
	float currentSize = 80.f * remainingMpPerc;
	mpBarSprite.SetSize(160, currentSize);
	mpBarSprite.SetPos(950, currentSize / 2);
}

void UI::DrawUI(CGraphics* g, std::vector<bool> buffs, std::vector<float> buffRemainingTime)
{
	underLayerSprite.Draw(g);
	hpBarSprite.Draw(g);
	mpBarSprite.Draw(g);
	actionBarSprite.Draw(g);


	//buffs
	
	if (buffs[0])
	{
		float buffTime = -buffRemainingTime[0] > 0 ? -buffRemainingTime[0] : 0;// in case if light is off

		hideBuffIcon.Draw(g);
		if(buffTime) 
			*g << font("AFontPTSerif.ttf", 12) << color(CColor::White()) << xy(75, 85) << buffTime;
	}
	if (buffs[1])
	{
		speedbuffIcon.Draw(g);
		*g << font("AFontPTSerif.ttf", 12) << color(CColor::White()) << xy(130, 85) << -buffRemainingTime[1];
	}

	if (buffs[2]) cheeseObtainedIcon.Draw(g);
}
