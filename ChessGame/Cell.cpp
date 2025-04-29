#include "Cell.h"

Cell::Cell(int rank, int file) : rank(rank), file(file) {}

string Cell::getAlgebraicNotation() {

    if (rank < 1 || rank > 8 || file < 1 || file > 8) {
        return "Invalid";
    }
    char f = ('a' + (file - 1));
    return f + to_string(rank);
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