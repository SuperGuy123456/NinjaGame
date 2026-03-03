#include "InputManager.h"

InputManager::InputManager(EventManager& _eventmanager) : eventmanager(_eventmanager)
{
	eventmanager = _eventmanager;
}
InputManager::~InputManager() {};

void InputManager::GetInput()
{
	// HOLD
	for (int i = 0; i < NUMBEROFHOLDKEYS; i++)
	{
		if (IsKeyDown(holdkeys[i]))
		{
			eventmanager.BroadcastMessage("HOLD_" + holdkeysreadable[i]);
		}
	}

	// PRESS
	for (int i = 0; i < NUMBEROFPRESSKEYS; i++)
	{
		if (IsKeyPressed(presskeys[i]))
		{
			eventmanager.BroadcastMessage("PRESS_" + presskeysreadable[i]);
		}
	}

	// RELEASE
	for (int i = 0; i < NUMBEROFRELEASEKEYS; i++)
	{
		if (IsKeyReleased(releasekeys[i]))
		{
			eventmanager.BroadcastMessage("RELEASE_" + releasekeysreadable[i]);
		}
	}
}

