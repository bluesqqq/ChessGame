#include "event.h"
#include "queue"
#include <iostream>

Event::Event(string name, int lifetime) : name(name), lifetime(lifetime) {}

string Event::getName() {
	return name;
}

int Event::getLifetime() {
	return lifetime;
}

void Event::updateState(Board& board) {
	applyEvent(board);
	lifetime--;
}

GravityEvent::GravityEvent() : Event("Gravity", 1) {}

void GravityEvent::applyEvent(Board& board) {
    for (int col = 0; col < 8; col++) {
        queue<Piece*> pieces;

        // Collect all pieces in the current column
        for (int row = 0; row < 8; row++) {
            if (board.getTile(row, col)->hasPiece()) {
                pieces.push(board.getTile(row, col)->removePiece());
            }
        }

        // Place pieces back to the left of the column
        for (int row = 0; row < 8; row++) {
            if (!pieces.empty()) {
                board.getTile(row, col)->setPiece(pieces.front());
                pieces.pop();
            }
            else {
                board.getTile(row, col)->setPiece(nullptr);
            }
        }
    }
}

IceAgeEvent::IceAgeEvent() : Event("Ice Age", 6) {}

void IceAgeEvent::applyEvent(Board& board) {
    // put a bunch of ice tiles
}