import pygame

# TODO: Implement check, checkmate, stalemate, promotion

# Constants
TILE_SIZE = 80
BOARD_SIZE = TILE_SIZE * 8
LIGHT = (240, 217, 181)
DARK = (181, 136, 99)
HIGHLIGHT_COLOR = (255, 255, 0)
HIGHLIGHT_ALPHA = 140

# Board setup
board = [['bR', 'bN', 'bB', 'bQ', 'bK', 'bB', 'bN', 'bR'],
         ['bP', 'bP', 'bP', 'bP', 'bP', 'bP', 'bP', 'bP'],
         ['..', '..', '..', '..', '..', '..', '..', '..'],
         ['..', '..', '..', '..', '..', '..', '..', '..'],
         ['..', '..', '..', '..', '..', '..', '..', '..'],
         ['..', '..', '..', '..', '..', '..', '..', '..'],
         ['wP', 'wP', 'wP', 'wP', 'wP', 'wP', 'wP', 'wP'],
         ['wR', 'wN', 'wB', 'wQ', 'wK', 'wB', 'wN', 'wR']]

# Functions
def highlight(selected_tile: tuple):
    rect = pygame.Rect(selected_tile[1] * TILE_SIZE, selected_tile[0] * TILE_SIZE, TILE_SIZE, TILE_SIZE)
    overlay = pygame.Surface((TILE_SIZE, TILE_SIZE), pygame.SRCALPHA)
    overlay.fill((*HIGHLIGHT_COLOR, HIGHLIGHT_ALPHA))
    screen.blit(overlay, rect.topleft)

def rmv_highlight(selected_tile: tuple):
    rect = pygame.Rect(selected_tile[1] * TILE_SIZE, selected_tile[0] * TILE_SIZE, TILE_SIZE, TILE_SIZE)
    color = LIGHT if (selected_tile[0] + selected_tile[1]) % 2 == 0 else DARK
    pygame.draw.rect(screen, color, rect)
    if board[selected_tile[0]][selected_tile[1]] != '..':
        screen.blit(piece_images[board[selected_tile[0]][selected_tile[1]]], rect.topleft)

def legal_move(from_tile: tuple, to_tile: tuple) -> bool:
    global castling, w_en_passant_legal, b_en_passant_legal, en_passant

    selected_piece = board[from_tile[0]][from_tile[1]][1]  # Get piece type without color
    if selected_piece == 'P':  # Pawn movement
        direction = 1 if board[from_tile[0]][from_tile[1]][0] == 'w' else -1
        start_row = 6 if direction == 1 else 1
        # Standard move
        if to_tile[1] == from_tile[1] and to_tile[0] == from_tile[0] - direction and board[to_tile[0]][to_tile[1]] == '..':
            return True
        # Double move from starting position
        if (from_tile[0] == start_row and to_tile[1] == from_tile[1] and 
            to_tile[0] == from_tile[0] - 2 * direction and 
            board[from_tile[0] - direction][from_tile[1]] == '..' and 
            board[to_tile[0]][to_tile[1]] == '..'):
            # Update en passant legality
            if direction == -1:
                w_en_passant_legal = to_tile[1]
            else:
                b_en_passant_legal = to_tile[1]
            return True
        # Captures
        if abs(to_tile[1] - from_tile[1]) == 1 and to_tile[0] == from_tile[0] - direction:
            if board[to_tile[0]][to_tile[1]] != '..' and board[to_tile[0]][to_tile[1]][0] != board[from_tile[0]][from_tile[1]][0]:
                return True
        # En passant
        if abs(to_tile[1] - from_tile[1]) == 1 and to_tile[0] == from_tile[0] - direction:
            if direction == 1 and to_tile[1] == w_en_passant_legal and to_tile[0] == 2:
                en_passant = True
                return True
            elif direction == -1 and to_tile[1] == b_en_passant_legal and to_tile[0] == 5:
                en_passant = True
                return True
        return False
    
    elif selected_piece == 'R':  # Rook movement
        if from_tile[0] == to_tile[0]:  # Moving in the same row
            step = 1 if to_tile[1] > from_tile[1] else -1
            for col in range(from_tile[1] + step, to_tile[1], step):
                if board[from_tile[0]][col] != '..':
                    return False
            return True
        elif from_tile[1] == to_tile[1]:  # Moving in the same column
            step = 1 if to_tile[0] > from_tile[0] else -1
            for row in range(from_tile[0] + step, to_tile[0], step):
                if board[row][from_tile[1]] != '..':
                    return False
            return True
        return False
    
    elif selected_piece == 'N':  # Knight movement
        row_diff = abs(from_tile[0] - to_tile[0])
        col_diff = abs(from_tile[1] - to_tile[1])
        return (row_diff == 2 and col_diff == 1) or (row_diff == 1 and col_diff == 2)
    
    elif selected_piece == 'B':  # Bishop movement
        row_diff = abs(from_tile[0] - to_tile[0])
        col_diff = abs(from_tile[1] - to_tile[1])
        if row_diff == col_diff:
            row_step = 1 if to_tile[0] > from_tile[0] else -1
            col_step = 1 if to_tile[1] > from_tile[1] else -1
            for step in range(1, row_diff):
                if board[from_tile[0] + step * row_step][from_tile[1] + step * col_step] != '..':
                    return False
            return True
        return False
    
    elif selected_piece == 'Q':  # Queen movement
        row_diff = abs(from_tile[0] - to_tile[0])
        col_diff = abs(from_tile[1] - to_tile[1])
        if from_tile[0] == to_tile[0]:  # Moving in the same row
            step = 1 if to_tile[1] > from_tile[1] else -1
            for col in range(from_tile[1] + step, to_tile[1], step):
                if board[from_tile[0]][col] != '..':
                    return False
            return True
        elif from_tile[1] == to_tile[1]:  # Moving in the same column
            step = 1 if to_tile[0] > from_tile[0] else -1
            for row in range(from_tile[0] + step, to_tile[0], step):
                if board[row][from_tile[1]] != '..':
                    return False
            return True
        elif row_diff == col_diff:  # Diagonal movement
            row_step = 1 if to_tile[0] > from_tile[0] else -1
            col_step = 1 if to_tile[1] > from_tile[1] else -1
            for step in range(1, row_diff):
                if board[from_tile[0] + step * row_step][from_tile[1] + step * col_step] != '..':
                    return False
            return True
        return False
    
    elif selected_piece == 'K':  # King movement
        row_diff = abs(from_tile[0] - to_tile[0])
        col_diff = abs(from_tile[1] - to_tile[1])
        if max(row_diff, col_diff) == 1:
            return True
        elif col_diff == 2 and row_diff == 0: #castling
            if board[from_tile[0]][from_tile[1]][0] == 'w' and from_tile[0] == 7:
                if to_tile[1] == 6 and w_short_castle_legal:
                    if board[7][5] == '..' and board[7][6] == '..':
                        castling = 'ws'
                        return True
                elif to_tile[1] == 2 and w_long_castle_legal:
                    if board[7][1] == '..' and board[7][2] == '..' and board[7][3] == '..':
                        castling = 'wl'
                        return True
            else:
                if to_tile[1] == 6 and b_short_castle_legal:
                    if board[0][5] == '..' and board[0][6] == '..':
                        castling = 'bs'
                        return True
                elif to_tile[1] == 2 and b_long_castle_legal:
                    if board[0][1] == '..' and board[0][2] == '..' and board[0][3] == '..':
                        castling = 'bl'
                        return True
        else:
            return False
                
    return False

def move_piece(from_tile: tuple, to_tile: tuple):
    global w_long_castle_legal, w_short_castle_legal, b_long_castle_legal, b_short_castle_legal, b_en_passant_legal, w_en_passant_legal, en_passant
    
    # Handle en passant capture
    if en_passant:
        if board[from_tile[0]][from_tile[1]][0] == 'w': #white pawn moved
            board[to_tile[0]+1][to_tile[1]] = '..' #remove black pawn
            rmv_highlight((to_tile[0]+1, to_tile[1]))
        else: #black pawn moved
            board[to_tile[0]-1][to_tile[1]] = '..' #remove white pawn
            rmv_highlight((to_tile[0]-1, to_tile[1]))
        en_passant = False

    # Update en passant rights
    if board[from_tile[0]][from_tile[1]][0] == 'b': #if black moved, reset black's en passant rights, else reset white
        b_en_passant_legal = 8 #8 out of bounds means not legal
    else:
        w_en_passant_legal = 8

    # Move rook if castling
    if castling:
        if castling == 'ws':
            board[7][5] = 'wR'
            board[7][7] = '..'
            rmv_highlight((7,7)) #remove rook
        elif castling == 'wl':
            board[7][3] = 'wR'
            board[7][0] = '..'
            rmv_highlight((7,0)) #remove rook
        elif castling == 'bs':
            board[0][5] = 'bR'
            board[0][7] = '..'
            rmv_highlight((0,7)) #remove rook
        elif castling == 'bl':
            board[0][3] = 'bR'
            board[0][0] = '..'
            rmv_highlight((0,0)) #remove rook

    # Update castling rights
    piece = board[from_tile[0]][from_tile[1]]
    if piece == 'wK':
        w_long_castle_legal = False
        w_short_castle_legal = False
    elif piece == 'bK':
        b_long_castle_legal = False
        b_short_castle_legal = False
    elif piece == 'wR':
        if from_tile == (7, 0):
            w_long_castle_legal = False
        elif from_tile == (7, 7):
            w_short_castle_legal = False
    elif piece == 'bR':
        if from_tile == (0, 0):
            b_long_castle_legal = False
        elif from_tile == (0, 7):
            b_short_castle_legal = False

    # Move
    board[to_tile[0]][to_tile[1]] = board[from_tile[0]][from_tile[1]]
    board[from_tile[0]][from_tile[1]] = '..'

#
w_long_castle_legal = True
w_short_castle_legal = True
b_long_castle_legal = True
b_short_castle_legal = True
castling = ''
w_en_passant_legal = 8 #8 out of bounds means not legal
b_en_passant_legal = 8
en_passant = False

# Initialize Pygame
pygame.init()

# Create window
screen = pygame.display.set_mode((BOARD_SIZE, BOARD_SIZE))
pygame.display.set_caption("Empty Chess Board")

# Load assets
assets = ['bR', 'bN', 'bB', 'bQ', 'bK', 'bP',
          'wR', 'wN', 'wB', 'wQ', 'wK', 'wP']

piece_images = {}
for piece in assets:
    piece_path = f"assets/{piece}.png"
    piece_image = pygame.image.load(piece_path).convert_alpha()
    piece_image = pygame.transform.smoothscale(piece_image, (TILE_SIZE, TILE_SIZE))
    piece_images[piece] = piece_image


# Draw chessboard
for row in range(8):
    for col in range(8):
        color = LIGHT if (row + col) % 2 == 0 else DARK
        pygame.draw.rect(screen, color, (col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE))

# Set pieces
def draw_board():
    for row in range(8):
        for col in range(8):
            if board[row][col] != '..':
                rect = pygame.Rect(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE)
                color = LIGHT if (row + col) % 2 == 0 else DARK
                pygame.draw.rect(screen, color, rect)
                screen.blit(piece_images[board[row][col]], rect.topleft)
    pygame.display.flip()
draw_board()

# Keep window open until closed
running = True
selected_tile = None
current_player = 'w'
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False

        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1:
                mx, my = event.pos
                col = mx // TILE_SIZE
                row = my // TILE_SIZE
                if (0<=row<8 and 0<=col<8) and (board[row][col]!='..') and (board[row][col][0]==current_player): #if selecting a piece
                    if selected_tile:
                        rmv_highlight(selected_tile) #old selected tile
                    selected_tile = (row, col)
                    highlight(selected_tile)
                    pygame.display.flip()
                elif selected_tile and (0 <= row < 8 and 0 <= col < 8): #if attempting to move selected piece
                    if legal_move(selected_tile, (row, col)):
                        move_piece(selected_tile, (row, col))
                        rmv_highlight(selected_tile)
                        selected_tile = None
                        current_player = 'b' if current_player == 'w' else 'w'
                        draw_board()
                else: #unselect if clicked outside or on empty tile or on opponent's piece
                    if selected_tile:
                        rmv_highlight(selected_tile)
                        selected_tile = None
                        pygame.display.flip()


pygame.quit()
