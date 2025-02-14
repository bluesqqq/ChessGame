#include "board.h"
#include <cmath>
#include <algorithm>

Board::Board(Texture2D* texture) : atlas(texture)
{
    // Populate with generic tiles
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            tiles[row][col] = new BasicTile(atlas);
        }
    }

    // Place Pawns
    for (int row = 0; row < 8; row++)
    {
        tiles[row][1]->setPiece(new Pawn(atlas, 1)); // Player 1 Pawns
        tiles[row][6]->setPiece(new Pawn(atlas, 2)); // Player 2 Pawns
    }

    // Place Rooks
    tiles[0][0]->setPiece(new Rook(atlas, 1));
    tiles[7][0]->setPiece(new Rook(atlas, 1));
    tiles[0][7]->setPiece(new Rook(atlas, 2));
    tiles[7][7]->setPiece(new Rook(atlas, 2));

    // Place Knights
    tiles[1][0]->setPiece(new Knight(atlas, 1));
    tiles[6][0]->setPiece(new Knight(atlas, 1));
    tiles[1][7]->setPiece(new Knight(atlas, 2));
    tiles[6][7]->setPiece(new Knight(atlas, 2));

    // Place Bishops
    tiles[2][0]->setPiece(new Bishop(atlas, 1));
    tiles[5][0]->setPiece(new Bishop(atlas, 1));
    tiles[2][7]->setPiece(new Bishop(atlas, 2));
    tiles[5][7]->setPiece(new Bishop(atlas, 2));

    // Place Queens
    tiles[3][0]->setPiece(new Queen(atlas, 1));  // Black Queen
    tiles[3][7]->setPiece(new Queen(atlas, 2));   // White Queen

    // Place Kings
    tiles[4][0]->setPiece(new King(atlas, 1));  // Black King
    tiles[4][7]->setPiece(new King(atlas, 2));   // White King

}

void Board::draw(int x, int y)
{
    bool hide = false;

    std::vector<std::pair<int, int>> highlightTiles;

    // If piece is selected, hide the other pieces
    if (x >= 0 && x < 8 && y >= 0 && y < 8 && tiles[x][y]->hasPiece())
    {
        hide = true;
        highlightTiles = tiles[x][y]->getPiece()->getValidMoves(x, y, *this);
        hide = true;
    }

    float time = GetTime(); // Get elapsed time

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            std::pair<int, int> current_pair = { row, col };
            auto it = std::find(highlightTiles.begin(), highlightTiles.end(), current_pair);

            Tile* tile = tiles[row][col];

            // Apply sine wave for a wavy effect
            float waveOffset = std::max(sin(time + (row + col) * 0.4f) * 0.1f, 0.0f);

            if (x == row && y == col)
            {
                tile->draw(row, col, waveOffset, true, false);
            }
            else if (it != highlightTiles.end())
            {
                tile->draw(row, col, waveOffset, true, false);
            }
            else
            {
                tile->draw(row, col, waveOffset, false, hide);
            }
        }
    }
}

void Board::SetTile(int row, int col, Tile* newTile)
{
    tiles[row][col] = newTile;
}

Tile* Board::GetTile(int row, int col)
{
    // If out of bounds, return a nullptr
    if (row >= 0 && row < 8 && col >= 0 && col < 8)
        return tiles[row][col];

    return nullptr;
}

bool Board::MovePiece(int pieceRow, int pieceCol, int destinationRow, int destinationCol)
{
    Tile* startTile = GetTile(pieceRow, pieceCol);
    Tile* endTile   = GetTile(destinationRow, destinationCol);

    // Check if both tiles are valid tiles, and the start tile has a piece to move
    if (startTile && endTile)
    {
        if (startTile->hasPiece())
        {
            if (startTile->getPiece()->isValidMove(pieceRow, pieceCol, *this, destinationRow, destinationCol))
            {

                if (endTile->hasPiece())
                {
                    endTile->removePiece();
                }

                Piece* targetPiece = startTile->removePiece();

                endTile->setPiece(targetPiece);

                return true;
            }
        }
    }

    return false;
}

int Board::GetPlayerTurn()
{
    return currentTurn % 2;
}
