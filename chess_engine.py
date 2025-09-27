import chess
import random
import heapq
from collections import deque

class Engine:
    def __init__(self, board: chess.Board, depth: int = 3):
        self.board = board
        self.depth = depth
    
    def play(self, move: chess.Move):
        self.board.push(move)
    
    def pick_move(self):
        return self.best_move_for_current_player()[0]

    def evaluate(self, cur_depth: int = 0) -> float:
        """Return a score for the current board position. Checkmate for white is +inf, checkmate for black is -inf."""
        if self.board.is_game_over():
            if self.board.is_checkmate():
                return -100000 if self.board.turn else 100000
            if self.board.is_stalemate():
                return 0
            if self.board.is_insufficient_material():
                return 0
            if self.board.is_seventyfive_moves():
                return 0
            if self.board.is_fivefold_repetition():
                return 0
            return 0
        if cur_depth == self.depth:
            return self.count_relative_material() + (random.random() / 100)
        
        return self.best_move_for_current_player(cur_depth)[1]

    def best_move_for_current_player(self, cur_depth: int = 0) -> (chess.Move, float):
        queue = deque()
        for move in self.board.legal_moves:
            self.board.push(move)
            queue.append((move, self.evaluate(cur_depth + 1)))
            self.board.pop()
        if self.board.turn == chess.WHITE:
            return max(queue, key=lambda x: x[1])
        else:
            return min(queue, key=lambda x: x[1])
 

    def count_relative_material(self):
        difference = 0
        total_material = 0
        for square in chess.SQUARES:
            piece = self.board.piece_at(square)
            if piece is None:
                continue
            mult = 1 if piece.color == chess.WHITE else -1
            value = 0
            if piece.piece_type == chess.PAWN:
                value = 1
            elif piece.piece_type == chess.KNIGHT:
                value = 3
            elif piece.piece_type == chess.BISHOP:
                value = 3
            elif piece.piece_type == chess.ROOK:
                value = 5
            elif piece.piece_type == chess.QUEEN:
                value = 9
            elif piece.piece_type == chess.KING:
                value = 0
            difference += value * mult
            total_material += value
        return (difference / total_material) * 100

    def count_material(self):
        material = 0
        for square in chess.SQUARES:
            piece = self.board.piece_at(square)
            if piece is None:
                continue
            mult = 1 if piece.color == chess.WHITE else -1
            value = 0
            if piece.piece_type == chess.PAWN:
                value = 1
            elif piece.piece_type == chess.KNIGHT:
                value = 3
            elif piece.piece_type == chess.BISHOP:
                value = 3
            elif piece.piece_type == chess.ROOK:
                value = 5
            elif piece.piece_type == chess.QUEEN:
                value = 9
            elif piece.piece_type == chess.KING:
                value = 0
            material += value * mult
        return material