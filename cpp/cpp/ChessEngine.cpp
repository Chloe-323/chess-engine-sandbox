#include "ChessEngine.h"

ChessEngine::ChessEngine(chess::Board* board, int depth, int beamWidth) {
    this->depth = depth;
    this->beamWidth = beamWidth;
    this->currentState = board;
}

ChessEngine::~ChessEngine() {
}

void ChessEngine::makeMove(chess::Move move) {
    this->currentState->makeMove(move);
}

chess::Move ChessEngine::getBestMove() {
    std::mt19937 gen(std::random_device{}()); 
    chess::Movelist legalMoves = this->calculateLegalMoves(this->currentState);
    std::uniform_int_distribution<> dis(0, legalMoves.size() - 1);
    return legalMoves[dis(gen)];
}

float ChessEngine::evaluate(chess::Board* position) {
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

float ChessEngine::constantTimeEvaluate(chess::Board* position) {
    return 0;
}

searchNode ChessEngine::alphaBetaSearch() {
    return { 0, chess::Move() };
}

searchNode ChessEngine::bestMoveForWhite(int curDepth, float alpha, float beta) {
    return { 0, chess::Move() };
}

searchNode ChessEngine::bestMoveForBlack(int curDepth, float alpha, float beta) {
    return { 0, chess::Move() };
}