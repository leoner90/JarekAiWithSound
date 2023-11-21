#pragma once

class CMyGame : public CGame
{
	//Sprites
	CSprite player , cherry, apple, pear, banana, cherryRules, bananaRules, pearRules, appleRules;

	//Delay For banana sprite appearance after been caught or reached bottom of the screen
	float bananaDelayTime = 0;
	float bananaDelay = 3000;

	//pear , parameters for motion
	float radius = 4;
	float angle = 0;

	//random position for given sprite
	void UpdateFruitPos(CSprite &sprite);

	//Create Sprites
	void spriteCreator(CSprite& sprite, char * spritePath);
	
	// music and sounds
	CSoundPlayer mainbBgMusic, catchSound, resetSound, scaleDownSize, gameOver;
	
	//score
	int score;		 

public:
	CMyGame(void);	 // Constructor
	~CMyGame(void);  // Destructor

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
