#pragma once

class Dialogbox
{
public:
	Dialogbox();
	~Dialogbox();
	void Update(float gametime);
	void Draw(CGraphics* g, CVector entityPos);
	void ShowDialogBox(string text, float gametime);
	CSprite dialogboxSprite;
	float dialogBoxTimer;
	bool isDialogBoxOpen;
	string textHolder;

private:

};