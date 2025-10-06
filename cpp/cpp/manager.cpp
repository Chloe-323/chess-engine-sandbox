#include <iostream>
#include "chess.hpp"
#include "ChessEngine.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <chrono>

#define BOARD_SIZE 800
#define SQUARE_SIZE (BOARD_SIZE / 8)
#define LIGHT_SQUARE_COLOR sf::Color(211, 225, 224)
#define DARK_SQUARE_COLOR sf::Color(107,153,152)
#define LIGHT_SELECTED_SQUARE_COLOR sf::Color(24, 222, 209)
#define DARK_SELECTED_SQUARE_COLOR sf::Color(4, 224, 199)

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

		switch (toupper(type)) {
		case 'P':
			this->pieceType = chess::PieceType::PAWN;
			break;
		case 'N':
			this->pieceType = chess::PieceType::KNIGHT;
			break;
		case 'B':
			this->pieceType = chess::PieceType::BISHOP;
			break;
		case 'R':
			this->pieceType = chess::PieceType::ROOK;
			break;
		case 'Q':
			this->pieceType = chess::PieceType::QUEEN;
			break;
		case 'K':
			this->pieceType = chess::PieceType::KING;
			break;
		}

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

	chess::PieceType getPieceType() { return this->pieceType; }
private:
	sf::RectangleShape *object;
	sf::Texture *texture;
	char type;
	bool isWhite;
	chess::PieceType pieceType;
	int x, y;
};

struct square {
	sf::RectangleShape* sq; //Graphical representation of the square
	Piece* currentPiece;
	chess::Square chessSq; //square for the chess library
};

enum ManagerState {
	WAITING_FOR_PLAYER_MOVE,
	WAITING_FOR_ENGINE_MOVE,
	ENGINE_PLAYED_MOVE,
};

void getRepr(std::string fen, char repr[8][8], bool playingWhite) {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			repr[i][j] = ' ';
		}
	}
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
			repr[playingWhite ? row : 7 - row][playingWhite ? col : 7 - col] = c;
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

void getBoardGraphicalRepresentation(char repr[8][8], square squares[8][8], bool playingWhite, square *selectedSquare) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {


			squares[i][j].chessSq = chess::Square(chess::File(playingWhite ? j : 7 - j), chess::Rank(playingWhite ? 7 - i : i));

			if (squares[i][j].sq == nullptr) {
				squares[i][j].sq = new sf::RectangleShape({ SQUARE_SIZE, SQUARE_SIZE });
				squares[i][j].sq->setPosition({ float(j * SQUARE_SIZE), float(i * SQUARE_SIZE) });
			}
			if ((i + j) % 2 == 0) {
				squares[i][j].sq->setFillColor(selectedSquare == &squares[i][j] ? LIGHT_SELECTED_SQUARE_COLOR : LIGHT_SQUARE_COLOR);
			}
			else {
				squares[i][j].sq->setFillColor(selectedSquare == &squares[i][j] ? DARK_SELECTED_SQUARE_COLOR : DARK_SQUARE_COLOR);
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
	ChessEngine engine(&board, 6, 12);
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
	getRepr(fen, repr, playingWhite);

	square *selectedSquare = nullptr;

	ManagerState managerState = playingWhite == (board.sideToMove() == chess::Color::WHITE) ? WAITING_FOR_PLAYER_MOVE : WAITING_FOR_ENGINE_MOVE;

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
			if (event->is<sf::Event::MouseButtonPressed>()) {
				if (event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
					square *newSquare = &squares[
							event->getIf<sf::Event::MouseButtonPressed>()->position.y / SQUARE_SIZE][
							event->getIf<sf::Event::MouseButtonPressed>()->position.x / SQUARE_SIZE
						];

					if (managerState == WAITING_FOR_PLAYER_MOVE && selectedSquare != nullptr && selectedSquare->currentPiece != nullptr) {
						chess::Move move;

						// Check if this is a castling move (king moves two squares)
						if (selectedSquare->currentPiece->getPieceType() == chess::PieceType::KING &&
							std::abs(static_cast<int>(newSquare->chessSq.file()) - static_cast<int>(selectedSquare->chessSq.file())) == 2) {
							// Determine if it's kingside or queenside castling
							bool isKingside = (newSquare->chessSq.file() > selectedSquare->chessSq.file());
							move = chess::uci::parseSan(board, isKingside ? "O-O" : "O-O-O");
						}
						// Handle pawn promotion
						else if (selectedSquare->currentPiece->getPieceType() == chess::PieceType::PAWN && 
							chess::Square::back_rank(newSquare->chessSq, playingWhite ? chess::Color::WHITE : chess::Color::BLACK)) {
							move = chess::Move::make(selectedSquare->chessSq, newSquare->chessSq, chess::PieceType::QUEEN);
						}
						else {
							move = chess::Move::make(selectedSquare->chessSq, newSquare->chessSq);
						}

						if (engine.isLegalMove(move)) {
							engine.makeMove(move);
							fen = board.getFen();
							getRepr(fen, repr, playingWhite);
							managerState = WAITING_FOR_ENGINE_MOVE;
							selectedSquare = nullptr;
							continue;
						}
						else {
							selectedSquare = newSquare;
						}
					}
					selectedSquare = newSquare;
					
				}
				else {
					selectedSquare = nullptr;
				}
			}
		}

		window.clear();
		
		getBoardGraphicalRepresentation(repr, squares, playingWhite, selectedSquare);
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
		if (managerState == WAITING_FOR_ENGINE_MOVE) {
			//TODO: make this on a separate thread.
			auto start = std::chrono::high_resolution_clock::now();
			chess::Move move = engine.getBestMove();
			std::cout << chess::uci::moveToSan(board, move) << std::endl;
			engine.makeMove(move);
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> duration = end - start; // Duration in milliseconds
			std::cout << "Execution time: " << duration.count() << " s" << std::endl;
			fen = board.getFen();
			getRepr(fen, repr, playingWhite);
			std::cout << "\n\n";
			consolePrintRepr(repr);
			std::cout << "\n\n";
			managerState = ENGINE_PLAYED_MOVE;
			selectedSquare = nullptr;
		}
		else if (managerState == ENGINE_PLAYED_MOVE) {
			managerState = WAITING_FOR_PLAYER_MOVE;
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			delete squares[i][j].sq;
			delete squares[i][j].currentPiece;
		}
	}
	return 0;
}