import os
from datetime import datetime
from typing import Optional, TextIO

class GameLogger:
    def __init__(self, directory: str = "games"):
        """
        Initialize the game logger.
        
        Args:
            directory: Directory to save game logs
        """
        self.directory = directory
        self.file: Optional[TextIO] = None
        self.move_number = 1
        self.last_move_was_white = False
        
        # Create games directory if it doesn't exist
        os.makedirs(directory, exist_ok=True)
    
    def start_new_game(self) -> None:
        """Start a new game log with a timestamped filename."""
        if self.file and not self.file.closed:
            self.file.close()
            
        timestamp = datetime.now().strftime("%m_%d_%Y_%H_%M_%S_%f")[:-3]  # Truncate to milliseconds
        self.filename = os.path.join(self.directory, f"{timestamp}.txt")
        self.file = open(self.filename, 'w', encoding='utf-8')
        self.move_number = 1
        self.last_move_was_white = False
    
    def log_move(self, move_san: str, is_white: bool) -> None:
        """
        Log a move to the current game file.
        
        Args:
            move_san: Move in Standard Algebraic Notation
            is_white: Whether the move was made by white
        """
        if not self.file or self.file.closed:
            return
            
        if is_white:
            # White's move on a new line with move number
            self.file.write(f"{self.move_number}. {move_san} ")
            self.last_move_was_white = True
        else:
            if self.last_move_was_white:
                # Black's move on the same line
                self.file.write(f"{move_san}\n")
                self.move_number += 1
            else:
                # First move of the game by black (unusual but possible)
                self.file.write(f"{self.move_number}... {move_san}\n")
                self.move_number += 1
            self.last_move_was_white = False
        
        self.file.flush()
    
    def end_game(self) -> None:
        """Properly close the current game log file."""
        if self.file and not self.file.closed:
            # Add a newline at the end if the last move was white
            if self.last_move_was_white:
                self.file.write("\n")
            self.file.close()
    
    def __del__(self):
        """Ensure the file is closed when the logger is destroyed."""
        self.end_game()
