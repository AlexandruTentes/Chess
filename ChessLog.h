#include <vector>
#include <string>
#include <fstream>

using namespace std;

int turnNumber = 0;
bool isNewTurn = false;

void createLog()
{
	ofstream outfile("gameLog.txt");
	outfile << "Game Log for Flip死Chess　version 0.1:" << endl;
	outfile << "Information: Chess notation, piece names are as one expects" << endl;
	outfile << "except Knight is \'N\' and Pawn doesn't have one lol" << endl;
	outfile << "It goes (Piece Name)(End Tile) when not taking anything," << endl;
	outfile << "It goes (Piece Name)'x'(End Tile) when taking and for" << endl;
	outfile << "Pawns it is (Column of Last Tile)'x'(End Tile)." << endl;
}

void incrementTurn()
{
	turnNumber++;
}

/*
* void logMove(int startTile, int endTile, string piece, bool didTake, bool isFlip, bool isNewTurn)
*
* Takes a bunch of turn information and outputs it to a log file in accordance
* with professional chess algebraic notation. If it is the start of a new turn,
* a new line is started and the turn number is written.
*
* Inputs:
* int startTile:  The index of the starting tile of the piece moved during the turn
* int endTile:    The index number of the tile which was landed on
* string piece:   The name of the piece which was moved
* bool didTake:   If a piece has been taken on the turn it must be displayed on notation
* bool isFlip:    If the turn is black it is a flipped board. (DEPRECIATED)
*				  Alters the logic to find the tile in algebraic form
* bool isNewTurn: If the turn is white's it is a new turn.
*				  Determines the creation of a new line in the log
*
*/
void logMove(int startTile, int endTile, string piece, bool didTake)
{
	isNewTurn = !isNewTurn;
	char startCol, endCol, endRow;

	ofstream outfile("gameLog.txt", ios::app); // open the file without erasin contents

	if (isNewTurn)
	{
		turnNumber++;
		outfile << endl << turnNumber << ". ";
	}

	// Convert 1-D index to 2D algebraic chess notation
	if (isNewTurn)
	{
		startCol = (startTile % 8 + 'a');
		endCol = (endTile % 8 + 'a');
		endRow = (8 - (endTile / 8) + '1');
	}
	else
	{
		startCol = ('h' - startTile % 8);
		endCol = ('h' - endTile % 8);
		endRow = ((endTile / 8) + '1');
	}

	// Write the correct chess notation piece character to a variable
	char pieceChar = ' ';
	if (piece == "Rook") pieceChar = 'R';
	else if (piece == "Bishop") pieceChar = 'B';
	else if (piece == "Knight") pieceChar = 'N';
	else if (piece == "Queen") pieceChar = 'Q';
	else if (piece == "King") pieceChar = 'K';
	else pieceChar = 0;

	if (didTake)
	{
		if (!pieceChar) // If pawn
		{
			outfile << startCol << 'x' << endCol << endRow << ' ';
		}
		else
		{
			outfile << pieceChar << 'x' << endCol << endRow << ' ';
		}

	}
	else
	{
		outfile << pieceChar << endCol << endRow << ' ';
	}



}

/*
* void logMove(int xStart, int xEnd, int yEnd, string piece, bool didTake, bool isNewTurn)
*
* Takes a bunch of turn information and outputs it to a log file in accordance
* with professional chess algebraic notation. If it is the start of a new turn,
* a new line is started and the turn number is written.
*
* Inputs:
* int xStart:     The xindex of the starting tile of the piece moved during the turn
* int xEnd:       The xindex of the tile which was landed on
* int yEnd:       The yIndex of the tile landed on
* string piece:   The name of the piece which was moved
* bool didTake:   If a piece has been taken on the turn it must be displayed on notation
*/
void logMove(int xStart, int xEnd, int yEnd, string piece, bool didTake)
{
	isNewTurn = !isNewTurn;
	char startCol, endCol, endRow;

	ofstream outfile("gameLog.txt", ios::app); // open the file without erasin contents

	if (isNewTurn)
	{
		turnNumber++;
		outfile << endl << turnNumber << ". ";
	}

	// Convert 1-D index to 2D algebraic chess notation
	if (isNewTurn)
	{
		startCol = (xStart + 'a');
		endCol = (xEnd + 'a');
		endRow = (8 - (yEnd)+'0');
	}
	else
	{
		startCol = ('h' - xStart);
		endCol = ('h' - xEnd);
		endRow = ((yEnd)+'1');
	}

	// Write the correct chess notation piece character to a variable
	char pieceChar = ' ';
	if (piece == "Rook") pieceChar = 'R';
	else if (piece == "Bishop") pieceChar = 'B';
	else if (piece == "Knight") pieceChar = 'N';
	else if (piece == "Queen") pieceChar = 'Q';
	else if (piece == "King") pieceChar = 'K';
	else pieceChar = 0;

	if (didTake)
	{
		if (!pieceChar) // If pawn
		{
			outfile << startCol << 'x' << endCol << endRow << ' ';
		}
		else
		{
			outfile << pieceChar << 'x' << endCol << endRow << ' ';
		}

	}
	else
	{
		outfile << pieceChar << endCol << endRow << ' ';
	}
}