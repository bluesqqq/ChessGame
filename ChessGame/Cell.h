#pragma once

#include <string>

using namespace std;

struct Cell {
    int rank;
    int file;
    Cell();

    Cell(int rank, int file);

    string getAlgebraicNotation();

    bool isInBounds() {
        return (rank >= 0 && rank <= 7 && file >= 0 && file <= 7);
    }

    bool operator==(const Cell& other) const;

    bool operator!=(const Cell& other) const;

    Cell operator+(const Cell& other) const;

    Cell& operator+=(const Cell& other);
};