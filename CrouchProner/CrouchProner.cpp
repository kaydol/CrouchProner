
#include "stdafx.h"
#include <thread>
#include <iostream>
#include <Windows.h>

//#define _WIN32_WINNT 0x050

#define TRIGGER 0x43 // 0x43 is 'C'
#define RESPONSE 0xa2 // 0xa2 is CTRL

unsigned int HOLD_TIME = 900; // how long to hold the TRIGGER before RESPONSE button press is simulated
unsigned int CHECKS = 15; // how many times during HOLD_TIME we check if the button is pressed
bool KEY_HELD = false;

using namespace std;

void threadFunction()
{
	int counter = 0;
	while (true) {
		if (KEY_HELD) ++counter; else counter = 0;
		if (counter == CHECKS) {
			//keybd_event('B', 0, 0, 0);
			//keybd_event(TRIGGER, (BYTE)MapVirtualKey(TRIGGER, 0), KEYEVENTF_KEYUP, 0);
			keybd_event(RESPONSE, (BYTE)MapVirtualKey(RESPONSE, 0), 0, 0);
			this_thread::sleep_for(std::chrono::milliseconds(15));
			keybd_event(RESPONSE, (BYTE)MapVirtualKey(RESPONSE, 0), KEYEVENTF_KEYUP, 0);
			cout << "Gone to prone" << endl;
			counter = 0;
		}
		this_thread::sleep_for(std::chrono::milliseconds(HOLD_TIME / CHECKS));
	}
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	BOOL fEatKeystroke = FALSE;

	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

			//cout << hex << (p->vkCode) << endl;

			if (fEatKeystroke = (p->vkCode == TRIGGER))
			{
				//cout << "TRIGGER PRESSED" << endl;

				if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) // Keydown
				{
					//keybd_event('B', 0, 0, 0);
					KEY_HELD = true;
				}
				else if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) // Keyup
				{
					//keybd_event('B', 0, KEYEVENTF_KEYUP, 0);
					KEY_HELD = false;
				}
				break;
			}
			break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
	// return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

int main(int argc, char* argv[])
{

	if (argc > 1) {
		HOLD_TIME = atoi(string(argv[1]).c_str());
	}
	if (argc > 2) {
		CHECKS = atoi(string(argv[2]).c_str());
	}

	cout << "====================================================================" << endl;
	cout << "This program will simulate pressing CTRL after holding C for " << HOLD_TIME << " ms" << endl;
	cout << "You can test if it works by holding C right now." << endl; 
	cout << "After you're done playing, simply close this window." << endl;
	cout << "====================================================================" << endl;

	thread thr(threadFunction);
	thr.detach();

	// Install the low-level keyboard & mouse hooks
	HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

	// Keep this app running until we're told to stop
	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hhkLowLevelKybd);

	return(0);
}
