import chess
import chess_engine

engine = chess_engine.Engine(chess.Board())

def calculate_square_closeness():
    for square in chess.SQUARES:
        print(chess.square_name(square), engine.calculate_square_closeness_to_center(square))
    
def constant_time_evaluate(fen: str):
    engine.board.set_fen(fen)
    print(engine.constant_time_evaluate())

constant_time_evaluate("rnbqkbnr/ppp1pppp/8/3p4/3P4/8/PPP1PPPP/RNBQKBNR w KQkq d6 0 1")