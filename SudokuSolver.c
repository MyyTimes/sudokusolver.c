#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ColorPaletteForTerminal.h"

#define INPUT_SUDOKU "sudoku.txt"
#define SAVE_STEPS "SaveSteps.txt"
#define MAX_LINE_LENGTH 100
#define MARKED_SLOT -1

void FillTempBoard(int**, int, int, int, int);
void FillBoardWithMarkers(int**, int, int, int, int, int, int);
int FindProperSlots(int**, int**, int, int, int, int);
int FindProperInBox(int**, int, int, int, int, int*, int*);
int FindProperInLine(int**, int, int, int, int*, int*); /* HORIZANTAL and VERTICAL */

int **CreateBoard(FILE*, int*, int*, int*);
int IsBoardComplete(int**, int);
int IsBoardValid(int**, int, int, int);
void PrintBoard(int**, int, int, int);
void CopyBoard(int**, int**, int);
void FreeBoard(int**, int);
void SaveBoardToText(FILE*, int**, int, int);

int main()
{
    FILE *sudokuFile = fopen(INPUT_SUDOKU, "r");
    FILE *saveStepsFile = fopen(SAVE_STEPS, "w");
    if(sudokuFile == NULL || saveStepsFile == NULL)
    {
        printf(RED "%s does not exist!\n" RESET, INPUT_SUDOKU);
        return 1;
    }

    int num, i, isCompleted = 0;
    int boardLength, boxWidth, boxHeight;
    int **tempBoard, **board = CreateBoard(sudokuFile, &boardLength, &boxWidth, &boxHeight);

    tempBoard = (int**)calloc(boardLength, sizeof(int*));
    for(i = 0; i < boardLength; i++)
    {
        tempBoard[i] = (int*)calloc(boardLength, sizeof(int));
    }

    /* print read text (as a board) */
    PrintBoard(board, boardLength, boxWidth, boxHeight);

    if(IsBoardValid(board, boardLength, boxWidth, boxHeight) == 0)
    {
        printf(RED "This sudoku board cannot be solvable!\n" RESET);
        return 1;
    }

    while(!isCompleted)
    {
        isCompleted = 1;
        for(num = 1; num <= boardLength; num++) /* try each possible numbers */
        {
            CopyBoard(board, tempBoard, boardLength);
            FillTempBoard(tempBoard, boardLength, boxWidth, boxHeight, num);

            /* SAVE board steps */
            SaveBoardToText(saveStepsFile, board, boardLength, num);
            SaveBoardToText(saveStepsFile, tempBoard, boardLength, num);

            if(FindProperSlots(board, tempBoard, boardLength, boxWidth, boxHeight, num))
            {
                isCompleted = 0; /* any of these number could not be located */
            }
        }
    }

    printf(GREEN "\nSOLVED SUDOKU:\n" RESET);
    PrintBoard(board, boardLength, boxWidth, boxHeight);

    if(!IsBoardValid(board, boardLength, boxWidth, boxHeight) || !IsBoardComplete(board, boardLength))
        printf(RED "Board solved: Unsuccessful\n" RESET);
    else
        printf(GREEN "Board solved: Successful\n" RESET);
    
    FreeBoard(board, boardLength);
    FreeBoard(tempBoard, boardLength);

    fclose(sudokuFile);
    fclose(saveStepsFile);

    getchar();
    return 0;
}

void SaveBoardToText(FILE *file, int **board, int length, int num)
{
    int i, j;
    fprintf(file, "Number: %d\n", num);
    for(i = 0; i < length; i++)
    {
        for(j = 0; j < length; j++)
        {
            fprintf(file, "%2d ", board[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");
}

int FindProperSlots(int **board, int **tempBoard, int boardLength, int boxW, int boxH, int num)
{
    int isFound, foundRow, foundCol, isLocated = 0;
    int i, j;

    /* IN EACH BOXES */
    for(i = 0; i < boardLength; i += boxH)
    {
        for(j = 0; j < boardLength; j += boxW)
        {
            if(FindProperInBox(tempBoard, i, j, boxW, boxH, &foundRow, &foundCol))
            {
                board[foundRow][foundCol] = num;
                isLocated = 1;
                FillBoardWithMarkers(tempBoard, boardLength, boxW, boxH, foundRow, foundCol, num);
                /* return isLocated; */
            }
        }
    }

    /* HORIZONTAL */
    for(i = 0; i < boardLength; i++) 
    {
        if (FindProperInLine(tempBoard, boardLength, i, 1, &foundRow, &foundCol))
        {
            board[foundRow][foundCol] = num;
            isLocated = 1;
            FillBoardWithMarkers(tempBoard, boardLength, boxW, boxH, foundRow, foundCol, num);
            /* return isLocated; */
        }
    }

    /* VERTICAL */
    for(i = 0; i < boardLength; i++) 
    {
        if (FindProperInLine(tempBoard, boardLength, i, 0, &foundRow, &foundCol)) 
        {
            board[foundRow][foundCol] = num;
            isLocated = 1;
            FillBoardWithMarkers(tempBoard, boardLength, boxW, boxH, foundRow, foundCol, num);
            /* return isLocated; */
        }
    }

    return isLocated;
}

int FindProperInLine(int **tempBoard, int boardLength, int fixedIndex, int isRow, int *foundRow, int *foundCol)
{
    int i, row, col;
    int isFound = 0;
    for(i = 0; i < boardLength; i++) 
    {
        row = isRow ? fixedIndex : i;
        col = isRow ? i : fixedIndex;

        if(tempBoard[row][col] == 0) 
        {
            if(isFound == 1) 
                return 0; /* more than one empty slot */
            else
            {
                isFound = 1;
                /* save pos */
                *foundRow = row;
                *foundCol = col;
            }
        }
    }

    return isFound;
}


int FindProperInBox(int **tempBoard, int row, int col, int boxW, int boxH, int *foundRow, int *foundCol)
{
    int isFound = 0, i, j;
    for(i = 0; i < boxH; i++)
    {
        for(j = 0; j < boxW; j++)
        {
            if(tempBoard[row + i][col + j] == 0)
            {
                if(isFound == 1)
                    return 0; /* more than one empty slot */
                else
                {
                    isFound = 1;
                    /* save pos */
                    *foundRow = row + i;
                    *foundCol = col + j;
                }
            }
        }
    }

    return isFound;
}

void FillTempBoard(int **markedBoard, int boardLength, int boxW, int boxH, int num)
{
    int row, col, i, j;

    /* FILL THE BOARD WITH '-1' */
    for(row = 0; row < boardLength; row++)
    {
        for(col = 0; col < boardLength; col++)
        {
            if(markedBoard[row][col] == num)
            {
                FillBoardWithMarkers(markedBoard, boardLength, boxW, boxH, row, col, num);
            }
        }
    }    
}

void FillBoardWithMarkers(int **markedBoard, int boardLength, int boxW, int boxH, int row, int col, int num)
{
    int i, j, startRow, startCol;

    /* FILL ROW */
    for(i = 0; i < boardLength; i++)
    {
        if(markedBoard[row][i] != num)
            markedBoard[row][i] = MARKED_SLOT;
    }

    /* FILL COLUMN */
    for(i = 0; i < boardLength; i++)
    {
        if(markedBoard[i][col] != num)
            markedBoard[i][col] = MARKED_SLOT;
    }

    /* CHECK BOX */
    startRow = row - (row % boxH);
    startCol = col - (col % boxW);
    for(i = startRow; i < startRow + boxH; i++)
    {
        for(j = startCol; j < startCol + boxW; j++)
        {
            if(markedBoard[i][j] != num)
                markedBoard[i][j] = MARKED_SLOT;
        }
    }
}

int IsBoardComplete(int **board, int boardLength)
{
    int i, j, isComplete = 1;
    for(i = 0; i < boardLength; i++)
    {
        for(j = 0; j < boardLength; j++)
        {
            if(board[i][j] == 0)
            {
                isComplete = 0;
                break;
            }
        }
    }
    return isComplete;
}

int IsBoardValid(int **board, int boardLength, int boxW, int boxH)
{
    int row, col, i, j;
    int startCol, startRow;
    int isValid = 1;

    for(row = 0; row < boardLength; row++)
    {
        for(col = 0; col < boardLength; col++)
        {
            if(board[row][col] == 0) continue;

            /* CHECK ROW */
            for(i = 0; i < boardLength; i++)
            {
                if(board[row][col] == board[row][i] && col != i)
                {
                    printf("The number %d is the same in (%d, %d) and (%d, %d)!\n", board[row][col], row, col, row, i);
                    isValid = 0;
                }
            }
            /* CHECK COLUMN */
            for(i = 0; i < boardLength; i++)
            {
                if(board[row][col] == board[i][col] && row != i)
                {
                    printf("The number %d is the same in (%d, %d) and (%d, %d)!\n", board[row][col], row, col, i, col);
                    isValid = 0;
                }
            }
            /* CHECK BOX */
            startRow = row - (row % boxH);
            startCol = col - (col % boxW);
            for(i = startRow; i < startRow + boxH; i++)
            {
                for(j = startCol; j < startCol + boxW; j++)
                {
                    if(board[row][col] == board[i][j] && !(row == i && col == j))
                    {
                        printf("The number %d is the same in (%d, %d) and (%d, %d)!\n", board[row][col], row, col, i, j);
                        isValid = 0;
                    }
                }
            }
        }
    }

    return isValid;
}

int **CreateBoard(FILE *file, int *boardLength, int *boxWidth, int *boxHeight)
{
    int **board;
    int col, row, numberStartIndex, currentIndex;
    char line[MAX_LINE_LENGTH];
    char *token;

    fgets(line, sizeof(line), file);
    sscanf(line, " %d %d %d", boardLength, boxHeight, boxWidth);

    board = (int**)calloc(*boardLength, sizeof(int*));

    row = 0;
    numberStartIndex = 0;
    currentIndex = 0;
    while(fgets(line, sizeof(line), file) && row < *boardLength)
    {
        board[row] = (int*)calloc(*boardLength, sizeof(int));
        line[strcspn(line, "\n")] = '\0';

        col = 0;
        numberStartIndex = 0;
        currentIndex = 0;

        while(1)
        {
            while(line[currentIndex] != ',' && line[currentIndex] != '\0')
            {
                currentIndex++;
            }

            if(numberStartIndex == currentIndex)
            {
                board[row][col] = 0;
            }
            else
            {
                board[row][col] = atoi(line + numberStartIndex);
            }
  
            if(line[currentIndex] == '\0')
                break;

            col++;
            currentIndex++;
            numberStartIndex = currentIndex;
        }

        row++;
    }

    return board;
}

void PrintBoard(int **board, int boardLength, int boxW, int boxH)
{
    int row, col;

    for(row = 0; row < boardLength; row++)
    {  
        if(row % boxH == 0) /* PRINT LINE TO SEPERATE BOXES */
        {
            for(col = 0; col < boardLength * 2 + (boardLength / boxW); col++)
            {
                if(col % (boxW * 2 + 1) == 0) printf(BMAGENTA "." RESET);
                else printf(BYELLOW "-" RESET);
            }
            printf(BMAGENTA ".\n" RESET);
        }
        
        for(col = 0; col < boardLength; col++)
        {
            if(col % boxW == 0) printf(BYELLOW "|" RESET);

            if(board[row][col] == 0) printf("  ");
            else
            {
                printf("%d ", board[row][col]);
            }
        }
        printf(BYELLOW "|\n" RESET);
    }

    /* LAST LINE */
    for(col = 0; col < boardLength * 2 + (boardLength / boxW); col++)
    {
        if(col % (boxW * 2 + 1) == 0) printf(BMAGENTA "." RESET);
        else printf(BYELLOW "-" RESET);
    }
    printf(BMAGENTA ".\n" RESET);
}

void CopyBoard(int **board, int **tempBoard, int boardLength)
{
    int i, j;
    for(i = 0; i < boardLength; i++)
        for(j = 0; j < boardLength; j++)
            tempBoard[i][j] = board[i][j];
}

void FreeBoard(int **board, int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        free(board[i]);
    }
    free(board);
}


