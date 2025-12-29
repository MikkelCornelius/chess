import pygame
import ctypes
import os
import pathlib
import time

# TODO: Implement check, checkmate, stalemate, promotion, bugfix undo button for en passant and promotion, make castle illigal when squares are threatned
# TODO: Bot endgame depth, bot openings, bot message formatting, bot castling, bot en passant

# GUI Constants
TILE_SIZE = 80
BOARD_SIZE = TILE_SIZE * 8
UI_HEIGHT = 100
BOTTOM_MENU_HEIGHT = 40
LIGHT = (240, 217, 181)
DARK = (181, 136, 99)
HIGHLIGHT_COLOR = (255, 255, 0)
HIGHLIGHT_ALPHA = 140
WHITE = (255, 255, 255)
GREY = (200, 200, 200)
BLACK = (0, 0, 0)

# Board setup
board = [['bR', 'bN', 'bB', 'bQ', 'bK', 'bB', 'bN', 'bR'],
         ['bP', 'bP', 'bP', 'bP', 'bP', 'bP', 'bP', 'bP'],
         ['..', '..', '..', '..', '..', '..', '..', '..'],
         ['..', '..', '..', '..', '..', '..', '..', '..'],
         ['..', '..', '..', '..', '..', '..', '..', '..'],
         ['..', '..', '..', '..', '..', '..', '..', '..'],
         ['wP', 'wP', 'wP', 'wP', 'wP', 'wP', 'wP', 'wP'],
         ['wR', 'wN', 'wB', 'wQ', 'wK', 'wB', 'wN', 'wR']]
selected_tile = None
past_move = None
history = []
board_encoder = {'bR': 'r', 'bN': 'n', 'bB': 'b', 'bQ': 'q', 'bK': 'k', 'bP': 'p',
                 'wR': 'R', 'wN': 'N', 'wB': 'B', 'wQ': 'Q', 'wK': 'K', 'wP': 'P',
                 '..': ' '}
board_encoded = ""
w_captured_pieces = {'bP': 0,
                     'bN': 0,
                     'bB': 0,
                     'bR': 0,
                     'bQ': 0,
                     'bK': 0}
b_captured_pieces = {'wP': 0,
                     'wN': 0,
                     'wB': 0,
                     'wR': 0,
                     'wQ': 0,
                     'wK': 0}

# Board coordinates
col_indices = {'a': 0, 'b': 1, 'c': 2, 'd': 3, 'e': 4, 'f': 5, 'g': 6, 'h': 7}
row_indices = {'1': 7, '2': 6, '3': 5, '4': 4, '5': 3, '6': 2, '7': 1, '8': 0}
row_labels = ['8', '7', '6', '5', '4', '3', '2', '1']
col_labels = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']

# Functions
def encode_board() -> str:
    encoded = ""
    for row in board:
        for piece in row:
            encoded += board_encoder[piece]
    rights = ["T" if x else "F" for x in [w_long_castle_legal, w_short_castle_legal, b_long_castle_legal,b_short_castle_legal]]
    return encoded+current_player+rights+str(en_passant_legal)

def highlight(selected_tile: tuple):
    if player=='w':
        rect = pygame.Rect(selected_tile[1] * TILE_SIZE, selected_tile[0] * TILE_SIZE + UI_HEIGHT, TILE_SIZE, TILE_SIZE)
    else:
        rect = pygame.Rect((7-selected_tile[1]) * TILE_SIZE, (7-selected_tile[0]) * TILE_SIZE + UI_HEIGHT, TILE_SIZE, TILE_SIZE)
    overlay = pygame.Surface((TILE_SIZE, TILE_SIZE), pygame.SRCALPHA)
    overlay.fill((*HIGHLIGHT_COLOR, HIGHLIGHT_ALPHA))
    screen.blit(overlay, rect.topleft)

def legal_move(from_tile: tuple, to_tile: tuple) -> bool:

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
            return True
        # Captures
        if abs(to_tile[1] - from_tile[1]) == 1 and to_tile[0] == from_tile[0] - direction:
            if board[to_tile[0]][to_tile[1]] != '..' and board[to_tile[0]][to_tile[1]][0] != board[from_tile[0]][from_tile[1]][0]:
                return True
        # En passant
        if abs(to_tile[1] - from_tile[1]) == 1 and to_tile[0] == from_tile[0] - direction:
            if direction == 1 and to_tile[1] == en_passant_legal and to_tile[0] == 2:
                return True
            elif direction == -1 and to_tile[1] == en_passant_legal and to_tile[0] == 5:
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
                        return True
                elif to_tile[1] == 2 and w_long_castle_legal:
                    if board[7][1] == '..' and board[7][2] == '..' and board[7][3] == '..':
                        return True
            else:
                if to_tile[1] == 6 and b_short_castle_legal:
                    if board[0][5] == '..' and board[0][6] == '..':
                        return True
                elif to_tile[1] == 2 and b_long_castle_legal:
                    if board[0][1] == '..' and board[0][2] == '..' and board[0][3] == '..':
                        return True
        else:
            return False
                
    return False

def move_piece(from_tile: tuple, to_tile: tuple):
    global past_move, w_long_castle_legal, w_short_castle_legal, b_long_castle_legal, b_short_castle_legal, en_passant_legal
    piece = board[from_tile[0]][from_tile[1]]
    past_move = (from_tile, to_tile)

    # Add captured piece
    captured_piece = board[to_tile[0]][to_tile[1]]
    if captured_piece != '..':
        if piece[0]=='w': #if white is capturing a piece, add that piece to list of captured pieces, else add it to blacks list
            w_captured_pieces[captured_piece] += 1
        else:
            b_captured_pieces[captured_piece] += 1

    # Handle pawn promotion
    if piece == 'wP':
        if to_tile[0] == 0:
            board[from_tile[0]][from_tile[1]] = 'wQ'  # Promote to queen
            b_captured_pieces['wQ'] -= 1
    elif piece == 'bP':
        if to_tile[0] == 7:
            board[from_tile[0]][from_tile[1]] = 'bQ'  # Promote to queen
            w_captured_pieces['bQ'] -= 1

    # Handle en passant capture
    #if pawn moves to an empty square, remove piece behind pawn (if any)
    w_en_passant = False
    b_en_passant = False
    if piece=='wP' and board[to_tile[0]][to_tile[1]]=="..":
        w_en_passant = True
    if piece=='bP' and board[to_tile[0]][to_tile[1]]=="..":
        b_en_passant = True
    #has to be done at the bottom, after "# Move"
    
    # Update en passant rights
        # Set new en passant
    if piece[1] == 'P' and abs(from_tile[0]-to_tile[0])>1: #if en passant
        en_passant_legal = to_tile[1]
    else:
        en_passant_legal = 8

    # Move rook if castling
    if piece[1]=='K' and from_tile[1]-to_tile[1]==2: #short castle
        board[from_tile[0]][to_tile[1]-1] = board[from_tile[0]][0]
        board[from_tile[0]][0] = ".."
    if piece[1]=='K' and from_tile[1]-to_tile[1]==-2: #long castle
        board[from_tile[0]][to_tile[1]-1] = board[from_tile[0]][7]
        board[from_tile[0]][7] = ".."

    # Update castling rights
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

    # Save to history
    history.append((from_tile, to_tile, board[to_tile[0]][to_tile[1]]))

    # Move
    board[to_tile[0]][to_tile[1]] = board[from_tile[0]][from_tile[1]]
    board[from_tile[0]][from_tile[1]] = '..'

    # Finally en passant
    if w_en_passant:
        board[to_tile[0]+1][to_tile[1]] = '..' #remove black pawn
    if b_en_passant:
        board[to_tile[0]-1][to_tile[1]] = '..' #remove white pawn

#
w_long_castle_legal = True
w_short_castle_legal = True
b_long_castle_legal = True
b_short_castle_legal = True
en_passant_legal = 8 #8 out of bounds means not legal

# This is vib coded shit. Had trouble with dependencies. For some reason it works now
# Load bot with fallback: if the DLL (or its dependencies) aren't found,
# try adding common MSYS2 runtime directories to the process DLL search path
def _load_bot_dll():
    dll_rel = os.path.join(os.path.dirname(__file__), 'chessbot.dll')
    # prefer the explicit full path
    dll_path = os.path.abspath(dll_rel)
    try:
        return ctypes.CDLL(dll_path)
    except (FileNotFoundError, OSError):
        # candidate MSYS2/UCRT directories to try
        candidates = [r"C:\msys64\ucrt64\bin", r"C:\msys64\mingw64\bin", r"C:\msys64\usr\bin"]
        for d in candidates:
            if not os.path.isdir(d):
                continue
            try:
                handle = os.add_dll_directory(d)
            except Exception:
                handle = None
            try:
                return ctypes.CDLL(dll_path)
            except (FileNotFoundError, OSError):
                # remove the directory we added and continue
                if handle is not None:
                    try:
                        os.remove_dll_directory(handle)
                    except Exception:
                        pass
                continue
        # As a last resort, try loading any chessbot.dll directly from those dirs
        for d in candidates:
            p = os.path.join(d, 'chessbot.dll')
            if os.path.exists(p):
                try:
                    return ctypes.CDLL(os.path.abspath(p))
                except Exception:
                    pass
        # re-raise a clear error
        raise FileNotFoundError(f"Could not load chessbot.dll. Tried {dll_path} and MSYS2 bins.")

bot = _load_bot_dll()
bot.get_move.restype = ctypes.c_char_p
bot.get_move.argtypes = [ctypes.c_char_p]

def get_bot_move():
    global current_player
    print("\nBot is thinking..")
    time_point = time.time()
    bot_response = bot.get_move(encode_board().encode()).decode('utf-8')
    think_time = time.time() - time_point
    print("Bot thought for:", round(think_time*1000), "ms")
    bot_move = bot_response[:4]
    continuation = bot_response[6:-6]
    evaluation = bot_response[-6:]
    print("Bot move:", bot_move)
    from_pos = (row_indices[bot_move[1]], col_indices[bot_move[0]])
    to_pos = (row_indices[bot_move[3]], col_indices[bot_move[2]])
    move_piece(from_pos, to_pos)
    current_player = 'b' if current_player == 'w' else 'w'
    #print("white to move" if current_player=='w' else "black to move")
    #print("Best continuation:", continuation, "Eval:", evaluation[0]+str(float(evaluation[1:])), end='\n\n')

# Initialize Pygame
pygame.init()

# Create window
screen = pygame.display.set_mode((BOARD_SIZE, BOARD_SIZE+2*UI_HEIGHT+BOTTOM_MENU_HEIGHT))
pygame.display.set_caption("Chess")
screen.fill(WHITE) #set white background

# Load assets
assets = ['bR', 'bN', 'bB', 'bQ', 'bK', 'bP',
          'wR', 'wN', 'wB', 'wQ', 'wK', 'wP']

piece_images = {}
for piece in assets:
    piece_path = f"assets/{piece}.png"
    piece_image = pygame.image.load(piece_path).convert_alpha()
    piece_image = pygame.transform.smoothscale(piece_image, (TILE_SIZE, TILE_SIZE))
    piece_images[piece] = piece_image

# Set pieces
def draw_board():
    for row in range(8):
        for col in range(8):
            rect = pygame.Rect(col * TILE_SIZE, row * TILE_SIZE + UI_HEIGHT, TILE_SIZE, TILE_SIZE)
            color = LIGHT if (row + col) % 2 == 0 else DARK
            pygame.draw.rect(screen, color, rect)
            if player == 'b':
                row = 7-row
                col = 7-col
            if board[row][col] != '..':
                screen.blit(piece_images[board[row][col]], rect.topleft)
    
    # Draw row & col labels
    font = pygame.font.SysFont(None, 25)
    if player=='w':
        for row in range(8):
            font_color = DARK if row%2==0 else LIGHT
            text = font.render(row_labels[row], True, font_color)
            screen.blit(text, (5, 5+UI_HEIGHT+row*TILE_SIZE))
        for col in range(8):
            font_color = LIGHT if col%2==0 else DARK
            text = font.render(col_labels[col], True, font_color)
            screen.blit(text, (5+col*TILE_SIZE, UI_HEIGHT+BOARD_SIZE-20))
    else:
        for row in range(8):
            font_color = DARK if row%2==0 else LIGHT
            text = font.render(row_labels[7-row], True, font_color)
            screen.blit(text, (5, 5+UI_HEIGHT+row*TILE_SIZE))
        for col in range(8):
            font_color = LIGHT if col%2==0 else DARK
            text = font.render(col_labels[7-col], True, font_color)
            screen.blit(text, (5+col*TILE_SIZE, UI_HEIGHT+BOARD_SIZE-20))

    # Highlight squares
    if selected_tile:
        highlight(selected_tile)
    if past_move:
        highlight(past_move[0])
        highlight(past_move[1])
    
    # Add captured pieces
    #black
    pygame.draw.rect(screen, WHITE, (0, 0, BOARD_SIZE, UI_HEIGHT)) #clear
    offset = 0
    for key in b_captured_pieces:
        if b_captured_pieces[key]>0:
            for _ in range(max(b_captured_pieces[key],0)):
                screen.blit(piece_images[key], (offset, 0 if player=='w' else UI_HEIGHT+BOARD_SIZE))
                offset += 20
            offset += 50
    
    #white
    pygame.draw.rect(screen, WHITE, (0, UI_HEIGHT+BOARD_SIZE, BOARD_SIZE, UI_HEIGHT)) #clear
    offset = 0
    for key in w_captured_pieces:
        if w_captured_pieces[key]>0:
            for _ in range(max(w_captured_pieces[key],0)):
                screen.blit(piece_images[key], (offset, UI_HEIGHT+BOARD_SIZE if player=='w' else 0))
                offset += 20
            offset += 50

    # Set changes
    pygame.display.flip()

def make_textbox(x, y, width, height, input_text):
    button_rect = pygame.Rect(x, y, width, height)
    font = pygame.font.SysFont(None, 36)
    text = font.render(input_text, True, (0,0,0))
    text_rect = text.get_rect(center=button_rect.center)
    pygame.draw.rect(screen, GREY, button_rect)
    screen.blit(text, text_rect)
    return button_rect

# Draw chessboard
for row in range(8):
    for col in range(8):
        color = LIGHT if (row + col) % 2 == 0 else DARK
        pygame.draw.rect(screen, color, (col * TILE_SIZE, row * TILE_SIZE + UI_HEIGHT, TILE_SIZE, TILE_SIZE))
undo_button_rect = make_textbox(10, BOARD_SIZE+2*UI_HEIGHT, 100, 30, "undo")

# Start main loop
running = True
player = 'w'
draw_board()

# main menu
start_button1_rect = make_textbox(4*TILE_SIZE-150, 300, 300, 50, "Single player")
start_button2_rect = make_textbox(4*TILE_SIZE-150, 400, 300, 50, "Multiplayer")
start_button3_rect = make_textbox(4*TILE_SIZE-150, 500, 300, 50, "Replay")
pygame.display.flip()
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            exit()
        
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1:
                if start_button1_rect.collidepoint(event.pos):
                    singleplayer = True
                    replay = False
                    running = False
                if start_button2_rect.collidepoint(event.pos):
                    singleplayer = False
                    replay = False
                    running = False
                if start_button3_rect.collidepoint(event.pos):
                    singleplayer = False
                    replay = True
                    running = False

# Select player
if singleplayer:
    draw_board()
    w_player_button_rect = make_textbox(4*TILE_SIZE-150, 300, 300, 50, "Play as white")
    b_player_button_rect = make_textbox(4*TILE_SIZE-150, 400, 300, 50, "Play as black")
    r_player_button_rect = make_textbox(4*TILE_SIZE-150, 500, 300, 50, "Random")
    pygame.display.flip()
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                exit()
            
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:
                    if w_player_button_rect.collidepoint(event.pos):
                        player = 'w'
                        running = False
                    if b_player_button_rect.collidepoint(event.pos):
                        player = 'b'
                        running = False
                    if r_player_button_rect.collidepoint(event.pos):
                        #simple random number generator
                        mx, my = event.pos
                        player = ['w','b'][(mx*my)%2]
                        running = False

# Select game to replay
elif replay:
    save_games = [p.name for p in pathlib.Path("past games").iterdir()]
    save_games.sort()
    replay_menu_buttons = []
    draw_board()
    for i in range(len(save_games)):
        replay_menu_buttons.append(make_textbox(4*TILE_SIZE-150, 200+i*100, 300, 50, save_games[i]))
    pygame.display.flip()
    running = True

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                exit()
            
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:
                    for game in range(len(save_games)):
                        if replay_menu_buttons[game].collidepoint(event.pos):
                            replay_game = save_games[game]
                            player = 'w'
                            running = False

# Always play from white perspective when multiplayer
else:
    player = 'w'

# If replaying, play the replay loop and terminate the program
if replay:
    with open(f"past games/{replay_game}", "r") as file:
        line = file.readline().strip()
        moves = [((row_indices[move[1]], col_indices[move[0]]), (row_indices[move[3]], col_indices[move[2]])) for move in line.split("->")]
    move = 0
    
    next_mv_button_rect = make_textbox(125, BOARD_SIZE+2*UI_HEIGHT, 100, 30, "next")
    running = True
    draw_board()
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                exit()
            
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:
                    if next_mv_button_rect.collidepoint(event.pos):
                        move_piece(*(moves[move]))
                        move += 1 #next move
                        draw_board()
                    if undo_button_rect.collidepoint(event.pos):
                        if history:
                            from_tile = history[-1][0]
                            to_tile = history[-1][1]
                            captured = history[-1][2]
                            board[from_tile[0]][from_tile[1]] = board[to_tile[0]][to_tile[1]]
                            board[to_tile[0]][to_tile[1]] = captured
                            history.pop()

                            if history: #update last move
                                past_move = history[-1]
                            else:
                                past_move = None
                            move -= 1
                            draw_board()
    exit()


# Start game
draw_board()
running = True
current_player = 'w'
if player=='b':
    get_bot_move()
    draw_board()
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False
            elif event.key == pygame.K_SPACE:
                get_bot_move()
                draw_board()

        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1:
                mx, my = event.pos
                col = mx // TILE_SIZE
                row = (my-UI_HEIGHT) // TILE_SIZE
                if player=='b':
                    col = 7-col
                    row= 7-row
                if (0<=row<8 and 0<=col<8) and (board[row][col]!='..') and (board[row][col][0]==current_player): #if selecting a piece
                    selected_tile = (row, col)
                    draw_board()
                    #highlight(selected_tile)
                    pygame.display.flip()
                elif selected_tile and (0 <= row < 8 and 0 <= col < 8): #if attempting to move selected piece
                    if legal_move(selected_tile, (row, col)):
                        move_piece(selected_tile, (row, col))
                        selected_tile = None
                        current_player = 'b' if current_player == 'w' else 'w'
                        draw_board()
                        if singleplayer:
                            get_bot_move()
                            draw_board()
                else: #unselect if clicked outside or on empty tile or on opponent's piece
                    if selected_tile:
                        selected_tile = None
                        pygame.display.flip()
                if undo_button_rect.collidepoint(event.pos):
                    if history:
                        if singleplayer: #undo twice when single player
                            from_tile = history[-1][0]
                            to_tile = history[-1][1]
                            captured = history[-1][2]
                            board[from_tile[0]][from_tile[1]] = board[to_tile[0]][to_tile[1]]
                            board[to_tile[0]][to_tile[1]] = captured
                            history.pop()
                        from_tile = history[-1][0]
                        to_tile = history[-1][1]
                        captured = history[-1][2]
                        board[from_tile[0]][from_tile[1]] = board[to_tile[0]][to_tile[1]]
                        board[to_tile[0]][to_tile[1]] = captured
                        history.pop()

                        if history: #update last move
                            past_move = history[-1]
                        else:
                            past_move = None
                        draw_board()


pygame.quit()

# Save game
count = 0
while os.path.exists("past games/untitled game"+str(count)):
    count += 1

with open("past games/untitled game"+str(count), "w") as file:
    for move in history[:-1]:
        from_tile = move[0]
        to_tile = move[1]
        file.write(col_labels[from_tile[1]]+row_labels[from_tile[0]]+col_labels[to_tile[1]]+row_labels[to_tile[0]]+"->")
    from_tile = history[-1][0]
    to_tile = history[-1][1]
    file.write(col_labels[from_tile[1]]+row_labels[from_tile[0]]+col_labels[to_tile[1]]+row_labels[to_tile[0]])