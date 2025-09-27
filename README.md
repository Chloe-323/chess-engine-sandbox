# Chess Engine with Pygame GUI

A chess game featuring a Pygame-based graphical interface and a simple AI opponent. Built with Python and the `python-chess` library.

## Features
- Play against an AI opponent or yourself (human vs human)
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
git clone https://github.com/yourusername/chess-engine-sandbox.git
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

The current AI implementation uses a simple approach that:
1. Explores all possible moves up to a specified depth (default is 3)
2. Evaluates positions based on relative material balance
3. Considers basic game end conditions (checkmate, stalemate, draws)

### Evaluation Function
- Compares material values of pieces (Pawn=1, Knight/Bishop=3, Rook=5, Queen=9)
- Detects immediate wins/losses (checkmate)
- Handles draw conditions (stalemate, insufficient material, 50-move rule, threefold repetition)

You can adjust the AI's search depth in `chess_gui.py` by modifying the `depth` parameter when initializing the `Engine` class (default is 3). Higher depths will make the AI stronger but slower.

### Future Improvements
- Implement minimax algorithm with alpha-beta pruning
- Add position evaluation beyond just material
- Include piece-square tables for better piece placement
- Add opening book support

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
