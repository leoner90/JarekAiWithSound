#pragma once
class UI
{
public:
	UI();
	void SetHpBar(float remainingHpPerc);
	void SetMpBar(float remainingHpPerc);
	void DrawUI(CGraphics* g, std::vector<bool> buffs, std::vector<float> buffRemainingTime);

private:
	//action bar
	CSprite underLayerSprite;
	CSprite actionBarSprite;
	CSprite hpBarSprite;
	CSprite mpBarSprite;

	//BUFFS
	CSprite speedbuffIcon;
	CSprite hideBuffIcon;
	CSprite cheeseObtainedIcon;
};