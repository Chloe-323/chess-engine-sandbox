# Chess with Pygame GUI

A simple chess game with a Pygame-based graphical user interface.

## Features
- Play chess against yourself (human vs human)
- Legal move highlighting
- Move validation using the `python-chess` library
- Clean, responsive interface

## Requirements
- Python 3.6+
- Pygame 2.5.2
- python-chess 1.999

## Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/chess-engine-sandbox.git
cd chess-engine-sandbox
```

2. Install the required packages:
```bash
pip install -r requirements.txt
```

## Running the Game

To start the game with the Pygame GUI:
```bash
python chess_gui.py
```

## Controls
- **Left-click**: Select a piece and make a move
- **Right-click**: Deselect the current piece
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

## Credits
- Uses the `python-chess` library for move validation and game state management
- Pygame for the graphical interface
