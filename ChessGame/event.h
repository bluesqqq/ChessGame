#pragma once

#include <string>

#include "board.h"

using namespace std;

class Event {
	private:
		string name;
		int lifetime;

	public:
		Event(string name, int lifetime);
		string getName();
		int getLifetime();
		
		void updateState(Board& board);

		virtual void applyEvent(Board& board) = 0;
};

class GravityEvent : public Event {
	public:
		GravityEvent();
		void applyEvent(Board& board) override;
};

class IceAgeEvent : public Event {
	public:
		IceAgeEvent();
		void applyEvent(Board& board) override;
};