#include "InputManager.h"

InputManager::InputManager(EventManager& _eventmanager) : eventmanager(_eventmanager)
{
	eventmanager = _eventmanager;
}
InputManager::~InputManager() {};

void InputManager::GetInput()
{
	// -------------------------
	// KEY HOLD
	// -------------------------
	for (int i = 0; i < NUMBEROFHOLDKEYS; i++)
	{
		if (IsKeyDown(holdkeys[i]))
		{
			eventmanager.BroadcastMessage("HOLD_" + holdkeysreadable[i]);
		}
	}

	// -------------------------
	// KEY PRESS
	// -------------------------
	for (int i = 0; i < NUMBEROFPRESSKEYS; i++)
	{
		if (IsKeyPressed(presskeys[i]))
		{
			eventmanager.BroadcastMessage("PRESS_" + presskeysreadable[i]);
		}
	}

	// -------------------------
	// KEY RELEASE
	// -------------------------
	for (int i = 0; i < NUMBEROFRELEASEKEYS; i++)
	{
		if (IsKeyReleased(releasekeys[i]))
		{
			eventmanager.BroadcastMessage("RELEASE_" + releasekeysreadable[i]);
		}
	}

	// -------------------------
	// MOUSE PRESS
	// -------------------------
	for (int i = 0; i < NUMBEROFMOUSEPRESSKEYS; i++)
	{
		if (IsMouseButtonPressed(mousepresskeys[i]))
		{
			eventmanager.BroadcastMessage("PRESS_" + mousepresskeysreadable[i]);

		}
	}

	// -------------------------
	// MOUSE RELEASE
	// -------------------------
	for (int i = 0; i < NUMBEROFMOUSEPRESSKEYS; i++)
	{
		if (IsMouseButtonReleased(mousereleasekeys[i]))
		{
			eventmanager.BroadcastMessage("RELEASE_" + mousereleasekeysreadable[i]);

		}
	}
}
