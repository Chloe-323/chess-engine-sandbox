import chess
import random
import heapq
from collections import deque


class Engine:
    def __init__(self, board: chess.Board, depth: int = 2):
        self.board = board
        self.depth = depth
        self.debug = True
    
    def pick_move(self):
        if self.debug:
            print("=======")
            print(f"Picking move for {"White" if self.board.turn else "Black"}")
        return self.alpha_beta_search()[1]

    def evaluate(self) -> float:
        """Return a score for the current board position. Checkmate for white is +inf, checkmate for black is -inf."""
        if self.board.is_game_over():
            return self.constant_time_evaluate()
        
        return self.alpha_beta_search()[0]

    def alpha_beta_search(self):
        if self.board.turn:
            return self.best_move_for_white()
        else:
            return self.best_move_for_black()

    def best_move_for_white(self, cur_depth: int = 0, alpha: float = -float('inf'), beta: float = float('inf')) -> (float, chess.Move):
        if cur_depth == self.depth or self.board.is_game_over():
            return (self.constant_time_evaluate(), None)
        value = -float('inf')
        best_move = None
        if self.debug:
            print(f"{'\t'*cur_depth}Looking at moves for White: Alpha: {alpha}, Beta: {beta}, depth: {cur_depth}")
        for move in self.ordered_legal_moves():
            if self.debug:
                print(f"{'\t'*(cur_depth + 1)}Looking at {self.board.san(move)}")
            self.board.push(move)
            move_value = self.best_move_for_black(cur_depth + 1, alpha, beta)[0]
            self.board.pop()
            if self.debug:
                print(f"{'\t'*(cur_depth + 1)}Move value: {move_value}")
            if move_value > value:
                value = move_value
                best_move = move
            if value >= beta:
                if self.debug:
                    print(f"{'\t'*(cur_depth + 1)}Beta cut")
                break
            alpha = max(alpha, value)
        return (value, best_move)

    def best_move_for_black(self, cur_depth: int = 0, alpha: float = -float('inf'), beta: float = float('inf')) -> (float, chess.Move):
        if cur_depth == self.depth or self.board.is_game_over():
            return (self.constant_time_evaluate(), None)
        value = float('inf')
        best_move = None
        if self.debug:
            print(f"{'\t'*cur_depth}Looking at moves for Black: Alpha: {alpha}, Beta: {beta}, depth: {cur_depth}")
        for move in self.ordered_legal_moves():
            if self.debug:
                print(f"{'\t'*(cur_depth + 1)}Looking at {self.board.san(move)}")
            self.board.push(move)
            move_value = self.best_move_for_white(cur_depth + 1, alpha, beta)[0]
            self.board.pop()
            if self.debug:
                print(f"{'\t'*(cur_depth + 1)}Move value: {move_value}")
            if move_value < value:
                value = move_value
                best_move = move
            if value <= alpha:
                if self.debug:
                    print(f"{'\t'*(cur_depth + 1)}Alpha cut")
                break
            beta = min(beta, value)
        return (value, best_move)


    def constant_time_evaluate(self):
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
        return self.count_relative_material() + (random.random() / 100)
    def ordered_legal_moves(self):
        turn_mult = -1 if self.board.turn else 1
        move_heap = []
        for move in self.board.legal_moves:
            self.board.push(move)
            heapq.heappush(move_heap, (self.constant_time_evaluate() * turn_mult, random.random(), move))
            self.board.pop()


        while move_heap:
            yield heapq.heappop(move_heap)[2]
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