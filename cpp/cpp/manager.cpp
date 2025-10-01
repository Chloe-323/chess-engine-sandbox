#include <iostream>
#include "chess.hpp"
#include "ChessEngine.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>

#define BOARD_SIZE 800
#define SQUARE_SIZE (BOARD_SIZE / 8)
#define LIGHT_SQUARE_COLOR sf::Color(211, 225, 224)
#define DARK_SQUARE_COLOR sf::Color(107,153,152)

class Piece {
public:
	Piece(char type) {
		this->object = new sf::RectangleShape({ SQUARE_SIZE, SQUARE_SIZE });
		this->texture = new sf::Texture();
		this->isWhite = isupper(type);

		std::string filename = "../../images/";
		filename += this->isWhite ? "w" : "b";
		filename += toupper(type);
		filename += ".png";

		if (!texture->loadFromFile(filename)) {
			return;
		}
		object->setTexture(texture);
		return;
	}
	~Piece() {
		delete this->object;
		delete this->texture;
	}

    bool loadFromFile(const std::string& filename, float scale = 1.0f) {

    }

    void setPosition(int x, int y) {
        this->x = x;
        this->y = y;
		this->object->setPosition({ float(x), float(y)});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(*this->object);
    }
private:
	sf::RectangleShape *object;
	sf::Texture *texture;
	char type;
	bool isWhite;
	int x, y;
};

struct square {
	sf::RectangleShape *sq;
	Piece* currentPiece;
};

void getRepr(std::string fen, char repr[8][8]) {
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
}

void consolePrintRepr(char repr[8][8]) {
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
	std::cout << "\033[27m" << std::endl;
}

void getBoardGraphicalRepresentation(char repr[8][8], square squares[8][8], bool playingWhite) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (squares[i][j].sq == nullptr) {
				squares[i][j].sq = new sf::RectangleShape({ SQUARE_SIZE, SQUARE_SIZE });
				squares[i][j].sq->setPosition({ float(j * SQUARE_SIZE), float(i * SQUARE_SIZE) });
				if ((i + j) % 2 == 0) {
					squares[i][j].sq->setFillColor(LIGHT_SQUARE_COLOR);
				}
				else {
					squares[i][j].sq->setFillColor(DARK_SQUARE_COLOR);
				}
			}
			if (squares[i][j].currentPiece != nullptr) {
				delete squares[i][j].currentPiece;
			}
			if (repr[i][j] != ' ') {
				Piece* piece = new Piece(repr[i][j]);
				piece->setPosition(j * SQUARE_SIZE, i * SQUARE_SIZE);
				squares[i][j].currentPiece = piece;
			}
			else {
				squares[i][j].currentPiece = nullptr;
			}
		}
	}
}

int main() {
	chess::Board board = chess::Board();
	ChessEngine engine(&board, 2, 2);
	sf::RenderWindow window(sf::VideoMode({ BOARD_SIZE, BOARD_SIZE }), "Chess");
	window.setFramerateLimit(30);
	bool playingWhite = true;

	std::string fen = board.getFen();

	char repr[8][8] = {
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, 
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, 
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, 
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, 
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, 
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, 
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }, 
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', }
	};
	square squares[8][8] = {
		{{nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}}, 
		{{nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}}, 
		{{nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}}, 
		{{nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}}, 
		{{nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}}, 
		{{nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}}, 
		{{nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}}, 
		{{nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}, {nullptr, nullptr}} 
	};
	getRepr(fen, repr);


	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		window.clear();
		
		getBoardGraphicalRepresentation(repr, squares, playingWhite);
		// Draw the board
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				window.draw(*squares[j][i].sq);
				if (squares[j][i].currentPiece != nullptr) {
					squares[j][i].currentPiece->draw(window);
				}
			}
		}

		window.display();
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			delete squares[i][j].sq;
			delete squares[i][j].currentPiece;
		}
	}
	return 0;
}