#define _CRT_SECURE_NO_WARNINGS //for freopen
#include <conio.h> //for _getch
#include <Windows.h>
#include <iostream>

#define STR_TITLE "program title"
#define STR_INTRO "a welcome text displayed on console screen"
#define STR_INFO  "an info text saying the window is found, displayed on console screen"
#define STR_INFO2 "another info text saying the window is found, displayed on console screen (not necessary)"

static RECT g_Rect; //Rectangle of working area of selected screen
static bool b_Found = false; //Indicator for whether the requested screen is found
BOOL __stdcall iterator(HMONITOR h, HDC, LPRECT, LPARAM) //Callback function for EnumDisplayMonitors
{
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(h, &mi);
	if (mi.dwFlags != MONITORINFOF_PRIMARY) //IMPORTANT: Change condition to '==' for primary screen
	{
		g_Rect = mi.rcWork;
		b_Found = true;
		printf_s("Non-primary monitor found, w=%d, h=%d\n", mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top);
	}
	return true; //Exit at the first item
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	AllocConsole(); //Create a console window
	SetConsoleTitleA(STR_TITLE);
	freopen("CONOUT$", "w+", stdout); //Reopen the output buffer of the console we got from AllocConsole as writable
	printf_s(STR_INTRO); //Debug text
	EnumDisplayMonitors(0, 0, iterator, 0); //Enumerate all monitors connected to the computer...
	while (!b_Found) Sleep(1); //...and wait until the screen we wanted is found

	HWND nt = FindWindowA("Chrome_WidgetWin_1", "Discord Notifications"); //Get the handle of notification panel window (I see that Discord uses CEF, according to window class name)
	printf_s(STR_INFO); //Debug text
	printf_s(STR_INFO2); //Debug text (not necessary)
	_getch(); //Dirty way of waiting for a key press
	FreeConsole(); //Close the console window
	while (true)
	{
		Sleep(1); //Another dirty way of avoid overloading cpu with an endless loop
		if (nt) { //If the panel found/available, ...
			RECT r = { 0, 0, 0, 0 };
			GetWindowRect(nt, &r); //...get it's dimensions...
			if (r.bottom == 0 && r.left == 0 && r.right == 0 && r.top == 0) continue; //...and check if it's a valid/visible window...
			SetWindowPos(nt, 0, g_Rect.right - (r.right - r.left), g_Rect.bottom - (r.bottom - r.top), 0, 0, SWP_NOSIZE); //...then move it to the right-bottom of the screen as the original one would be
		}
		nt = FindWindowA("Chrome_WidgetWin_1", "Discord Notifications"); //Update the window handle if there's something changed
																		 //(for example when there is no notification, the panel window gets destroyed (nt = NULL), otherwise it's created again)
	}
	return S_OK;
}