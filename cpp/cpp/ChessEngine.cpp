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

    int16_t result = (countMaterial(position) << 8) + (countPositionalControl(position)) + (countPawnStructure(position)) + std::uniform_int_distribution<int>(-5, 5)(this->gen);
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
    for (std::pair < chess::PieceType, int16_t> piece : { 
        std::pair{chess::PieceType::PAWN, 1}, 
        std::pair{chess::PieceType::BISHOP, 3}, 
        std::pair{chess::PieceType::KNIGHT, 3},
        std::pair{chess::PieceType::ROOK, 5}, 
        std::pair{chess::PieceType::QUEEN,9} 
        }) {
        total += (position->pieces(piece.first, chess::Color::WHITE).count()) * piece.second;
        total -= (position->pieces(piece.first, chess::Color::BLACK).count()) * piece.second;
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

int16_t ChessEngine::countPieceMobility(chess::Board* position) {
    int16_t pieceMobility = 0;
    for (chess::Color color : {chess::Color::WHITE, chess::Color::BLACK}) {

        chess::Bitboard bitboard = position->pieces(chess::PieceType::PAWN, color);

        while (bitboard) {
            uint8_t sqIndex = bitboard.pop();
            if (color == chess::Color::WHITE) {
                pieceMobility += chess::attacks::pawn(color, chess::Square(sqIndex)).count();
            }
            else {
                pieceMobility -= chess::attacks::pawn(color, chess::Square(sqIndex)).count();
            }
        }

        bitboard = position->pieces(chess::PieceType::KNIGHT, color);
        while (bitboard) {
            uint8_t sqIndex = bitboard.pop();
            if (color == chess::Color::WHITE) {
                pieceMobility += chess::attacks::knight(chess::Square(sqIndex)).count();
            }
            else {
                pieceMobility -= chess::attacks::knight(chess::Square(sqIndex)).count();
            }
        }

        chess::Bitboard occupied = position->occ();
        auto handleSlidingPiece = [&](chess::PieceType type, auto attackFunc) {
            bitboard = position->pieces(type, color);
            while (bitboard) {
                uint8_t sqIndex = bitboard.pop();
                if (color == chess::Color::WHITE) {
                    pieceMobility += attackFunc(chess::Square(sqIndex), occupied).count();
                }
                else {
                    pieceMobility -= attackFunc(chess::Square(sqIndex), occupied).count();
                }
            }
        };
        handleSlidingPiece(chess::PieceType::BISHOP, chess::attacks::bishop);
        handleSlidingPiece(chess::PieceType::ROOK, chess::attacks::rook);
        handleSlidingPiece(chess::PieceType::QUEEN, chess::attacks::queen);
    }
    return pieceMobility;
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

    constexpr std::array<chess::Bitboard, 15> diagonalMasks = {
        0x0000000000000001, // a1
        0x0000000000000102, // a2 - b1
        0x0000000000010204, // a3 - b2 - c1
        0x0000000001020408, // a4 - b3 - c2 - d1
        0x0000000102040810, // a5 - b4 - c3 - d2 - e1
        0x0000010204081020, // a6 - b5 - c4 - d3 - e2 - f1
        0x0001020408102040, // a7 - b6 - c5 - d4 - e3 - f2 - g1
        0x0102040810204080, // a8 - b7 - c6 - d5 - e4 - f3 - g2 - h1
        0x0204081020408000, // b8 - c7 - d6 - e5 - f4 - g3 - h2
        0x0408102040800000, // c8 - d7 - e6 - f5 - g4 - h3
        0x0810204080000000, // d8 - e7 - f6 - g5 - h4
        0x1020408000000000, // e8 - f7 - g6 - h5
        0x2040800000000000, // f8 - g7 - h6
        0x4080000000000000, // g8 - h7
        0x8000000000000000, // h8
    };

    constexpr std::array<chess::Bitboard, 15> antiDiagonalMasks {    
        0x0000000000000080, // h1
        0x0000000000008040, // h2 - g1
        0x0000000000804020, // h3 - g2 - f1
        0x0000000080402010, // h4 - g3 - f2 - e1
        0x0000008040201008, // h5 - g4 - f3 - e2 - d1
        0x0000804020100804, // h6 - g5 - f4 - e3 - d2 - c1
        0x0080402010080402, // h7 - g6 - f5 - e4 - d3 - c2 - b1
        0x8040201008040201, // h8 - g7 - f6 - e5 - d4 - c3 - b2 - a1
        0x4020100804020100, // g8 - f7 - e6 - d5 - c4 - b3 - a2
        0x2010080402010000, // f8 - e7 - d6 - c5 - b4 - a3
        0x1008040201000000, // e8 - d7 - c6 - b5 - a4
        0x0804020100000000, // d8 - c7 - b6 - a5
        0x0402010000000000, // c8 - b7 - a6
        0x0201000000000000, // b8 - a7
        0x0100000000000000  // a8
    };

    int16_t longestWhitePawnChainLength = 0;
    int16_t longestBlackPawnChainLength = 0;
    for (auto [masks, stride] : {
        std::pair(diagonalMasks, 9),
        std::pair(antiDiagonalMasks, 7)
        }) {
        for (auto [pawnBitboard, longest] : {
            std::pair(whitePawns, &longestWhitePawnChainLength),
            std::pair(blackPawns, &longestBlackPawnChainLength)
            })
        {
            for (chess::Bitboard diagonalMask : masks) {
                chess::Bitboard pawnsOnDiagonal = pawnBitboard & diagonalMask;
                int8_t prevSquare = -1;
                int16_t currentChain = 0, bestChain = 0;
                while (pawnsOnDiagonal) {
                    int8_t sq = pawnsOnDiagonal.pop();
                    if (prevSquare != -1 && sq == prevSquare + stride) currentChain++;
                    else currentChain = 1;
                    prevSquare = sq;
                    bestChain = std::max(bestChain, currentChain);
                }
                *longest = std::max(*longest, bestChain);
            }
        }
    }
    return (2 << longestWhitePawnChainLength) - (2 << longestBlackPawnChainLength);
}
int16_t ChessEngine::countKingSafety(chess::Board* position) {
    //# of checks. carries between sub-branches. figure out how the math looks for that.
    //# of pieces pinned to the king.
    //
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