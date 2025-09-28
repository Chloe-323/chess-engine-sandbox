import chess
import random
import heapq
import copy


class Engine:
    def __init__(self, board: chess.Board, depth: int = 5, beam_width: int = 12):
        self.board = board
        self.depth = depth
        self.debug = True
        self.beam_width = beam_width
        self.explored = {}
        self.skips = 0
    
    def pick_move(self) -> chess.Move:
        """
        Select the best move for the current position.
        Returns:
            The best move found, or None if no legal moves exist
        """
        if self.debug:
            print("=======")
            print(f"Picking move for {"White" if self.board.turn else "Black"}")
        (value, move) = self.alpha_beta_search()
        print(f"Explored: {len(self.explored)}, Skips: {self.skips}")
        if self.debug:
            print(f"Picked move {self.board.san(move)} with value {value}")

        return move

    def evaluate(self) -> float:
        """Return a score for the current board position. Checkmate for white is +inf, checkmate for black is -inf."""
        if self.board.is_game_over():
            return self.constant_time_evaluate()
        
        return self.alpha_beta_search()[0]

    def alpha_beta_search(self) -> tuple[float, chess.Move]:
        """
        Perform an alpha-beta search to find the best move.
        Returns:
            A tuple of (evaluation, best_move) for the current position
        """
        if self.board.turn:
            return self.best_move_for_white()
        else:
            return self.best_move_for_black()

    def best_move_for_white(self, cur_depth: int = 0, alpha: float = -float('inf'), beta: float = float('inf')) -> tuple[float, chess.Move]:
        """
        Find the best move for white using alpha-beta pruning.
        Args:
            cur_depth: Current depth in the search tree
            alpha: Best evaluation for white (maximizing player)
            beta: Best evaluation for black (minimizing player)
        Returns:
            A tuple of (evaluation, best_move) from white's perspective
        """
        if self.board.is_game_over():
            return (self.constant_time_evaluate(), None)
        value = -float('inf')
        best_move = None
        if self.debug:
            print(f"{'\t'*cur_depth}Looking at moves for White: Alpha: {alpha}, Beta: {beta}, depth: {cur_depth} {'Only looking at captures and checks' if cur_depth >= self.depth else ''}")
        move_iter = self.ordered_legal_moves() if cur_depth < self.depth else self.ordered_legal_quiescent_moves()
        for move in move_iter:
            if self.debug:
                print(f"{'\t'*(cur_depth + 1)}Looking at {self.board.san(move)}")
            self.board.push(move)
            if self.board.fen() in self.explored and self.explored[self.board.fen()][0] >= self.depth - cur_depth:
                move_value = self.explored[self.board.fen()][1]
                self.skips += 1
            else:
                move_value = self.best_move_for_black(cur_depth + 1, alpha, beta)[0]
                self.explored[self.board.fen()] = (self.depth - cur_depth, move_value)
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
        if value == -float('inf'):
            if self.debug:
                print(f"{'\t'*(cur_depth + 1)}No moves at depth {cur_depth}, returning constant time evaluation")
            return (self.constant_time_evaluate(), None)
        return (value, best_move)

    def best_move_for_black(self, cur_depth: int = 0, alpha: float = -float('inf'), beta: float = float('inf')) -> tuple[float, chess.Move]:
        """
        Find the best move for black using alpha-beta pruning.
        Args:
            cur_depth: Current depth in the search tree
            alpha: Best evaluation for white (maximizing player)
            beta: Best evaluation for black (minimizing player)
        Returns:
            A tuple of (evaluation, best_move) from white's perspective
        """
        if self.board.is_game_over():
            return (self.constant_time_evaluate(), None)
        value = float('inf')
        best_move = None
        if self.debug:
            print(f"{'\t'*cur_depth}Looking at moves for Black: Alpha: {alpha}, Beta: {beta}, depth: {cur_depth} {'Only looking at captures and checks' if cur_depth >= self.depth else ''}")
        move_iter = self.ordered_legal_moves() if cur_depth < self.depth else self.ordered_legal_quiescent_moves()
        for move in move_iter:
            if self.debug:
                print(f"{'\t'*(cur_depth + 1)}Looking at {self.board.san(move)}")
            self.board.push(move)
            if self.board.fen() in self.explored and self.explored[self.board.fen()][0] >= self.depth - cur_depth:
                move_value = self.explored[self.board.fen()][1]
                self.skips += 1
            else:
                move_value = self.best_move_for_white(cur_depth + 1, alpha, beta)[0]
                self.explored[self.board.fen()] = (self.depth - cur_depth, move_value)
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
        if value == float('inf'):
            if self.debug:
                print(f"{'\t'*(cur_depth + 1)}No moves at depth {cur_depth}, returning constant time evaluation")
            return (self.constant_time_evaluate(), None)
        return (value, best_move)

    def constant_time_evaluate(self) -> float:
        """
        Evaluate the current position without any search.
        Returns:
            float: Evaluation score from white's perspective.
                  Large positive/negative values for checkmate, 0 for draws.
                  For non-terminal positions, returns material advantage + small random noise.
        """
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
        if self.board.fen() in self.explored:
            return self.explored[self.board.fen()][1]
        return self.count_relative_material() + self.count_positional_control()

    # Helper functions
    def ordered_legal_moves(self):
        """
        Generate legal moves in order of evaluation.
        Returns:
            Generator of legal moves ordered by evaluation.
        """
        turn_mult = -1 if self.board.turn else 1
        move_heap = []
        pertinent_move_heap = []
        for move in self.board.legal_moves:
            target_heap = pertinent_move_heap if self.is_good_capture(move) or self.board.gives_check(move) else move_heap
            self.board.push(move)
            heapq.heappush(target_heap, (self.constant_time_evaluate() * turn_mult, random.random(), move))
            self.board.pop()

        while pertinent_move_heap:
            yield heapq.heappop(pertinent_move_heap)[2]
        normal_moves_checked = 0
        while move_heap:
            normal_moves_checked += 1
            yield heapq.heappop(move_heap)[2]
            if normal_moves_checked > self.beam_width:
                break
    def ordered_legal_quiescent_moves(self):
        turn_mult = -1 if self.board.turn else 1
        move_heap = []
        for move in self.board.legal_moves:
            if not self.is_good_capture(move) and not self.board.gives_check(move):
                continue
            self.board.push(move)
            heapq.heappush(move_heap, (self.constant_time_evaluate() * turn_mult, random.random(), move))
            self.board.pop()

        while move_heap:
            yield heapq.heappop(move_heap)[2]
    def is_good_capture(self, move: chess.Move) -> bool:
        #if it's not a capture, it can't be a good capture
        if not self.board.is_capture(move):
            return False
        
        attackers = [self.board.piece_at(square) for square in self.board.attackers(not self.board.turn, move.to_square)]
        defenders = [self.board.piece_at(square) for square in self.board.attackers(self.board.turn, move.to_square)]
        # if fewer attackers than defenders, probably not a good capture
        if (len(attackers) <= len(defenders)):
            return False
        # if the lowest value attacker is worth more than the lowest value defender, probably not a good capture
        if(min(self.get_piece_value(attacker) for attacker in attackers) > min(self.get_piece_value(defender) for defender in defenders)):
            return False
        # worth looking into
        return True
    def count_positional_control(self) -> float:
        """
        Calculate the positional control of the board.
        Returns:
            float: Positional control calculated based on the number of pieces that can see each square, and how close that square is to the center.
                Range: 
                Positive values favor white, negative favor black.
        """
        total = 0
        difference = 0
        for square in chess.SQUARES:
            closeness_to_center = self.calculate_square_closeness_to_center(square)
            difference += closeness_to_center * len(self.board.attackers(chess.WHITE, square))
            difference -= closeness_to_center * len(self.board.attackers(chess.BLACK, square))
            total += closeness_to_center * len(self.board.attackers(chess.WHITE, square))
            total += closeness_to_center * len(self.board.attackers(chess.BLACK, square))
        return (difference / total)
    def count_relative_material(self) -> float:
        """
        Calculate the relative material advantage as a percentage.
        Returns:
            float: Material advantage as a percentage of total material. 
                Range: [-100, 100]
                Positive values favor white, negative favor black.
        """
        difference = 0
        total_material = 0
        for square in chess.SQUARES:
            piece = self.board.piece_at(square)
            if piece is None:
                continue
            mult = 1 if piece.color == chess.WHITE else -1
            value = self.get_piece_value(piece)
            difference += value * mult
            total_material += value
        return (difference / total_material) * 100
    def count_material(self) -> int:
        """
        Calculate the total material on the board.
        Returns:
            int: Total material count (positive for white, negative for black)
            Piece values: Pawn=1, Knight/Bishop=3, Rook=5, Queen=9, King=0
        """
        material = 0
        for square in chess.SQUARES:
            piece = self.board.piece_at(square)
            if piece is None:
                continue
            mult = 1 if piece.color == chess.WHITE else -1
            value = self.get_piece_value(piece)
            material += value * mult
        return material
    def get_piece_value(self, piece: chess.Piece) -> int:
        """
        Get the value of a piece.
        Args:
            piece: The piece to get the value of
        Returns:
            int: The value of the piece
        """
        if piece is None:
            return 0
        if piece.piece_type == chess.PAWN:
            return 1
        elif piece.piece_type == chess.KNIGHT:
            return 3
        elif piece.piece_type == chess.BISHOP:
            return 3
        elif piece.piece_type == chess.ROOK:
            return 5
        elif piece.piece_type == chess.QUEEN:
            return 9
        elif piece.piece_type == chess.KING:
            return 0
        return 0
    def calculate_square_closeness_to_center(self, square: chess.Square) -> float:
        return (((3.5 - abs(chess.square_rank(square) - 3.5))) * ((3.5 - abs(chess.square_file(square) - 3.5))) / 36) + 0.75