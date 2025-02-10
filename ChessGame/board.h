#include "raylib.h"
#include "piece.h"
#include "tile.h"
#include <cmath>

using namespace std;

class Board
{
private:
    Tile* tiles[8][8];
    Texture2D* atlas;

public:
    Board(Texture2D* texture) : atlas(texture)
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
        for (int col = 0; col < 8; col++)
        {
            tiles[1][col]->setPiece(new Pawn(atlas, 1)); // Player 1 Pawns
            tiles[6][col]->setPiece(new Pawn(atlas, 2)); // Player 2 Pawns
        }

        // Place Rooks
        tiles[0][0]->setPiece(new Rook(atlas, 1));
        tiles[0][7]->setPiece(new Rook(atlas, 1));
        tiles[7][0]->setPiece(new Rook(atlas, 2));
        tiles[7][7]->setPiece(new Rook(atlas, 2));

        // Place Knights
        tiles[0][1]->setPiece(new Knight(atlas, 1));
        tiles[0][6]->setPiece(new Knight(atlas, 1));
        tiles[7][1]->setPiece(new Knight(atlas, 2));
        tiles[7][6]->setPiece(new Knight(atlas, 2));

        // Place Bishops
        tiles[0][2]->setPiece(new Bishop(atlas, 1));
        tiles[0][5]->setPiece(new Bishop(atlas, 1));
        tiles[7][2]->setPiece(new Bishop(atlas, 2));
        tiles[7][5]->setPiece(new Bishop(atlas, 2));

        // Place Queens
        tiles[0][3]->setPiece(new Queen(atlas, 1));  // Black Queen
        tiles[7][3]->setPiece(new Queen(atlas, 2));   // White Queen

        // Place Kings
        tiles[0][4]->setPiece(new King(atlas, 1));  // Black King
        tiles[7][4]->setPiece(new King(atlas, 2));   // White King
    }

    void draw()
    {
        float time = GetTime(); // Get elapsed time

        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                Tile* tile = tiles[row][col];

                // Apply sine wave for a wavy effect
                float waveOffset = max(sin(time + (row + col) * 0.5f) * 0.2f, 0.0f);

                tile->draw(row, col, waveOffset);
            }
        }
    }

    void SetTile(int row, int col, Tile* newTile)
    {
        tiles[row][col] = newTile;
    }

    Tile* GetTile(int row, int col)
    {
        return tiles[row][col];
    }
};