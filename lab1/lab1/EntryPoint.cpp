#include "App_ns.h">

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	CApp app{};
	return app.Run();
}