#pragma once
#include "raylib.h"
#include "EventManager.h"
#include <string>
#include <iostream>

class InputManager //Basically enhanced input actions but c++ style and just uses event mnaager to broadcast button presses
{
public:

	// Counts
	static const int NUMBEROFHOLDKEYS = 6;
	static const int NUMBEROFPRESSKEYS = 7;
	static const int NUMBEROFRELEASEKEYS = 6;

	// HOLD keys
	KeyboardKey holdkeys[NUMBEROFHOLDKEYS] = {
		KEY_W, KEY_A, KEY_S, KEY_D, KEY_SPACE, KEY_LEFT_SHIFT
	};

	string holdkeysreadable[NUMBEROFHOLDKEYS] = {
		"W", "A", "S", "D", "Space", "SHIFT"
	};

	// PRESS keys
	KeyboardKey presskeys[NUMBEROFPRESSKEYS] = {
		KEY_E, KEY_Q, KEY_O, KEY_ESCAPE, KEY_SEMICOLON, KEY_LEFT_BRACKET, KEY_TAB
	};

	string presskeysreadable[NUMBEROFPRESSKEYS] = {
		"E", "Q", "O", "Escape", ";", "[", "TAB"
	};

	// RELEASE keys (same as HOLD keys)
	KeyboardKey releasekeys[NUMBEROFRELEASEKEYS] = {
		KEY_W, KEY_A, KEY_S, KEY_D, KEY_SPACE, KEY_LEFT_SHIFT
	};

	string releasekeysreadable[NUMBEROFRELEASEKEYS] = {
		"W", "A", "S", "D", "Space", "SHIFT"
	};

	InputManager(EventManager& _eventmanager);
	~InputManager();

	void GetInput(); //Call every frame to check for input and broadcast events

private:
	EventManager& eventmanager;
};

