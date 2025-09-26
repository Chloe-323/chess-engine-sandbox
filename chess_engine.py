import chess
import random

class Engine:
    def __init__(self, board):
        self.board = board
    
    def play(self, move: chess.Move):
        self.board.push(move)
    
    def pick_move(self, color, legal_moves):
        return random.choice(list(legal_moves))