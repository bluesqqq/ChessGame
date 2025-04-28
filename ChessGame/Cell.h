#pragma once

#include <string>

using namespace std;

struct Cell {
    int rank;
    int file;
    Cell(int rank, int file);

    string getAlgebraicNotation();

    bool isInBounds() {
        return (rank >= 1 && rank <= 8 && file >= 1 && file <= 8);
    }

    bool operator==(const Cell& other) const;

    Cell operator+(const Cell& other) const;

    Cell& operator+=(const Cell& other);
};