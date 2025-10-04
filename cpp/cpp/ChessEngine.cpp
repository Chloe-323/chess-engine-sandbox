#include "ChessEngine.h"

ChessEngine::ChessEngine(chess::Board* board, int depth, int beamWidth) {
    this->depth = depth;
    this->beamWidth = beamWidth;
    this->currentState = board;
    this->debug = false;
    std::random_device rd;
    this->gen = std::mt19937(rd());
}

ChessEngine::~ChessEngine() {
}

void ChessEngine::makeMove(chess::Move move) {
    this->currentState->makeMove(move);
}

chess::Move ChessEngine::getBestMove() {
    auto toReturn = alphaBetaSearch();
    return toReturn;
}

int16_t ChessEngine::evaluate(chess::Board* position) {
    if (position == nullptr) position = this->currentState;
    return 0;
}

bool ChessEngine::isLegalMove(chess::Move move, chess::Board* position) {
    if (position == nullptr) position = this->currentState;
    chess::Movelist legalMoves = this->calculateLegalMoves(position);
    return std::find(legalMoves.begin(), legalMoves.end(), move) != legalMoves.end();
}

chess::Movelist ChessEngine::calculateLegalMoves(chess::Board* position) {
    if (position == nullptr) position = this->currentState;
    chess::Movelist toReturn;
    chess::movegen::legalmoves(toReturn, *position);
    return toReturn;
}

int16_t ChessEngine::constantTimeEvaluate(chess::Board* position, chess::Movelist* legalMoves ) {
    if (legalMoves == nullptr) {
        chess::Movelist moves = calculateLegalMoves(position);
        legalMoves = &moves;
    }
    switch (getGameState(position, legalMoves)) {
    case STILL_PLAYING:
        break;
    case BLACK_WINS:
        return -0x7fff;
    case DRAW:
        return 0;
    case WHITE_WINS:
        return 0x7fff;
    }

    int16_t result = (countMaterial(position) << 8) + (countPositionalControl(position)) + std::uniform_int_distribution<int>(-5, 5)(this->gen);
    return result;
}

chess::Move ChessEngine::alphaBetaSearch() {
    if (this->currentState->sideToMove() == chess::Color::WHITE) {
        return this->bestMoveForWhite(this->currentState);
    }
    return this->bestMoveForBlack(this->currentState);
}

chess::Move ChessEngine::bestMoveForWhite(chess::Board* position, int curDepth, int16_t alpha, int16_t beta) {
    chess::Movelist legalMoves = calculateLegalMoves(position);
    chess::Move toReturn = chess::Move();
    toReturn.setScore(-0x7fff);
    if (curDepth >= this->depth || getGameState(position, &legalMoves) != STILL_PLAYING) {
        toReturn.setScore(constantTimeEvaluate(position, &legalMoves));
        return toReturn;
    }

    for (chess::Move& move : legalMoves) {
        position->makeMove(move);
        move.setScore(constantTimeEvaluate(position));
        position->unmakeMove(move);
    }

    std::sort(legalMoves.begin(), legalMoves.end(), std::greater<chess::Move>());

    for (int i = 0; i < std::min(this->beamWidth, legalMoves.size()); i++) {
        if (this->debug) {
            for (int j = 0; j < curDepth; j++) { std::cout << "\t"; }
            std::cout << "Looking at " << chess::uci::moveToSan(*position, legalMoves[i]) << std::endl;
        }
        position->makeMove(legalMoves[i]);
        legalMoves[i].setScore(bestMoveForBlack(position, curDepth + 1, alpha, beta).score());
        position->unmakeMove(legalMoves[i]);

        if (this->debug) {
            for (int j = 0; j < curDepth; j++) { std::cout << "\t"; }
            std::cout << " score: " << legalMoves[i].score() << std::endl;
        }
        toReturn = toReturn > legalMoves[i] ? toReturn : legalMoves[i];

        if (legalMoves[i].score() >= beta) {
            break;
        }
        alpha = std::max(alpha, legalMoves[i].score());
    }
    if (this->debug) {
        for (int j = 0; j < curDepth; j++) { std::cout << "\t"; }
        std::cout << "Picked move: " << chess::uci::moveToSan(*position, toReturn) << std::endl;
    }
    return toReturn;
}

chess::Move ChessEngine::bestMoveForBlack(chess::Board* position, int curDepth, int16_t alpha, int16_t beta) {
    chess::Movelist legalMoves = calculateLegalMoves(position);
    chess::Move toReturn = chess::Move();
    toReturn.setScore(0x7fff);
    if (curDepth >= this->depth || getGameState(position, &legalMoves) != STILL_PLAYING) {
        toReturn.setScore(constantTimeEvaluate(position, &legalMoves));
        return toReturn;
    }

    for (chess::Move& move : legalMoves) {
        position->makeMove(move);
        move.setScore(constantTimeEvaluate(position));
        position->unmakeMove(move);
    }

    std::sort(legalMoves.begin(), legalMoves.end());

    for (int i = 0; i < std::min(this->beamWidth, legalMoves.size()); i++) {
        if (this->debug) {
            for (int j = 0; j < curDepth; j++) { std::cout << "\t"; }
            std::cout << "Looking at " << chess::uci::moveToSan(*position, legalMoves[i]) << std::endl;
        }
        position->makeMove(legalMoves[i]);
        legalMoves[i].setScore(bestMoveForWhite(position, curDepth + 1, alpha, beta).score());
        position->unmakeMove(legalMoves[i]);

        if (this->debug) {
            for (int j = 0; j < curDepth; j++) { std::cout << "\t"; }
            std::cout << " score: " << legalMoves[i].score() << std::endl;
        }
        toReturn = toReturn < legalMoves[i] ? toReturn : legalMoves[i];

        if (legalMoves[i].score() <= alpha) {
            break;
        }
        if (this->debug) {
            for (int j = 0; j < curDepth; j++) { std::cout << "\t"; }
            std::cout << "Picked move: " << chess::uci::moveToSan(*position, toReturn) << std::endl;
        }
        beta = std::min(beta, legalMoves[i].score());
    }
    return toReturn;
}

GameState ChessEngine::getGameState(chess::Board* position, chess::Movelist* legalMoves) {
    if (position->isInsufficientMaterial()) return DRAW;
    if (position->isRepetition()) return DRAW;
    if (position->isHalfMoveDraw() || legalMoves->empty()) {
        if (position->inCheck()) return position->sideToMove() == chess::Color::WHITE ? BLACK_WINS : WHITE_WINS;
        return DRAW;
    }
    return STILL_PLAYING;
}


int16_t ChessEngine::countMaterial(chess::Board* position) {
    int16_t total = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            chess::Piece piece = position->at(chess::Square(chess::File(i), chess::Rank(j)));
            if (piece == chess::Piece::NONE) {
                continue;
            } else if (piece == chess::Piece::WHITEPAWN) {
                total += 1;
            } else if (piece == chess::Piece::WHITEKNIGHT) {
                total += 3;
            } else if (piece == chess::Piece::WHITEBISHOP) {
                total += 3;
            } else if (piece == chess::Piece::WHITEROOK) {
                total += 5;
            } else if (piece == chess::Piece::WHITEQUEEN) {
                total += 9;
            } else if (piece == chess::Piece::WHITEKING) {
                total += 0;
            } else if (piece == chess::Piece::BLACKPAWN) {
                total -= 1;
            } else if (piece == chess::Piece::BLACKKNIGHT) {
                total -= 3;
            } else if (piece == chess::Piece::BLACKBISHOP) {
                total -= 3;
            } else if (piece == chess::Piece::BLACKROOK) {
                total -= 5;
            } else if (piece == chess::Piece::BLACKQUEEN) {
                total -= 9;
            } else if (piece == chess::Piece::BLACKKING) {
                total -= 0;
            }
        }
    }
    return total;
}

int16_t ChessEngine::countRelativeMaterial(chess::Board* position) {
    int16_t total = 0;
    int16_t difference = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            chess::Piece piece = position->at(chess::Square(chess::File(i), chess::Rank(j)));
            if (piece == chess::Piece::NONE) {
                continue;
            }
            else if (piece == chess::Piece::WHITEPAWN) {
                total += 1;
                difference += 1;
            }
            else if (piece == chess::Piece::WHITEKNIGHT) {
                total += 3;
                difference += 3;
            }
            else if (piece == chess::Piece::WHITEBISHOP) {
                total += 3;
                difference += 3;
            }
            else if (piece == chess::Piece::WHITEROOK) {
                total += 5;
                difference += 5;
            }
            else if (piece == chess::Piece::WHITEQUEEN) {
                total += 9;
                difference += 9;
            }
            else if (piece == chess::Piece::WHITEKING) {
                total += 0;
                difference += 0;
            }
            else if (piece == chess::Piece::BLACKPAWN) {
                total += 1;
                difference -= 1;
            }
            else if (piece == chess::Piece::BLACKKNIGHT) {
                total += 3;
                difference -= 3;
            }
            else if (piece == chess::Piece::BLACKBISHOP) {
                total += 3;
                difference -= 3;
            }
            else if (piece == chess::Piece::BLACKROOK) {
                total += 5;
                difference -= 5;
            }
            else if (piece == chess::Piece::BLACKQUEEN) {
                total += 9;
                difference -= 9;
            }
            else if (piece == chess::Piece::BLACKKING) {
                total += 0;
                difference -= 0;
            }
        }
    }
    return (difference << 15) / total; //TODO: maybe a cheaper operation?
}

int16_t ChessEngine::countPositionalControl(chess::Board* position) {
    int16_t positionalControl = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            chess::Square square = chess::Square(chess::File(i), chess::Rank(j));
            int16_t squareValue = ChessEngine::squareValueLookupTable[i][j];
            chess::Bitboard blackAttackers = chess::attacks::attackers(*position, chess::Color::BLACK, square);
            positionalControl -= blackAttackers.count() * squareValue;
            chess::Bitboard whiteAttackers = chess::attacks::attackers(*position, chess::Color::WHITE, square);
            positionalControl += whiteAttackers.count() * squareValue;
        }
    }
    return positionalControl;
}

int16_t ChessEngine::countPieceActivity(chess::Board* position) {
    return 0;
}
int16_t ChessEngine::countPawnStructure(chess::Board* position) {
    chess::Bitboard whitePawns = position->pieces(chess::PieceType::PAWN, chess::Color::WHITE);
    chess::Bitboard blackPawns = position->pieces(chess::PieceType::PAWN, chess::Color::BLACK);

    //How are we counting pawn structure?
    //Length of pawn chains: each pawn is part of a chain. I'm thinking we do, for each pawn, score += 1 << std::min(4, pawnChainLength)
    //Pawn islands: each pawn is part of an island. Get the width of that island. This punishes isolated and doubled pawns. We can do, for each island, score += 1 << min(6, islandWidth).

    //so we wanna do it by file. a, b, c... What's the best way to do it? Bits are ordered as a1, b1, c1, d1, etc...
    //Create a bitmask for each file. Then, make a file bitboard by ANDing the bitmask with the pawn bitboard. Then, use LSB on that. 
    int64_t fileA = 0x0101010101010101;
    int64_t fileB = 0x0202020202020202;
    int64_t fileC = 0x0404040404040404;
    int64_t fileD = 0x0808080808080808;
    int64_t fileE = 0x1010101010101010;
    int64_t fileF = 0x2020202020202020;
    int64_t fileG = 0x4040404040404040;
    int64_t fileH = 0x8080808080808080;

    int64_t fileMasks[8] = {
    0x0101010101010101,
    0x0202020202020202,
    0x0404040404040404,
    0x0808080808080808,
    0x1010101010101010,
    0x2020202020202020,
    0x4040404040404040,
    0x8080808080808080
    };
    return 0;
}
int16_t ChessEngine::countKingSafety(chess::Board* position) {
    return 0;
}

const int16_t ChessEngine::squareValueLookupTable[8][8] = {
    {2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 3, 3, 3, 3, 2, 2},
    {2, 2, 3, 4, 4, 3, 2, 2},
    {2, 2, 3, 4, 4, 3, 2, 2},
    {2, 2, 3, 3, 3, 3, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 2},
};