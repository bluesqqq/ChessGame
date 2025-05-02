#include "Cell.h"

Cell::Cell() : rank(0), file(0) {}

Cell::Cell(int rank, int file) : rank(rank), file(file) {}

string Cell::getAlgebraicNotation() const {

    if (!isInBounds()) return "Invalid";
    char f = ('a' + file);
    return f + to_string(rank + 1);
}

bool Cell::operator==(const Cell& other) const {
    return (rank == other.rank && file == other.file);
}

bool Cell::operator!=(const Cell& other) const {
    return (rank != other.rank || file != other.file);
}

Cell Cell::operator+(const Cell& other) const {
    return { rank + other.rank, file + other.file };
}

Cell& Cell::operator+=(const Cell& other) {
    rank += other.rank;
    file += other.file;
    return *this;
}