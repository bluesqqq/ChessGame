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
		
		void updateEvent(Board& board);

		virtual void applyEvent(Board& board) = 0;
};

class GravityEvent : public Event {
	public:
		GravityEvent();
		void applyEvent(Board& board) override;
};