#include "chess.hpp"
#include <random>

class ChessEngine {
    public:
        ChessEngine(chess::Board* board, int depth, int beamWidth);
        ~ChessEngine();
        void makeMove(chess::Move move);
        chess::Move getBestMove();
        float evaluate(chess::Board* position);

        //getters and setters
        chess::Board* getCurrentState() { return this->currentState; }
    private:
        chess::Board* currentState;
        int depth;
        int beamWidth;
};