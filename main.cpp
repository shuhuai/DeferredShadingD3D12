

#include "stdafx.h"


#include "directxApp.h"



int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	directxProcess* App = new directxProcess();
	return App->Run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
