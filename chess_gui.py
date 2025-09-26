import pygame
import sys
import os
import io
import chess
import chess.svg
from chess_engine import Engine

# Initialize pygame
pygame.init()

# Constants
WINDOW_SIZE = 800
SQUARE_SIZE = WINDOW_SIZE // 8
FPS = 60
# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
LIGHT_SQUARE = (240, 217, 181)
DARK_SQUARE = (181, 136, 99)
SELECTED_HIGHLIGHT = (255, 255, 0, 128) #yellow highlight for selected piece
MOVE_HIGHLIGHT = (255, 255, 0, 128)  # yellow highlight for last move
LEGAL_MOVE_HIGHLIGHT = (144, 238, 144, 128)
PIECE_IMAGES = {}
PIECE_TYPES = ['p', 'n', 'b', 'r', 'q', 'k', 'P', 'N', 'B', 'R', 'Q', 'K']

def load_images():
    """Load SVG images for the chess pieces."""
    piece_types = ['P', 'N', 'B', 'R', 'Q', 'K']
    for color in ['w', 'b']:
        for piece_type in piece_types:
            piece_key = f"{color}{piece_type}"
            png_path = f'images/{piece_key}.png'
            try:
                if os.path.exists(png_path):
                    image = pygame.image.load(png_path)
                    PIECE_IMAGES[piece_key] = pygame.transform.scale(image, (SQUARE_SIZE, SQUARE_SIZE))
                    print(f"Successfully loaded PNG for {piece_key}")
                    continue
            except Exception as e:
                print(f"Warning: Could not load PNG for {piece_key}: {str(e)}")
            
            # Create a placeholder surface if no image is found
            print(f"Warning: No image found for {piece_key}, using placeholder")
            surf = pygame.Surface((SQUARE_SIZE, SQUARE_SIZE), pygame.SRCALPHA)
            color_val = (200, 200, 200, 100) if color == 'w' else (50, 50, 50, 100)
            pygame.draw.circle(surf, color_val, (SQUARE_SIZE // 2, SQUARE_SIZE // 2), SQUARE_SIZE // 4)
            PIECE_IMAGES[piece_key] = surf

def draw_board(screen):
    """Draw the chess board."""
    for row in range(8):
        for col in range(8):
            color = LIGHT_SQUARE if (row + col) % 2 == 0 else DARK_SQUARE
            pygame.draw.rect(screen, color, 
                           (col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))

def draw_pieces(screen, board, flipped=False):
    """Draw the chess pieces on the board, accounting for board flip."""
    for square in chess.SQUARES:
        piece = board.piece_at(square)
        if piece:
            file = chess.square_file(square)
            rank = chess.square_rank(square)
            
            if flipped:
                file = 7 - file
                rank = 7 - rank
            
            x = file * SQUARE_SIZE
            y = (7 - rank) * SQUARE_SIZE  # Invert y-coordinate for display
            
            # Create the image key: 'w' or 'b' followed by the uppercase piece symbol
            color_prefix = 'w' if piece.color == chess.WHITE else 'b'
            piece_char = piece.symbol().upper()
            image_key = f"{color_prefix}{piece_char}"
            
            if image_key in PIECE_IMAGES:
                screen.blit(PIECE_IMAGES[image_key], (x, y))
            else:
                # Fallback: Draw a circle if image not found
                center = (x + SQUARE_SIZE // 2, y + SQUARE_SIZE // 2)
                color = WHITE if piece.color == chess.WHITE else BLACK
                pygame.draw.circle(screen, color, center, SQUARE_SIZE // 3)
                print(f"Could not find image for {image_key}")

def draw_highlight(surface, square, highlight_type='move', flipped=False):
    """
    Highlight a square on the board.
    
    Args:
        surface: The surface to draw on
        square: The square to highlight (0-63)
        highlight_type: Type of highlight ('move' or 'selected')
        flipped: Whether the board is flipped
    """
    if square is None:
        return
    
    # Get file and rank
    file = chess.square_file(square)
    rank = chess.square_rank(square)
    
    if flipped:
        # Flip the coordinates for display
        file = 7 - file
        rank = 7 - rank
    
    # Calculate screen coordinates
    # Note: In Pygame, y=0 is at the top of the screen
    x = file * SQUARE_SIZE
    y = (7 - rank) * SQUARE_SIZE  # Invert y-coordinate for display
    
    # Create a surface with per-pixel alpha
    highlight = pygame.Surface((SQUARE_SIZE, SQUARE_SIZE), pygame.SRCALPHA)
    
    # Choose highlight color based on type
    if highlight_type == 'move':
        color = MOVE_HIGHLIGHT
    elif highlight_type == 'legal':
        color = LEGAL_MOVE_HIGHLIGHT
    else:  # 'selected' or default
        color = SELECTED_HIGHLIGHT
    
    pygame.draw.rect(highlight, color, (0, 0, SQUARE_SIZE, SQUARE_SIZE))
    surface.blit(highlight, (x, y))

def get_square_from_pos(pos, flipped=False):
    """Convert mouse position to chess square, accounting for board flip."""
    x, y = pos
    if x < 0 or x >= WINDOW_SIZE or y < 0 or y >= WINDOW_SIZE:
        return None
    
    file = x // SQUARE_SIZE
    rank = 7 - (y // SQUARE_SIZE)  # Convert y-coordinate to rank (0-7)
    
    if flipped:
        file = 7 - file
        rank = 7 - rank
    return chess.square(file, rank)

def main():
    # Set up the display
    screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE))
    pygame.display.set_caption('Chess (Ctrl+F to flip board)')
    clock = pygame.time.Clock()
    
    # Load piece images
    load_images()
    
    # Initialize the chess board and game state
    board = chess.Board()
    selected_square = None
    valid_moves = []
    last_move = None  # Track the last move made
    board_flipped = False  # Track if board is flipped
    engine = Engine(board)
    running = True
    
    def is_ai_turn():
        """Check if it's the AI's turn to move."""
        # If board is not flipped, AI plays black. If flipped, AI plays white.
        return (not board_flipped and not board.turn) or (board_flipped and board.turn)

    while running:
        # Check if it's AI's turn
        if is_ai_turn() and not board.is_game_over():
            # Get AI move
            legal_moves = list(board.legal_moves)
            if legal_moves:  # Ensure there are legal moves
                ai_move = engine.pick_move(board.turn, legal_moves)
                if ai_move in legal_moves:  # Double check the move is legal
                    board.push(ai_move)
                    last_move = (ai_move.from_square, ai_move.to_square)
        
        # Handle events
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_f and pygame.key.get_mods() & pygame.KMOD_CTRL:
                    # Toggle board flip when Ctrl+F is pressed
                    board_flipped = not board_flipped
            
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:  # Left mouse button
                    square = get_square_from_pos(event.pos, board_flipped)
                    
                    if square is not None:
                        # If a square is already selected
                        if selected_square is not None:
                            # If clicking on the same square, deselect
                            if square == selected_square:
                                selected_square = None
                                valid_moves = []
                            else:
                                piece = board.piece_at(selected_square)
                                # Try to make a move
                                move = chess.Move(selected_square, square)
                                
                                # Check if it's a promotion
                                if (chess.square_rank(square) in [0, 7] and 
                                    piece and piece.piece_type == chess.PAWN):
                                    # For now, default to queen promotion
                                    move = chess.Move(selected_square, square, promotion=chess.QUEEN)
                                
                                if move in board.legal_moves:
                                    board.push(move)
                                    last_move = (selected_square, square)  # Update last move

                                selected_square = None
                                valid_moves = []
                        else:
                            # Select a piece
                            piece = board.piece_at(square)
                            if piece and piece.color == board.turn:
                                selected_square = square
                                valid_moves = [move.to_square for move in board.legal_moves 
                                            if move.from_square == square]
                
                elif event.button == 3:  # Right mouse button
                    selected_square = None
                    valid_moves = []
        
        # Draw everything
        screen.fill(WHITE)
        draw_board(screen)
        
        # Draw highlights for valid moves
        for move_square in valid_moves:
            draw_highlight(screen, move_square, 'legal', board_flipped)
        
        # Draw highlight for selected piece
        if selected_square is not None:
            draw_highlight(screen, selected_square, 'selected', board_flipped)
            
        # Draw last move highlights
        if last_move:
            from_square, to_square = last_move
            draw_highlight(screen, from_square, 'move', board_flipped)
            draw_highlight(screen, to_square, 'move', board_flipped)
        
        # Draw pieces
        draw_pieces(screen, board, board_flipped)
        
        # Update the display
        pygame.display.flip()
        clock.tick(FPS)
    
    pygame.quit()

if __name__ == "__main__":
    main()
