#include "chess.hpp"
#include <random>
#include <algorithm>

struct searchNode {
    float score;
    chess::Move move;
};

class ChessEngine {
    public:
        ChessEngine(chess::Board* board, int depth, int beamWidth);
        ~ChessEngine();
        void makeMove(chess::Move move);
        chess::Move getBestMove();
        float evaluate(chess::Board* position);

        //getters and setters
        chess::Board* getCurrentState() { return this->currentState; }
        bool isLegalMove(chess::Move move, chess::Board* position = nullptr);
    private:
        chess::Movelist calculateLegalMoves(chess::Board* position);

        float constantTimeEvaluate(chess::Board* position);
        searchNode alphaBetaSearch();
        searchNode bestMoveForWhite(int curDepth, float alpha = -INFINITY, float beta = INFINITY);
        searchNode bestMoveForBlack(int curDepth, float alpha = -INFINITY, float beta = INFINITY);

        chess::Board* currentState;
        int depth;
        int beamWidth;
};