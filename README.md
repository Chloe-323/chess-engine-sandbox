# Chess Engine with Pygame GUI

A chess game featuring a Pygame-based graphical interface and a simple AI opponent. Built with Python and the `python-chess` library.

## Features
- Play against an AI opponent
- Simple but effective AI using minimax algorithm with material evaluation
- Game over detection with visual feedback (checkmate, stalemate, draw conditions)
- Legal move highlighting
- Move validation using the `python-chess` library
- Clean, responsive interface
- Board flipping (Ctrl+F to flip the board)
- Game state information in console

## Requirements
- Python 3
- Pygame
- python-chess

## Installation

1. Clone the repository:
```bash
git clone https://github.com/Chloe-323/chess-engine-sandbox.git
cd chess-engine-sandbox
```

2. Install the required packages:
```bash
pip install pygame python-chess
```

## Running the Game

To start the game with the Pygame GUI:
```bash
python chess_gui.py
```

By default, you play as White and the AI plays as Black. The AI will automatically make its move after you make yours.

## Controls
- **Left-click**: Select a piece and make a move
- **Right-click**: Deselect the current piece
- **Ctrl+F**: Flip the board
- **Close the window**: Exit the game

## Adding Piece Images
For the best experience, you can add chess piece images to an `images` folder in the project directory. The expected filenames are:
- `wp.png` - White pawn
- `wN.png` - White knight
- `wB.png` - White bishop
- `wR.png` - White rook
- `wQ.png` - White queen
- `wK.png` - White king
- `bp.png` - Black pawn
- `bN.png` - Black knight
- `bB.png` - Black bishop
- `bR.png` - Black rook
- `bQ.png` - Black queen
- `bK.png` - Black king

If images are not found, the game will use simple geometric shapes as placeholders.

## How to Play
1. Click on a piece to select it (valid moves will be highlighted)
2. Click on a highlighted square to move the selected piece
3. Pawns will automatically promote to queens when reaching the opposite side
4. When the game ends, click anywhere to start a new game

## AI Implementation

The chess engine implements an advanced AI using:
- Minimax algorithm with alpha-beta pruning for efficient search
- Quiescence search to avoid horizon effects
- Move ordering to improve pruning efficiency
- Depth-limited search with configurable depth (default is 2)

### Features
- **Alpha-Beta Pruning**: Significantly reduces the number of positions evaluated
- **Quiescence Search**: Extends search in volatile positions (captures and checks)
- **Move Ordering**: Examines promising moves first to improve pruning
- **Evaluation Function**:
  - Material evaluation (Pawn=1, Knight/Bishop=3, Rook=5, Queen=9)
  - Checkmate detection
  - Draw detection (stalemate, insufficient material, 50-move rule, threefold repetition)
  - Relative material advantage calculation

### Configuration
You can adjust the AI's behavior by modifying these parameters in `chess_engine.py`:
- `depth`: Search depth (default: 2)
- `debug`: Enable/disable debug output (default: False)

## Starting from Custom Positions

You can start the game from any position using FEN (Forsyth–Edwards Notation):

```bash
python chess_gui.py "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
```

If no FEN string is provided, the game starts from the standard initial position.

## Future Improvements
- Optimize move ordering
- Maintain an explored game tree to avoid redundant calculations
- Add iterative deepening with time control
- Implement more sophisticated evaluation features (piece-square tables, mobility, pawn structure)
- Implement opening book
- Add endgame tablebases

## Planned Approach
*I will be doing three different "versions" of the chess engine.* 
1. Version 1 only uses this textbook I found. I am not allowed to google anything or ask AI for help.
2. Version 2 will still use rule-based approaches, but with full access to the internet. I plan to use it to look up opening book moves, and to get some ideas for evaluation functions. I will also look into what other chess engines to, and try to implement some of their approaches.
3. Version 3 will use tensorflow to try to get a neural network to play chess.

## Game Logging

The game automatically logs all moves to timestamped files in the `games/` directory. Each game is saved in a separate file with the following format:

- **File Location**: `games/` directory in the project folder
- **Filename Format**: `MM_DD_YYYY_HH_MM_SS_SSS.txt` (e.g., `09_28_2025_14_06_09_999.txt`)
- **Log Format**: Standard Algebraic Notation (e.g., `1. e4 e5 2. Nf3 Nc6`)

### Features:
- Automatic creation of log files for each new game
- Proper handling of move numbering and turn order
- Clean separation of games into individual files
- Human-readable move notation

### Example Log File:
```
1. e4 e5
2. Nf3 Nc6
3. Bb5 a6
4. Ba4 Nf6
5. O-O Be7
6. Re1 b5
7. Bb3 d6
8. c3 O-O
```

### Notes:
- Logs are automatically created when you start a new game
- Each move is logged immediately after it's made
- The `games/` directory is automatically created if it doesn't exist
- Log files are plain text and can be opened with any text editor

## Game Over Conditions
The game detects several endgame conditions:
- Checkmate (with winner announcement)
- Stalemate
- Draw by insufficient material
- Draw by fifty-move rule
- Draw by threefold repetition

When the game ends, the board will dim and display the result. Click anywhere to start a new game.

## Credits
- Uses the `python-chess` library for move validation and game state management
- Pygame for the graphical interface
