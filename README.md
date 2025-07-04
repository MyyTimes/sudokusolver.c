# Sudoku Solver in C

This is a logic-based Sudoku solver written in C.  
It reads Sudoku boards of custom size (e.g. 9x9) from a text file.
When solving Sudoku, for each number, the slots where it cannot be placed are marked, and the possible slots are identified.
Lastly, prints the solved board with color-coded formatting in the terminal.

## Features

- Parses boards from a file (comma-separated format).
- Supports non-standard Sudoku dimensions (e.g. 6x6, 12x12).
- Sudoku can be divided into its own special boxes (e.g. 2x3).
- Uses rule-based logic to eliminate invalid placements.
- Detects invalid or unsolvable sudoku boards.
- Visual, colored terminal output using ANSI escape codes.
- It can be compiled with the --ansi option.
- It writes each step as output to a text file (named `SaveSteps.txt`).

## Input Format

Text name must be names as `sudoku.txt`
The first line of the input file specifies:
[length of the sudoku board side] [length of the boxes height] [length of the boxes width]
Empty cells are represented by consecutive commas (`,,`).  
Example text:  
  6 2 3  
  1,,,,,4  
  ,2,,,,  
  ,,3,5,,  
  ,,6,4,,  
  ,6,,,5,  
  ,,1,,,6  

## Build and Run

- GCC compiler (or any C compiler)

### Compile

bash
`gcc --ansi SudokuSolver.c -o sudokusolver`

FILE STRUCTURE  
├── SudokuSolver.c  
├── ColorPaletteForTerminal.h  
├── sudoku.txt  
└── README.md  

## Author
Muhammed Yusuf Yıldız

Thanks to Yağız Rasim Kalay for the sudoku examples.
