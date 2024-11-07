#pragma once

class Dialogbox
{
public:
	Dialogbox();
	void Update(float gametime);
	void Draw(CGraphics* g, CVector entityPos);
	void ShowDialogBox(string text, float gametime);

private:
	CSprite dialogboxSprite;
	string textHolder;
	
	//open & close conditions
	float dialogBoxTimer;
	bool isDialogBoxOpen;
};