#include "ChessEngine.h"

ChessEngine::ChessEngine(chess::Board* board, int depth, int beamWidth) {
    this->depth = depth;
    this->beamWidth = beamWidth;
    this->currentState = board;
}

ChessEngine::~ChessEngine() {
    delete this->currentState;
}

void ChessEngine::makeMove(chess::Move move) {
    this->currentState->makeMove(move);
}

chess::Move ChessEngine::getBestMove() {
    std::mt19937 gen(std::random_device{}()); 
    chess::Movelist legalMoves;
    chess::movegen::legalmoves(legalMoves, *this->currentState);
    std::uniform_int_distribution<> dis(0, legalMoves.size() - 1);
    return legalMoves[dis(gen)];
}

float ChessEngine::evaluate(chess::Board* position) {
    return 0;
}