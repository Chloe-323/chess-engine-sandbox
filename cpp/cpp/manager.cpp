#include <iostream>
#include "chess.hpp"
#include "ChessEngine.h"

int main() {
	ChessEngine engine(new chess::Board(), 2, 2);
	std::string fen = engine.getCurrentState()->getFen();
	char repr[8][8] = {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, };
	int row = 0;
	int col = 0;
	for (char c : fen) {
		if (c == '/') {
			row++;
			col = 0;
		}
		else if (c >= '0' && c <= '9') {
			col += c - '0';
		}
		else if (c == ' ') {
			break;
		}
		else {
			repr[row][col] = c;
			col++;
		}
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((i + j) % 2 == 0) {
				std::cout << "\033[7m";
			}
			else {
				std::cout << "\033[27m";
			}
			std::cout << repr[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << "\033[27m";
	chess::Move move = engine.getBestMove();
	std::cout << chess::uci::moveToSan(*engine.getCurrentState(), move);

	return 0;
}