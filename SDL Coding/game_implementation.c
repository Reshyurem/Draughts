/* #include <stdio.h>
#include <math.h>

#define WHITE  
#define BLACK

struct gridBoard__
{
    int colour;
    bool 
};

typedef struct gridBoard__ gridBoard;

void initializeboard(Pos board[][])
{
    //white on top
    //black on the bottom
    for (int row = 0; row < 3; row++)
    {

    }
}

void chk_pos(int x, int y)
{
}

int main()
{
    Pos board[8][8];
    initializeboard(board);
} */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct cell__
{
    char piece; //O for white, X for black
    char type;  // K for king , N for normal
};
typedef struct cell__ Cell;

void display(Cell grid[8][8])
{
    // displays the board's current status whenever called
    for (int i = 0; i < 8; i++)
    {
        printf("  +---+---+---+---+---+---+---+---+\n");
        printf("%d | %c | %c | %c | %c | %c | %c | %c | %c |\n",
               (i + 1), grid[i][0].piece, grid[i][1].piece, grid[i][2].piece, grid[i][3].piece, grid[i][4].piece,
               grid[i][5].piece, grid[i][6].piece, grid[i][7].piece);
    }
    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    A   B   C   D   E   F   G   H    \n");
}

void intializeboard(Cell grid[8][8])
{
    //white on top (O)
    //black in the bottom (X)

    //WHITE
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if ((i + j) % 2 == 1)
            {
                grid[i][j].piece = 'O';
                grid[i][j].type = 'N';
            }    
            else
                grid[i][j].piece = ' ';
                
        }
    }
    for (int i = 3; i < 5; i++)
        for (int j = 0; j < 8; j++)
            grid[i][j].piece = ' ';

    //BLACK
    for (int i = 5; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if ((i + j) % 2 == 1)
            {
                grid[i][j].piece = 'X';
                grid[i][j].type = 'N';
            }                
            else
                grid[i][j].piece = ' ';
        }
    }
}

int input(Cell grid[8][8], bool turn) // turn 0 white(O), 1 black(X)
{
    char letter;
    int X, Y;
    printf("Enter the position of the piece which you wish to move: ");
    scanf("%c", &letter);
    scanf("%d", &Y);
    X = letter - '0';

    if (X >= 8 || Y >= 8 || X < 0 || Y < 0)
        return -1;

    if (!turn)
    {
        if (grid[Y][X].piece != 'O')
            return -1;
    }
    else
    {
        if (grid[Y][X].piece != 'X')
            return -1;
    }

    printf("1) Move to upper left   2) Move to upper right\n 3) Move to lower left  4) Move to lower right\n");
    int inp;
    scanf("%d", &inp);
    switch (inp)
    {
    case 1:
        if(X == 0 || Y == 0)
            printf("Can't move piece there.");
            return -1;


    case 2:        
    }
}


int main()
{
    Cell grid[8][8];
    intializeboard(grid);
    display(grid);
}