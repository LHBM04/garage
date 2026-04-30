#include "Precompiled.h"
#include "Application.h"

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	Application::Options options;
	Application app(options);

	return app.Run();
}
