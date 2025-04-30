#pragma once

#include <vector>
#include "textures.h"
#include "include/raylib-cpp.hpp"
#include <iostream>
#include "Cell.h"
#include "board.h"
#include "piece.h"

using namespace std;

class PromotionMenu {
	private:
        Cell promotionCell;
        bool open = true;
        float totalWidth = 0;

	public:
		PromotionMenu(Cell promotionCell) : promotionCell(promotionCell) {
            
        };

        void draw(raylib::Texture2D* atlas, Board& board) {

            if (open && totalWidth < 1) {
                totalWidth += 0.02f;
            } else if (!open && totalWidth > 0) {
				totalWidth -= 0.02f;
			}

			Vector2 pos = board.cellToScreenPosition(promotionCell);

            float x = pos.x;
            float y = pos.y;

            DrawRectangle(x, y, TILE_WIDTH, TILE_WIDTH + (TILE_WIDTH * 3 * totalWidth), WHITE);
            DrawRectangleLines(x, y, TILE_WIDTH, TILE_WIDTH + (TILE_WIDTH * 3 * totalWidth), BLACK);

            Vector2 mousePos = GetMousePosition();

            Tile* promotionTile = board.getTile(promotionCell);

            Piece* promotionPiece = promotionTile->getPiece();

            for (int i = 0; i < 4; i++) {
                float yOffset = float(i * TILE_WIDTH) * totalWidth;
                float boxX = x + 4;
                float boxY = y + 4 + yOffset;
                float boxW = TILE_WIDTH - 8;
                float boxH = TILE_WIDTH - 8;

                Rectangle iconBox = { boxX, boxY, boxW, boxH };
                bool hovering = CheckCollisionPointRec(mousePos, iconBox);

                Color bgColor = hovering ? LIGHTGRAY : WHITE;

                // Background box
                DrawRectangleRec(iconBox, bgColor);
                DrawRectangleLinesEx(iconBox, 1, BLACK);

                // Icon
                SpriteBounds bounds = iconSprites[i];
                Rectangle source = bounds.toSpriteRect();
                DrawTextureRec(*atlas, source, { x, y + yOffset }, WHITE);

                if (hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // Replace the piece with the selected one
                    Piece* newPiece = nullptr;
					int player = promotionPiece->getPlayer();

                    switch (i) {
                        case ICON_KNIGHT: newPiece = new Knight(atlas, player); break;
                        case ICON_ROOK:   newPiece = new Rook(atlas, player); break;
                        case ICON_BISHOP: newPiece = new Bishop(atlas, player); break;
                        case ICON_QUEEN:  newPiece = new Queen(atlas, player); break;
                        default: break;
                    }

                    if (newPiece) {
                        delete promotionTile->getPiece();  // Clean up old piece
                        promotionTile->setPiece(newPiece);
                        open = false;  // Close menu after promotion
                    }
                }

            }
        }

        bool finished() {
			return !open && totalWidth <= 0;
        }
};

