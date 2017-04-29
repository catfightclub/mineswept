// Project Name: SD4 - ASCII Minesweeper
//
// Project Members:
//		Alex Lanzetta
//		Jose Gonzalez
//		Janice Martyres
//
// Date: 4/27/17
//
// Honor Code: I have neither given nor received unauthorized aid in completing this work, nor have I presented someone else’s work as my own

#include <iostream>
#include <string>
#include <time.h>
#include <Windows.h>

using namespace std;

// Game parameters
int fieldWidth = 10;
int fieldHeight = 10;
int numMines = 0;
int numFlags = 0;

// Game states
bool inGame = false;
bool gameOver = true;

struct space {
	int adjMines;
	int x;
	int y;
	bool mine;
	bool flag;
	bool visible;

	// Turns on or off the flag on a tile
	// Can't toggle a visible tile
	void toggleFlag() {

			if (!visible)
			{
				if (flag)
				{
					numFlags++;
					flag = false;
				}
				else
				{
					if (numFlags > 0)
					{
						numFlags--;
						flag = true;
					}
					else
						cout << "\nYou dont have any flags!\n";
				}
			}
			else
				cout << "\nYou cant flag a visible space!\n";

	}


	void makeVisible();
};

void startMenu();
void mainMenu();
bool inGameMenu();
void generateMinefield();
int charToInt(char);
space * getTile(int, int);
void drawMinefield();
bool checkWin();

// Game board representing the mines
space** minefield;


//Shows instructions.
//Takes the dimensions of the board and the number of mines in it.
void startMenu() {
	int input = 0;

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); // Console object for changing colors
	int R = 4, G = 2, B = 1, BRIGHT = 8, BG = 16; // Useful values used for console color changing

	cout << "Welcome to ASCII Minesweeper! This is Minesweeper that works with commands.";
	cout << "\n\n\nINSTRUCTIONS\nThe rows of the board are labeled with letters, the columns with numbers.";
	cout << "\n\nTo perform an action, write the initial of the action: (";
	SetConsoleTextAttribute(console, R + G + BRIGHT + (B+BRIGHT)*BG);
	cout << "f";
	SetConsoleTextAttribute(console, R + G + B);
	cout << " for flag, ";
	SetConsoleTextAttribute(console, R + G + BRIGHT + (B + BRIGHT)*BG);
	cout << "s";
	SetConsoleTextAttribute(console, R + G + B);
	cout << " for show),followed by a letter for the row, and a number for the column.";
	cout << "\n\nFor example: \"f a 2\" will put a <f>lag on the second column of the first row.\n";
	cout << "\n\nNEW GAME:";
	
	while (input < 3 || input > 25)
	{
		cout << "\nPlease enter the desired width of the board: ";
		cin >> input;

		if (input < 3 || input > 25)
		{
			cout << "Wrong size. Please enter a number between 3 and 25.\n";
		}
		else
			fieldWidth = input;

		cin.clear();
		fflush(stdin);
	}

	input = 0;
	while (input < 3 || input > 25) {
		cout << "\nPlease enter the desired height of the board: ";
		cin >> input;

		if (input < 3 || input > 25) {
			cout << "Wrong size. Please enter a number between 3 and 25.\n";
		} else
			fieldHeight = input;

		cin.clear();
		fflush(stdin);
	}

	input = 0;

	while (input < 1 || input >= fieldHeight * fieldWidth)
	{
		cout << "\nPlease enter the desired number of mines on the board: ";
		cin >> input;

		if (input < 1 || input > (fieldHeight * fieldWidth) - 1)
		{
			cout << "Please enter a number between 1 and " << (fieldHeight * fieldWidth) - 1 << ".\n";
		}
		else
			numMines = input;

		cin.clear();
		fflush(stdin);
	}
	cout << "\n\n";

	generateMinefield();
}


//Has the loops that keep the game going.
void mainMenu() {
	numFlags = 0;
	char input = 'y';

	while (input == 'y' || input == 'Y')
	{
		// If you selected play
		system("cls");
		inGame = false;
		gameOver = false;
		int numberOfTurns = 0;

		startMenu();
		inGame = true;
		numFlags = numMines;

		while (true) {
			if (inGame) {
				if (gameOver) {
					drawMinefield();
					cout << "\n\nThat was a mine!\n=== You Lost! === \n\n";
					break;
				}
				else {
					//Game loop
					inGame = inGameMenu();
				}
				numberOfTurns++;
			}
			else {
				drawMinefield();
				cout << "\n\n=== You Won! ===\n\n";
				break;
			}
		}

		cout << "\nThis match was " << numberOfTurns << " turns long.";
		cout << "\nYou want to play again? y/n: ";
		cin >> input;
	}
}


bool inGameMenu() {
	char action;
	int X;
	char Y;

	int temp;

	drawMinefield();
	while (true)
	{
		cout << "\nYou have " << numFlags << " flags." << endl << "Enter action (<f>lag or <s>how, followed by row letter & column number): ";
		cin >> action >> Y >> X;
		temp = charToInt(Y);
		if ((temp >= 0 && temp < fieldHeight) && (X >= 1 && X < fieldWidth+1))
		{
			if (action == 'f' || action == 'F')
			{
				minefield[X - 1][temp].toggleFlag();
				break;
			}
			else if (action == 's' || action == 'S')
			{
				minefield[X-1][temp].makeVisible();
				break;
			}
			else
			{
				cout << "\nPlease enter a valid input.";
			}
		}
		else
		{
			cout << "\nPlease enter a valid input.";
			//X = 0;
			//Y = 'A';
		}
		cin.clear();
		fflush(stdin);
	}
	// Takes input and runs various functions below for running the game loop

	return !checkWin();
}

// Creates minefield based on the fieldWidth and fieldHeight, and numMines
void generateMinefield() {
	srand(time(NULL)); // Initialize the random number generator

	// Initialize the minefield data structure
	minefield = new space*[fieldWidth];
	for (int spotX = 0; spotX < fieldWidth; spotX++) {
		minefield[spotX] = new space[fieldHeight];
		for (int spotY = 0; spotY < fieldHeight; spotY++) {
			// Set all the spaces to 0 for now
			minefield[spotX][spotY] = space{ 0, spotX, spotY, false, false, false };
		}
	}

	// Place mines
	int randomNumber = rand();
	for (int mine = 0; mine < numMines; mine++) {
		bool foundSpot = false;

		do {
			int spotX = rand() % fieldWidth;
			int spotY = rand() % fieldHeight;

			if (spotX == 0 && spotY == 0) continue;		// Never place a mine at {0, 0}
			if (minefield[spotX][spotY].mine) continue;	// Don't place a mine over an existing mine

			minefield[spotX][spotY].mine = true;
			foundSpot = true;

		} while (!foundSpot);
	}

	// Determine the mine count on each space
	for (int spotX = 0; spotX < fieldWidth; spotX++) {
		for (int spotY = 0; spotY < fieldHeight; spotY++) {

			int adjMines = 0;

			for (int mineX = -1; mineX <= 1; mineX++) {
				for (int mineY = -1; mineY <= 1; mineY++) {
					// It's fine to check if self is a mine
					if (getTile(spotX + mineX, spotY + mineY) != nullptr && getTile(spotX + mineX, spotY + mineY)->mine) adjMines++;
				}
			}

			minefield[spotX][spotY].adjMines = adjMines;

		}
	}
}

int charToInt(char a)
{
	if (a >= 'a' && a <= 'z')
	{
		return (a - 97);
	}
	else if (a >= 'A' && a <= 'Z')
	{
		return (a - 65);
	}
	else
		return -1;
}

// Had to change this to space* because it is possible for a space to not be defined
// Error checking throughout the code should revolve around this change
space* getTile(int x, int y) {
	// Returns the space struct given x and y
	if (x >= 0 && x < fieldWidth && y >= 0 && y < fieldHeight) return &minefield[x][y];
	return nullptr;
}

// Equivalent to "clicking" on a board space in MS Minesweeper
// I had to move it below the getTile() function so it had it in scope - Alex
void space::makeVisible() {
	// Can't make invisible /again/
	if (visible) return;

	// Oops!
	if (mine) {
		// Force all tiles to be visible
		for (int spotX = 0; spotX < fieldWidth; spotX++) {
			for (int spotY = 0; spotY < fieldHeight; spotY++) {
				minefield[spotX][spotY].visible = true;
			}
		}
		gameOver = true;
		return;
	}

	// Turn off the flag and make it visible
	flag = false;
	visible = true;

	// If it has no mines next to it, unlock the tiles around it
	if (adjMines == 0) {
		// Make its adjacent tiles visible
		for (int adjX = -1; adjX <= 1; adjX++) {
			for (int adjY = -1; adjY <= 1; adjY++) {
				if (getTile(x + adjX, y + adjY) != nullptr && !getTile(x + adjX, y + adjY)->visible) getTile(x + adjX, y + adjY)->makeVisible();
			}
		}
	}
}

// Draws the field in-place in the console
void drawMinefield() {
	// If you use double width, the columns will be stretched, to allow the numbers to fit, and for it to have a better resolution
	// Personally I think it's uglier to have it stretched. The down-side to having it single-column, is that we have to skip numbers
	bool useDoubleWidth = false;

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); // Store the console for changing colors
	int R = 4, G = 2, B = 1, BRIGHT = 8, BG = 16;
	int colors[9] = {
		0 + 0 + 0 + BRIGHT + (0)*BG, // No mines
		0 + 0 + B + BRIGHT + (B)*BG, // One mine
		0 + G + 0 + BRIGHT + (G)*BG, // Two mines
		R + 0 + 0 + BRIGHT + (R)*BG, // ... so on
		0 + G + B + BRIGHT + (G + B)*BG,
		R + 0 + B + BRIGHT + (R + B)*BG,
		R + 0 + B + BRIGHT + (R + B)*BG,
		R + 0 + B + BRIGHT + (R + B)*BG,
		R + 0 + B + BRIGHT + (R + B)*BG
	};

	for (int spotY = -2; spotY < fieldHeight + 1; spotY++) {

		SetConsoleTextAttribute(console, (BRIGHT)*BG);

		// Draw Y axis headers
		if (spotY >= 0 && spotY != fieldHeight) {
			if (spotY >= 26) cout << char('A' + floor(spotY / 26));
			else cout << " ";
			cout << char('A' + floor(spotY % 26)) << " ";
		}
		else cout << "   ";

		for (int spotX = 0; spotX < fieldWidth; spotX++) {

			// Draw X axis headers
			if (spotY == -2) {

				// First digit of the top numbers
				if (spotX >= 10) {
					// If we're using single-wide spaces, skip every-other space for the numbers at the top
					if (useDoubleWidth || spotX % 2 == 0) cout << floor((spotX + 1) / 10);
					else cout << " ";
				}
				else cout << " ";
				if (useDoubleWidth) cout << " ";
				continue;

			}
			else if (spotY == -1) {

				// Second digit of the top numbers

				// If we're using single-wide spaces, skip every-other space for the numbers at the top
				if (useDoubleWidth || spotX % 2 == 0) cout << (spotX + 1) % 10;
				else cout << " ";
				if (useDoubleWidth) cout << " ";
				continue;

			}
			else if (spotY == fieldHeight) {

				// Put another blank row at the bottom of the field
				cout << " ";
				if (useDoubleWidth) cout << " ";
				continue;

			}

			// Draw the spaces. Colors and text depends on the state of the space
			space selectedSpace = minefield[spotX][spotY];
			if (selectedSpace.visible) {
				if (selectedSpace.mine) {
					SetConsoleTextAttribute(console, R + BRIGHT + (0)*BG);
					cout << "*";
				}
				else {
					SetConsoleTextAttribute(console, colors[selectedSpace.adjMines]);
					if (selectedSpace.adjMines > 0) {
						cout << selectedSpace.adjMines;
					}
					else {
						cout << ".";
					}
				}
			}
			else {
				if (selectedSpace.flag) {
					SetConsoleTextAttribute(console, R);
					cout << "F";
				}
				else {
					SetConsoleTextAttribute(console, BRIGHT);
					cout << "?";
				}
			}
			if (useDoubleWidth) cout << " ";
		}

		SetConsoleTextAttribute(console, (BRIGHT)*BG);
		cout << "  ";
		SetConsoleTextAttribute(console, 0);
		cout << " " << endl;
	}

	SetConsoleTextAttribute(console, R + G + B + (0)*BG); // Reset the console color for the rest of the interface
}

// Checks the entire minefield to determine if all mines have been properly flagged and all numbers have been set to visible
bool checkWin() {
	for (int spotX = 0; spotX < fieldWidth; spotX++) {
		for (int spotY = 0; spotY < fieldHeight; spotY++) {

			space* selectedSpace = getTile(spotX, spotY);

			// Mines must be flagged
			// All other spaces must be visible
			if (selectedSpace->mine) {
				if (!selectedSpace->flag) return false;
			}
			else if (!selectedSpace->visible) return false;

		}
	}

	// All spaces seem perfect! You win!!
	return true;
}


int main() {
	mainMenu();
	return 0;
}