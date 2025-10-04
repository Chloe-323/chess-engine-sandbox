#pragma once
#include "chess.hpp"
#include <random>
#include <algorithm>
#include <cmath>

enum GameState {
    STILL_PLAYING,
    BLACK_WINS,
    DRAW,
    WHITE_WINS
};

class ChessEngine {
    public:
        ChessEngine(chess::Board* board, int depth, int beamWidth);
        ~ChessEngine();
        void makeMove(chess::Move move);
        chess::Move getBestMove();
        int16_t evaluate(chess::Board* position);
        bool isLegalMove(chess::Move move, chess::Board* position = nullptr);

        //getters and setters
        chess::Board* getCurrentState() { return this->currentState; }
    private:
        chess::Movelist calculateLegalMoves(chess::Board* position);
        chess::Move alphaBetaSearch();
        chess::Move bestMoveForWhite(chess::Board* position, int curDepth = 0, int16_t alpha = -0x7fff, int16_t beta = 0x7fff);
        chess::Move bestMoveForBlack(chess::Board* position, int curDepth = 0, int16_t alpha = -0x7fff, int16_t beta = 0x7fff);
        GameState getGameState(chess::Board* position, chess::Movelist* legalMoves);

        int16_t constantTimeEvaluate(chess::Board* position, chess::Movelist* legalMoves = nullptr);
        int16_t countMaterial(chess::Board* position);
        int16_t countPositionalControl(chess::Board* position);
        int16_t countPawnStructure(chess::Board* position);


        //Implemented but unused
        int16_t countRelativeMaterial(chess::Board* position);
        int16_t countPieceMobility(chess::Board* position);

        //Not yet implemented
        int16_t countKingSafety(chess::Board* position);


        chess::Board* currentState;
        int depth;
        int beamWidth;
        bool debug;
        std::mt19937 gen;
        const static int16_t squareValueLookupTable[8][8];
};