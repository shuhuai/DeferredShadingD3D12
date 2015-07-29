#include "stdafx.h"
#include "windowsApp.h"
#include <Windowsx.h>
windowsApp* appPointer;

 LRESULT CALLBACK windowsApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		appPointer->Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		appPointer->KeyDown(wParam);
	case WM_KEYUP:
		//appPointer->KeyUp(wParam);
	case WM_LBUTTONDOWN:
		appPointer->MousePress(LOWORD(lParam), HIWORD(lParam));
	case WM_LBUTTONUP:
		appPointer->MouseRelease(wParam);
	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON)
		{
			appPointer->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


windowsApp::windowsApp()
{
mWidth=800;
mHeight=600;
}

int windowsApp::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR    lpCmdLine, int     nCmdShow)
{
	appPointer = this;
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MSG msg;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	ZeroMemory(&msg, sizeof(msg));

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &windowsApp::WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "Deferred Shading";
	RegisterClassEx(&wc);

	RECT rect = { 0, 0, mWidth, mHeight };
	DWORD dwStyle = WS_OVERLAPPEDWINDOW &  ~WS_MAXIMIZEBOX;
	AdjustWindowRect(&rect, dwStyle, FALSE);
	mHwnd = CreateWindow(wc.lpszClassName,
		wc.lpszClassName,
		dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, hInstance, NULL);

	ShowWindow(mHwnd, nCmdShow);
	UpdateWindow(mHwnd);
	Setup();
	
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}
			else {
				Update();
				Render();

			}
		}
	


	return (int)msg.wParam;
}

