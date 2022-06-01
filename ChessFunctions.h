#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lib/stb_image.h"
#include "ChessLog.h"

using namespace std;

/*
The MIT License (MIT)

Copyright (c) 2014-2022 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Simple helper function to load an image into a OpenGL texture with common settings
// Code taken from source: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
bool LoadTextureFromFile(const char* filename, GLuint* out_texture,
	int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width,
		&image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}



struct Texture
{
	string piece = "";
	int my_image_width = 0;
	int my_image_height = 0;
	bool clicked = false;
	GLuint my_image_texture = 0;
};

struct tile
{
	tile* next, * prev;
	int x, y, numOfTimesMoved = 0;
	string piece = "";
	bool isOwnPiece = false;
	bool isWhite = false;
	bool is_valid_tile = false;
	bool highlighted;
	Texture tex;

	void override(tile* otherTile, bool count_move = true) {
		piece = otherTile->piece;
		numOfTimesMoved = otherTile->numOfTimesMoved + (count_move == 1);
		isOwnPiece = otherTile->isOwnPiece;
		isWhite = otherTile->isWhite;
		tex = otherTile->tex;
	}

	void clear()
	{
		piece = "";
		numOfTimesMoved = 0;
		isOwnPiece = false;
		isWhite = false;
		tex = Texture();
	}

	void rotate() {
		x = 7 - x;
		y = 7 - y;
		if (piece.length() > 0) isOwnPiece = !isOwnPiece;
		isWhite = !isWhite;
	}

	tile() {};
	tile(int x, const char* piece)
	{
		this->x = x;
		this->tex.piece = piece;
	}

	//Operators
	bool operator == (const tile& other)
	{
		return this->x == other.x;
	}

	tile& operator =(const tile& other)
	{
		this->piece = other.piece;
		this->numOfTimesMoved = other.numOfTimesMoved;
		this->isOwnPiece = other.isOwnPiece;
		this->isWhite = other.isWhite;
		this->is_valid_tile = other.is_valid_tile;
		this->highlighted = other.highlighted;
		this->tex = other.tex;
		this->x = other.x;
		this->y = other.y;
		this->prev = other.prev;
		this->next = other.next;
		return *this;
	}
};

tile* head = nullptr;
tile* tail = nullptr;

void regularMove(tile* pieceTile, tile* targetTile, bool count_move = true) {
	logMove(pieceTile->x, targetTile->x, targetTile->y, 
		pieceTile->piece, (targetTile != nullptr && !targetTile->isOwnPiece));
	targetTile->override(pieceTile, count_move);
	pieceTile->clear();
}

tile* left(tile* p) {
	if (p == nullptr) return nullptr;
	else if (p->x == 0) return nullptr;
	else return p->prev;
}

tile* right(tile* p) {
	if (p == nullptr) return nullptr;
	else if (p->x == 7) return nullptr;
	else return p->next;
}

tile* prev(tile* p, int n) {
	if (n == 0 || p == nullptr) return p;
	else return prev(p->prev, n - 1);
}

tile* up(tile* p) { return prev(p, 8); }

tile* next(tile* p, int n) {
	if (n == 0 || p == nullptr) return p;
	else return next(p->next, n - 1);
}
tile* down(tile* p) { return next(p, 8); }

//now can do for instance
tile* leftDiag(tile* p) { return left(up(p)); }
tile* search_pos(int i)
{
	tile* next = head;

	while (next != nullptr && i > 0)
	{
		i--;
		next = next->next;
	}

	return next;
}

void rotate_list()
{
	if (head == nullptr || tail == nullptr)
		return;

	tile* curr_head = head;
	tile* next_head = head->next;

	tile* tmp_tail = tail;
	tail = head;
	head = tmp_tail;

	while (curr_head != nullptr)
	{
		
		curr_head->rotate();

		tile* tmp = curr_head->next;
		curr_head->next = curr_head->prev;
		curr_head->prev = tmp;

		curr_head = next_head;

		if (next_head != nullptr)
			next_head = next_head->next;
	}
}

tile* createTile(int xPos) {
	tile* temp = new tile();
	temp->x = xPos % 8;
	temp->y = xPos / 8;
	temp->next = nullptr;
	temp->prev = nullptr;
	return temp;
}

tile* addTile(int xPos)
{
	tile* node = createTile(xPos);

	if (tail == nullptr)
	{
		head = node;
		tail = node;
	}
	else
	{
		tail->next = node;
		node->prev = tail;
		tail = node;
	}

	return node;
}

// Debugging function, prtints all tiles in the list
void printTiles()
{
	tile* p = head;
	for (; p; p = p->next)
	{
		//cout << "x: " << p->x << "\t";
		if (p->piece == "Rook") cout << "R";
	}
}

void printTile(tile* p)
{
	//cout << p->x;
	if (p->piece == "") cout << "E";
	else if (p->piece == "Rook") cout << "R";
	else if (p->piece == "Bishop") cout << "B";
	else if (p->piece == "Knight") cout << "H";
	else if (p->piece == "Queen") cout << "Q";
	else if (p->piece == "King") cout << "H";
	else if (p->piece == "Pawn") cout << "P";

}

tile* findTile(int index)
{
	tile* p = head;

	//if (index < BOARD_SIZE / 2)
	//{
	p = head;
	for (int i = 0; i != index; i++, p = p->next)
	{
		// Room for logic
	}
	return p;
	//}
	/*
	* else
	{
		p = tail;
		for (int i = 63; i != index + 1; i--, p = p->prev)
		{
			// Room for logic
		}
		return p;
	}
	*/
}

//moves piece to targetTile
void move(tile* pieceTile, tile* targetTile)
{
	targetTile->piece = pieceTile->piece;
	targetTile->isOwnPiece = true;
	pieceTile->piece = "";
	pieceTile->isOwnPiece = false;
}

vector<tile*> bishopValidMoves(tile* bishopPos) {
	vector<tile*> validMoves = {};
	tile* temp;
	temp = bishopPos;
	//left up diag:
	while (up(left(temp))) {
		temp = up(left(temp));
		//check if own piece
		if (temp->isOwnPiece) break;
		else validMoves.push_back(temp);
		//check if enemy piece
		if ((temp->piece).length() > 0) break;
	}
	temp = bishopPos;
	//right up diag;
	while (up(right(temp))) {
		temp = up(right(temp));
		//check if own piece
		if (temp->isOwnPiece) break;
		else validMoves.push_back(temp);
		//check if enemy piece
		if ((temp->piece).length() > 0) break;
	}
	temp = bishopPos;
	while (down(left(temp))) {
		temp = down(left(temp));
		//check if own piece
		if (temp->isOwnPiece) break;
		else validMoves.push_back(temp);
		//check if enemy piece
		if ((temp->piece).length() > 0) break;
	}
	temp = bishopPos;
	while (down(right(temp))) {
		temp = down(right(temp));
		//check if own piece
		if (temp->isOwnPiece) break;
		else validMoves.push_back(temp);
		//check if enemy piece
		if ((temp->piece).length() > 0) break;
	}
	return validMoves;
}

vector<tile*> rookValidMoves(tile* p)
{
	vector<tile*> rooks = {};
	tile* pp = p;

	//Checking how many spaces we can move right
	while (right(pp) != nullptr)
	{
		pp = pp->next;
		if (pp->piece != "")
		{
			if (pp->isOwnPiece)
				break;
			else
			{
				rooks.push_back(pp);
				break;
			}
		}
		else
		{
			rooks.push_back(pp);
		}
	}

	//Checking how many spaces we can move left
	pp = p;
	while (left(pp) != nullptr)
	{
		pp = pp->prev;
		if (pp->piece != "")
		{
			if (pp->isOwnPiece)
				break;
			else
			{
				rooks.push_back(pp);
				break;
			}
		}
		else
		{
			rooks.push_back(pp);
		}
	}

	//Checking how many spaces we can move up
	pp = p;
	while (up(pp) != nullptr)
	{
		pp = up(pp);
		if (pp->piece != "")
		{
			if (pp->isOwnPiece)
				break;
			else
			{
				rooks.push_back(pp);
				break;
			}
		}
		else
		{
			rooks.push_back(pp);
		}
	}

	//Checking how many spaces we can move down
	pp = p;
	while (down(pp) != nullptr)
	{
		pp = down(pp);
		if (pp->piece != "")
		{
			if (pp->isOwnPiece)
				break;
			else
			{
				rooks.push_back(pp);
				break;
			}
		}
		else
		{
			rooks.push_back(pp);
		}
	}

	return rooks;
}

vector<tile*> knightValidMoves(tile* knightPos) {
	vector<tile*> validMoves = {};
	tile* temp;
	temp = left(up(up(knightPos)));
	if (temp && !temp->isOwnPiece) validMoves.push_back(temp);
	temp = right(up(up(knightPos)));
	if (temp && !temp->isOwnPiece) validMoves.push_back(temp);

	temp = up(right(right(knightPos)));
	if (temp && !temp->isOwnPiece) validMoves.push_back(temp);
	temp = down(right(right(knightPos)));
	if (temp && !temp->isOwnPiece) validMoves.push_back(temp);

	temp = right(down(down(knightPos)));
	if (temp && !temp->isOwnPiece) validMoves.push_back(temp);
	temp = left(down(down(knightPos)));
	if (temp && !temp->isOwnPiece) validMoves.push_back(temp);

	temp = up(left(left(knightPos)));
	if (temp && !temp->isOwnPiece) validMoves.push_back(temp);
	temp = down(left(left(knightPos)));
	if (temp && !temp->isOwnPiece) validMoves.push_back(temp);

	return validMoves;
}

vector<tile*> pawnValidMoves(tile* pawnPos) {
	bool enPassantable;
	vector<tile*> validMoves = {};
	tile* temp = pawnPos;
	if (up(temp) != nullptr && up(temp)->piece.length() == 0) validMoves.push_back(up(temp));
	//double move
	temp = up(pawnPos);
	if (up(temp) != nullptr && up(temp)->piece.length() == 0 
		&& pawnPos->numOfTimesMoved == 0) validMoves.push_back(up(temp));
	//capture regular
	temp = up(left(pawnPos));
	if (temp && temp->piece.length() > 0 && !temp->isOwnPiece) validMoves.push_back(temp);
	temp = up(right(pawnPos));
	if (temp && temp->piece.length() > 0 && !temp->isOwnPiece) validMoves.push_back(temp);
	//en passant! en passant! en passant!
	temp = left(pawnPos);
	enPassantable = (temp && !temp->isOwnPiece && temp->piece == "Pawn" && temp->numOfTimesMoved == 1 && temp->y == 3);
	if (enPassantable && up(temp) && up(temp)->piece == "") validMoves.push_back(up(temp));
	temp = right(pawnPos);
	enPassantable = (temp && !temp->isOwnPiece && temp->piece == "Pawn" && temp->numOfTimesMoved == 1 && temp->y == 3);
	if (enPassantable && up(temp) && up(temp)->piece == "") validMoves.push_back(up(temp));
	return validMoves;
}

vector<tile*> queenValidMoves(tile* p)
{
	vector<tile*> queens = {};
	vector<tile*> vector1 = rookValidMoves(p);
	vector<tile*> vector2 = bishopValidMoves(p);

	for (tile* i : vector1)
	{
		queens.push_back(i);
	}

	for (tile* i : vector2)
	{
		queens.push_back(i);
	}

	return queens;
}

vector<tile*> kingValidMoves(tile* p)
{
	vector<tile*> kings = {};
	tile* pp;
	//castling
	if (p->numOfTimesMoved == 0) {
		//look left for rook
		pp = left(p);
		while (pp != nullptr) {
			if (pp->isOwnPiece && pp->piece == "Rook" && pp->numOfTimesMoved == 0)
			{
				kings.push_back(left(left(p)));
				break;
			}
			else if (pp->piece != "") break;
			else pp = left(pp);
		}
		pp = right(p);
		while (pp != nullptr) {
			if (pp->isOwnPiece && pp->piece == "Rook" && pp->numOfTimesMoved == 0) 
			{
				kings.push_back(right(right(p)));
				break;
			}
			else if (pp->piece != "") break;
			else pp = right(pp);
		}
	}
	//check all the squares to the right
	pp = p->next;
	if (pp != NULL && pp->x != 1)
	{
		if (pp->piece != "" && !pp->isOwnPiece)
		{
			kings.push_back(pp);
		}
		else if (pp->piece == "")
		{
			kings.push_back(pp);
		}

		pp = up(right(p));
		if (pp != NULL)
		{
			if (pp->piece != "" && !pp->isOwnPiece)
			{
				kings.push_back(pp);
			}
			else if (pp->piece == "")
			{
				kings.push_back(pp);
			}
		}

		pp = down(right(p));
		if (pp != NULL)
		{
			if (pp->piece != "" && !pp->isOwnPiece)
			{
				kings.push_back(pp);
			}
			else if (pp->piece == "")
			{
				kings.push_back(pp);
			}
		}

	}

	//check all the squares to the left
	pp = p->prev;
	if (pp != NULL && pp->x != 1)
	{
		if (pp->piece != "" && !pp->isOwnPiece)
		{
			kings.push_back(pp);
		}
		else if (pp->piece == "")
		{
			kings.push_back(pp);
		}

		pp = up(left(p));
		if (pp != NULL)
		{
			if (pp->piece != "" && !pp->isOwnPiece)
			{
				kings.push_back(pp);
			}
			else if (pp->piece == "")
			{
				kings.push_back(pp);
			}
		}

		pp = down(left(p));
		if (pp != NULL)
		{
			if (pp->piece != "" && !pp->isOwnPiece)
			{
				kings.push_back(pp);
			}
			else if (pp->piece == "")
			{
				kings.push_back(pp);
			}
		}

	}


	pp = up(p);
	if (pp != NULL)
	{
		if (pp->piece != "" && !pp->isOwnPiece)
		{
			kings.push_back(pp);
		}
		else if (pp->piece == "")
		{
			kings.push_back(pp);
		}
	}

	pp = down(p);
	if (pp != NULL)
	{
		if (pp->piece != "" && !pp->isOwnPiece)
		{
			kings.push_back(pp);
		}
		else if (pp->piece == "")
		{
			kings.push_back(pp);
		}
	}

	return kings;

}

tile* getOwnPiece(const char * p)
{
	tile* next = head;
	string piece = p;

	while (next != nullptr)
	{
		if (next->piece == piece && next->isOwnPiece)
			break;

		next = next->next;
	}

	return next;
}

vector<tile*> callCorrectFunction(string s, tile* p)
{
	if (s == "pawn" || s == "Pawn")
		return pawnValidMoves(p);
	else if (s == "bishop" || s == "Bishop")
		return bishopValidMoves(p);
	else if (s == "rook" || s == "Rook")
		return rookValidMoves(p);
	else if (s == "knight" || s == "Knight")
		return knightValidMoves(p);
	else if (s == "queen" || s == "Queen")
		return queenValidMoves(p);
	else if (s == "king" || s == "King")
		return kingValidMoves(p);
	else
		return vector<tile*> ();
}


string arr[] = { "Pawn", "Bishop", "Rook", "Knight", "Queen" };

bool isKingInCheck(tile* kingLocation)
{
	vector<tile*> vec;

	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		vec = callCorrectFunction(arr[i], kingLocation);

		for (auto& item : vec)
		{
			if (item->piece == arr[i] && !item->isOwnPiece)
				return true;
		}
	}

	return false;
}

int isKingInCheckmate(tile* kingLocation)
{
	//returns 0 if king is not in checkmate, 1 if it is in checkmate, and 2 if it is a stalemate
	bool staleMate = false;
	if (isKingInCheck(kingLocation))
		staleMate = true;
	tile* p = head;
	string s;
	vector<tile*> check;
	while (p != NULL)
	{
		if (p->piece != "" && p->isOwnPiece)
		{
			check = callCorrectFunction(p->piece, p);
			for (auto& i : check)
			{
				s = i->piece;
				regularMove(p, i);
				if (!isKingInCheck)
				{
					regularMove(i, p);
					i->piece = s;
					return 0;
				}
				regularMove(i, p);
				i->piece = s;
			}
		}

		p = p->next;
	}
	if (staleMate)
		return 2;
	else
		return 1;
}

bool isKingInCheckNextMove(tile*source, tile* dest, bool & change_player)
{
	regularMove(source, dest, false);

	tile* next = getOwnPiece("King");
	bool is_in_check = isKingInCheck(next);

	if (is_in_check)
		regularMove(dest, source, false);
	else
	{
		dest->numOfTimesMoved++;
		change_player = true;
	}

	return is_in_check;
}