#pragma once
#include <vector>
#include "ChessFunctions.h"
#define BOARD_SIZE 64
#define ROW 8
using namespace std;

tile* king_white = nullptr;
tile* king_black = nullptr;

tile* makeBoard()
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		addTile(i);
		//addTile(i);
	}
	return head;
}
void printBoard()
{
	int count = 1;
	tile* p = head;
	for (int i = 0; i < BOARD_SIZE; i++, count++)
	{
		printTile(p);
		p = p->next;
		if ((count % 8) == 0)
		{
			cout << endl;
		}
	}
}
tile* placePiece(int pos, string pieceName, bool isEnemy)
{
	tile* p = findTile(pos);

	p->piece = pieceName;
	p->isOwnPiece = !isEnemy;
	p->highlighted = false;
	p->isWhite = !isEnemy;
	p->tex.piece = pieceName + "_" + (p->isWhite == true ? "white" : "black");
	p->tex.piece[0] = std::tolower(pieceName[0]);
	//cout << "tile found: " << p->x << endl;
	return p;
}
void populateBoard()
{
	placePiece(0, "Rook", true);
	placePiece(1, "Knight", true);
	placePiece(2, "Bishop", true);
	placePiece(3, "King", true);
	placePiece(4, "Queen", true);
	placePiece(5, "Bishop", true);
	placePiece(6, "Knight", true);
	placePiece(7, "Rook", true);
	placePiece(8, "Pawn", true);
	placePiece(9, "Pawn", true);
	placePiece(10, "Pawn", true);
	placePiece(11, "Pawn", true);
	placePiece(12, "Pawn", true);
	placePiece(13, "Pawn", true);
	placePiece(14, "Pawn", true);
	placePiece(15, "Pawn", true);
	placePiece(63, "Rook", false);
	placePiece(62, "Knight", false);
	placePiece(61, "Bishop", false);
	placePiece(60, "Queen", false);
	placePiece(59, "King", false);
	placePiece(58, "Bishop", false);
	placePiece(57, "Knight", false);
	placePiece(56, "Rook", false);
	placePiece(55, "Pawn", false);
	placePiece(54, "Pawn", false);
	placePiece(53, "Pawn", false);
	placePiece(52, "Pawn", false);
	placePiece(51, "Pawn", false);
	placePiece(50, "Pawn", false);
	placePiece(49, "Pawn", false);
	placePiece(48, "Pawn", false);
}