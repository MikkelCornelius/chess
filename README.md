# Chess Bot in C++

A personal learning project focused on building a chess engine and bot from the ground up in C++.

This repository is a work in progress and is meant to help me learn core C++ programming concepts, game logic, search algorithms, evaluation heuristics, and software design through a real project.

## Why this project exists

This project started as a way to explore:

- C++ fundamentals and object-oriented design
- board-state representation and chess rules
- legal move generation
- minimax search and evaluation functions
- performance profiling and optimization
- testing and debugging in a more complex codebase

It is intentionally a learning-focused project rather than a polished production chess engine.

## Current features

- Chess board representation
- Move generation and rule handling
- Piece evaluation logic
- Minimax-based move selection
- Unit and scenario-based testing
- Experimental Python-based UI work alongside the C++ engine

## How the bot works

The engine stores a chess position as a board structure and uses move generation to find legal moves. A minimax search then explores possible continuations, and an evaluator assigns a numerical score to positions based on material, piece placement, and positional heuristics.

The C++ bot exposes a simple interface through `get_move(...)`, which takes a board state string and returns the best move found by the search.

## Building

This project is built with g++ and targets C++20.

### Build the chess bot library

On Windows PowerShell:

```powershell
g++ -std=c++20 -shared -fPIC -o chessbot.dll .\src\chess_bot.cpp .\src\board.cpp .\src\evaluator.cpp .\src\moveGenerator.cpp .\src\mover.cpp
```

### Build the unit tests

```powershell
g++ -std=c++20 -o test_chess_bot.exe .\test\unit_test_chess_bot.cpp .\src\chess_bot.cpp .\src\board.cpp .\src\evaluator.cpp .\src\moveGenerator.cpp .\src\mover.cpp
```

Then run:

```powershell
.\test_chess_bot.exe
```

### Build the scenario tests

```powershell
g++ -std=c++20 -o test_chess_bot.exe .\test\scenario_test_chess_bot.cpp .\src\chess_bot.cpp .\src\board.cpp .\src\evaluator.cpp .\src\moveGenerator.cpp .\src\mover.cpp
```

## Notes

This is not a full-featured production-grade engine yet. There are still TODOs in the codebase related to areas like:

- check/checkmate/stalemate detection
- castling edge cases
- promotion handling
- move legality edge cases
- performance tuning for endgame and deeper search
