#pragma once
#include "Map/Map.h"
#include "Player.h"
#include "Enemy.h"


class CMyGame : public CGame
{
	Map mainMap;
	Player player;
	vector<Enemy*> AllEnemies;

public:
	CMyGame(void);
	//~CMyGame(void);

	void EnemyCreator();

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
 
	void menuHandler(CGraphics* g);
	void initSpritesHandler();

	//prefabs  
	CSprite* catSpritePrefab;
	CSprite* cat2SpritePrefab;
	CSprite* dogSpritePrefab;
	CSprite* humanSpritePrefab;


	CSprite startScreen;
	CSprite mainMenuSelectionLogo;
	//game Over
	CSprite gameOverBg;
	CSprite gameWinBg;
	bool gameOver;
	float deadScreenTimer;

	bool gameStarted;
	bool IsGameWon;
	bool showControllImg;


	int currentMenuState;
	enum gameStates {MENU, PAUSE, INGAME};

	int startScreenSelection;
	enum menuScreenSelection { CONTINUE, NEWGAME, CONTROLS, EXIT, BACK };


	//sounds
	bool isMainMusicPlayong; // native Functions dosrn't work at all
	CSoundPlayer  mainBgMusic;
	CSoundPlayer  loseSound;
	CSoundPlayer  winSound;
};