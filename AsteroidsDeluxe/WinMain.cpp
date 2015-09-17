#pragma once
#include "Asteroids.h"
#include "windows.h"
#include "resource.h"

#define IDI_MYICON 201

using namespace Polycode;

#if defined(_DEBUG)
Asteroids * pAsteroids;
#else
std::unique_ptr<Asteroids> pAsteroids;
#endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	PolycodeView *view = new PolycodeView(hInstance, nCmdShow, L"Asteroids Deluxe Beta 1.00");

#if defined(_DEBUG)
	pAsteroids = new Asteroids(view);
#else
	pAsteroids = std::unique_ptr<Asteroids>( new Asteroids(view) );
#endif
	
	HICON hMyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));
	SendMessage(view->hwnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hMyIcon);

	MSG Msg;

	do
	{
		while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	} 
	while (pAsteroids->Update());

	return Msg.wParam;
}