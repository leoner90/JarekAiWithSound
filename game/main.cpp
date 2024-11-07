#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	//app.DebugMemoryLeaks();
	app.OpenWindow(1366, 768, "Cheddar Heist!");
	//app.OpenFullScreen(1366, 768, 32);
	//app.OpenConsole();
	app.SetClearColor(CColor::Black());
	app.Run(&game);
	return(0);
}
