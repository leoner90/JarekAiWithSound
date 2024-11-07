#pragma once
#include "Map/Map.h"
#include "Player.h"
#include "Enemy.h"

class CMyGame : public CGame
{
private:
	//Functions
	void EnemyCreator();
	void menuHandler(CGraphics* g);

	// main Objects
	Map mainMap;
	Player player;
	vector<Enemy*> AllEnemies;

	//Prefabs  
	CSprite* catSpritePrefab;
	CSprite* cat2SpritePrefab;
	CSprite* dogSpritePrefab;
	CSprite* humanSpritePrefab;


	CSprite startScreen;
	CSprite mainMenuSelectionLogo;
	//game Over
	CSprite gameOverBg;
	CSprite gameWinBg;

	//Game Conditions
	float deadScreenTimer;
	bool gameStarted;
	bool IsGameWon;
	bool gameOver;

	//MENU Handler
	int currentMenuState;
	int startScreenSelection;
	bool showControllImg;
	enum gameStates { MENU, PAUSE, INGAME };
	enum menuScreenSelection { CONTINUE, NEWGAME, CONTROLS, EXIT, BACK };


	//sounds
	bool isMainMusicPlayong; // native Functions dosrn't work at all
	CSoundPlayer  mainBgMusic;
	CSoundPlayer  loseSound;
	CSoundPlayer  winSound;

public:
	CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnStartGame();
	virtual void OnGameOver();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
};