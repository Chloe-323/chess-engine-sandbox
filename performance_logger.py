import csv
import time
from datetime import datetime
from pathlib import Path
from typing import Optional, Dict, Any

class PerformanceLogger:
    def __init__(self, log_dir: str = "performance_logs"):
        """
        Initialize the performance logger.
        
        Args:
            log_dir: Directory where performance logs will be stored
        """
        # Create the log directory if it doesn't exist
        self.log_dir = Path(log_dir)
        self.log_dir.mkdir(parents=True, exist_ok=True)
        
        # Create a timestamped log file
        timestamp = datetime.now().strftime("%m_%d_%Y_%H_%M_%S_%f")[:-3]  # Truncate to milliseconds
        self.log_file = self.log_dir / f"performance_{timestamp}.csv"
        self.current_log: Dict[str, Any] = {}
        self.start_time: Optional[float] = None
        
        # Write the CSV header
        with open(self.log_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([
                'timestamp', 'depth', 'positions_evaluated', 'positions_skipped', 
                'time_taken_ms', 'move_san', 'color', 'branching_factor',
                'nodes_per_second', 'best_score', 'min_score', 'max_score',
                'alpha_prunes', 'beta_prunes', 'total_prunes'
            ])
        
        print(f"Performance log created at: {self.log_file}")
    
    def start_timer(self, move_san: str, depth: int, is_white: bool) -> None:
        """Start timing a move evaluation."""
        self.current_log = {
            'timestamp': datetime.now().isoformat(),
            'depth': depth,
            'move_san': move_san,
            'is_white': is_white,
            'positions_evaluated': 0,
            'positions_skipped': 0,
            'alpha_prunes': 0,
            'beta_prunes': 0,
            'min_score': float('inf'),
            'max_score': -float('inf')
        }
        self.start_time = time.time()
    
    def update_stats(self, positions_evaluated: int, positions_skipped: int,
                    alpha_prunes: int = 0, beta_prunes: int = 0,
                    score: Optional[float] = None) -> None:
        """Update the evaluation statistics.
        
        Args:
            positions_evaluated: Number of positions evaluated
            positions_skipped: Number of positions skipped (transposition table)
            alpha_prunes: Number of alpha cutoffs (incremental)
            beta_prunes: Number of beta cutoffs (incremental)
            score: Current score to update min/max range
        """
        if self.current_log is not None:
            self.current_log['positions_evaluated'] = self.current_log.get('positions_evaluated', 0) + positions_evaluated
            self.current_log['positions_skipped'] = self.current_log.get('positions_skipped', 0) + positions_skipped
            self.current_log['alpha_prunes'] = self.current_log.get('alpha_prunes', 0) + alpha_prunes
            self.current_log['beta_prunes'] = self.current_log.get('beta_prunes', 0) + beta_prunes
            
            if score is not None:
                self.current_log['min_score'] = min(self.current_log.get('min_score', float('inf')), score)
                self.current_log['max_score'] = max(self.current_log.get('max_score', -float('inf')), score)
    
    def log_move(self, move_san: str, depth: int, is_white: bool, 
                positions_evaluated: int, positions_skipped: int, 
                time_taken_ms: float, best_score: float, alpha_prunes: int, beta_prunes: int) -> None:
        """
        Log a completed move evaluation.
        
        Args:
            move_san: Move in Standard Algebraic Notation
            depth: Search depth
            is_white: Whether the move was made by white
            positions_evaluated: Number of positions evaluated
            positions_skipped: Number of positions skipped due to transposition table
            time_taken_ms: Time taken to evaluate the move in milliseconds
        """
        # Calculate branching factor if we have enough data
        branching_factor = 0
        if positions_evaluated > 1 and depth > 0:
            branching_factor = positions_evaluated ** (1.0 / depth)
        
        # Calculate nodes per second and other metrics
        nodes_per_second = 0
        if time_taken_ms > 0:
            nodes_per_second = (positions_evaluated / time_taken_ms) * 1000
            
        # Get pruning stats
        total_prunes = alpha_prunes + beta_prunes
        min_score = self.current_log.get('min_score', best_score)
        max_score = self.current_log.get('max_score', best_score)
        
        # Write to CSV
        with open(self.log_file, 'a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([
                datetime.now().isoformat(),
                depth,
                positions_evaluated,
                positions_skipped,
                f"{time_taken_ms:.2f}",
                move_san,
                'white' if is_white else 'black',
                f"{branching_factor:.2f}",
                f"{nodes_per_second:.2f}",
                f"{best_score:.2f}",
                f"{min_score:.2f}",
                f"{max_score:.2f}",
                alpha_prunes,
                beta_prunes,
                total_prunes
            ])
        
        # Reset the current log
        self.current_log = {}
        self.start_time = None
