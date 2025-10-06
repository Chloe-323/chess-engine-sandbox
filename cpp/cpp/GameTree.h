#pragma once
#include "chess.hpp"


class GameTreeNode {
public:
	GameTreeNode(chess::PackedBoard position, int16_t score);
	void destroy(GameTreeNode* goldenChild);

	GameTreeNode alphaBetaSearch();
	GameTreeNode bestMoveForWhite(int curDepth = 0, int16_t alpha = -0x7fff, int16_t beta = 0x7fff);
	GameTreeNode bestMoveForBlack(int curDepth = 0, int16_t alpha = -0x7fff, int16_t beta = 0x7fff);
private:
	~GameTreeNode() {}
	std::list<GameTreeNode*> children;
	chess::PackedBoard position;
	int16_t score;

	int16_t constantTimeEvaluate(chess::Movelist* legalMoves = nullptr);
	int16_t countMaterial(chess::Board* position);
	int16_t countPositionalControl(chess::Board* position);
	int16_t countPawnStructure(chess::Board* position);
};

