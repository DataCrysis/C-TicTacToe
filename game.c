#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>


#define TTT_X 1
#define TTT_O 2
#define TTT_N 0


typedef enum {PLAYER_X, PLAYER_O} Player;


typedef struct GameBoard {
	int board[3][3];
	Player player;
} GameBoard;


void init_GameBoard(GameBoard *gameboard) {
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			gameboard->board[i][j] = TTT_N;
		}
	}
	gameboard->player = PLAYER_X;
}


const char *get_symbol(int n) {
	return (n == TTT_N ? " " : (n == TTT_X ? "X" : "O"));
}


void ClearScreen() {
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = {0, 0};

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

void print_GameBoard(GameBoard *gameboard) {
	int* r0 = gameboard->board[0];
	int* r1 = gameboard->board[1];
	int* r2 = gameboard->board[2];
	printf("%s|%s|%s\n%s|%s|%s\n%s|%s|%s\n",
		get_symbol(r0[0]), get_symbol(r1[0]), get_symbol(r2[0]),
		get_symbol(r0[1]), get_symbol(r1[1]), get_symbol(r2[1]),
		get_symbol(r0[2]), get_symbol(r1[2]), get_symbol(r2[2])
	);
}


GameBoard *start_game() {
	GameBoard *gameboard = malloc(sizeof(GameBoard));
	assert(gameboard != NULL);
	init_GameBoard(gameboard);
	print_GameBoard(gameboard);
	return gameboard;
}


int get_digit_input(char *msg) {
	printf(msg);
	int i;
	scanf("%i", &i);
	return i;
}


int set_board_cell(GameBoard *gameboard, int x, int y, int n) {
	int cur_n = gameboard->board[x][y];
	if (cur_n == TTT_N) {
		gameboard->board[x][y] = n;
		return 1;
	}
	return 0;
}


void switch_turn(GameBoard *gameboard) {
	Player plr = (gameboard->player == PLAYER_X ? PLAYER_O : PLAYER_X);
	gameboard->player = plr;
}


int check_cell(GameBoard *gameboard, int x, int y, int n) {
	return (gameboard->board[x][y] == n);
}


int check_row(GameBoard *gameboard, int x, int n) {
	int* row = gameboard->board[x];
	int j;
	for (j = 0; j < 3; j++) {
		if (row[j] != n) return 0;
	}
	return 1;
}


int check_col(GameBoard *gameboard, int y, int n) {
	int j;
	for (j = 0; j < 3; j++) {
		if (gameboard->board[j][y] != n) return 0;
	}
	return 1;
}


int check_diag(GameBoard *gameboard, int d, int n) {
	int d0 = gameboard->board[0][d == 0 ? 0 : 2];
	int d1 = gameboard->board[1][1];
	int d2 = gameboard->board[2][d == 0 ? 2 : 0];
	return ((d0 == n && d1 == n && d2 == n) ? 1 : 0);
}


int check_winner(GameBoard *gameboard, int n) {
	int r0 = check_row(gameboard, 0, n);
	int r1 = check_row(gameboard, 1, n);
	int r2 = check_row(gameboard, 2, n);
	int c0 = check_col(gameboard, 0, n);
	int c1 = check_col(gameboard, 1, n);
	int c2 = check_col(gameboard, 2, n);
	int d0 = check_diag(gameboard, 0, n);
	int d1 = check_diag(gameboard, 1, n);
	return ((r0 || r1 || r2 || c0 || c1 || c2 || d0 || d1) ? 1 : 0);
}


int check_full(GameBoard *gameboard) {
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			int c = gameboard->board[i][j];
			if (c == TTT_N) {
				return 0;
			}
		}
	}
	return 1;
}


int game_turn(GameBoard *gameboard) {
	if (check_winner(gameboard, TTT_X)) return TTT_X;
	if (check_winner(gameboard, TTT_O)) return TTT_O;
	if (check_full(gameboard)) return TTT_N;
	Player plr = gameboard->player;
	int symb_n = (plr == PLAYER_X ? TTT_X : TTT_O);
	printf("\nPlayer %s:\n", get_symbol(symb_n));
	int x = get_digit_input("   X: ");
	int y = get_digit_input("   Y: ");
	printf("%d, %d\n", x, y);
	if (set_board_cell(gameboard, x, y, symb_n)) {
		switch_turn(gameboard);
		ClearScreen();
		print_GameBoard(gameboard);
	}
	return game_turn(gameboard);
}


void end_game(GameBoard *gameboard) {
	if (gameboard != NULL) {
		free(gameboard);
	}
}


int main(int argc, char *argv[]) {

	srand(time(NULL));

	int play_again;

	do {

		ClearScreen();

		GameBoard *gameboard = start_game();

		int winner = game_turn(gameboard);
		//int winner = TTT_N;

		ClearScreen();
		print_GameBoard(gameboard);

		switch(winner) {
			case TTT_X:
				printf("\nX wins!\n");
				break;
			case TTT_O:
				printf("\nO wins!\n");
				break;
			case TTT_N:
				printf("\nScratch\n");
				break;
		}

		end_game(gameboard);

		printf("Play again? (Y/N) ");

		char again[10];
		char y[] = "y";
		scanf("%s", again);

		play_again = strcmp(again, "y");
		if (play_again != 0) play_again = strcmp(again, "Y");

	} while (play_again == 0);

	return 0;

}
