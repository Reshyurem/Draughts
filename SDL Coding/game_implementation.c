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

int noofwhitepieces = 12; // Keeps track of the number of white and black pieces
int noofblackpieces = 12;

struct cell__
{
    char piece;
    // O for white, W for white king
    // X for black, B for black king
    char type; // K for king , N for normal
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
            {
                grid[i][j].piece = ' ';
                grid[i][j].type = ' ';
            }
        }
    }

    //MIDDLE
    for (int i = 3; i < 5; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            grid[i][j].piece = ' ';
            grid[i][j].type = ' ';
        }
    }

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
            {
                grid[i][j].piece = ' ';
                grid[i][j].type = ' ';
            }
        }
    }
}

void takepiece(Cell grid[8][8], int direction, int Y, int X) // Coordinate of the attacking piece
{
    //0 top left
    //1 top right
    //2 bottom right
    //3 bottom left
    switch (direction)
    {
    case 0:                                          //top left
        grid[Y - 2][X - 2].piece = grid[Y][X].piece; // Moves the piece
        grid[Y - 2][X - 2].type = grid[Y][X].type;
        grid[Y][X].piece = ' '; // Empties the original place
        grid[Y][X].type = ' ';
        //checks which colour the opposite piece is and decreases the noofpieces global variable
        if (grid[Y - 1][X - 1].piece == 'O' || grid[Y - 1][X - 1].piece == 'W')
            noofwhitepieces--;
        else
            noofblackpieces--;
        grid[Y - 1][X - 1].piece = ' '; // Deletes the opposite piece
        grid[Y - 1][X - 1].type = ' ';

    case 1: //top right
        grid[Y - 2][X + 2].piece = grid[Y][X].piece;
        grid[Y - 2][X + 2].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';
        if (grid[Y - 1][X + 1].piece == 'O' || grid[Y - 1][X + 1].piece == 'W')
            noofwhitepieces--;
        else
            noofblackpieces--;
        grid[Y - 1][X + 1].piece = ' ';
        grid[Y - 1][X + 1].type = ' ';

    case 2: //bottom right
        grid[Y + 2][X + 2].piece = grid[Y][X].piece;
        grid[Y + 2][X + 2].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';
        if (grid[Y + 1][X + 1].piece == 'O' || grid[Y + 1][X + 1].piece == 'W')
            noofwhitepieces--;
        else
            noofblackpieces--;
        grid[Y + 1][X + 1].piece = ' ';
        grid[Y + 1][X + 1].type = ' ';

    case 3: //bottom left
        grid[Y + 2][X - 2].piece = grid[Y][X].piece;
        grid[Y + 2][X - 2].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';
        if (grid[Y + 1][X - 1].piece == 'O' || grid[Y + 1][X - 1].piece == 'W')
            noofwhitepieces--;
        else
            noofblackpieces--;
        grid[Y + 1][X - 1].piece = ' ';
        grid[Y + 1][X - 1].type = ' ';

    default:
    }
}

void movepiece(Cell grid[8][8], int direction, int Y, int X)
{
    //0 top left
    //1 top right
    //2 bottom right
    //3 bottom left

    switch (direction)
    {
    case 0: // top left
        grid[Y - 1][X - 1].piece = grid[Y][X].piece;
        grid[Y - 1][X - 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                          // Movement complete
        if (grid[Y - 1][X - 1].piece == 'X' && Y == 1) // Checking if can be made king
        {
            grid[Y - 1][X - 1].piece = 'B'; // Turns black normal piece(X) into black king(B)
            grid[Y - 1][X - 1].type = 'K';
        }

    case 1: // top right
        grid[Y - 1][X + 1].piece = grid[Y][X].piece;
        grid[Y - 1][X + 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                          // Movement complete
        if (grid[Y - 1][X + 1].piece == 'X' && Y == 1) // Checking if can be made king
        {
            grid[Y - 1][X + 1].piece = 'B'; // Turns black normal piece(X) into black king(B)
            grid[Y - 1][X + 1].type = 'K';
        }

    case 2: // bottom right
        grid[Y + 1][X + 1].piece = grid[Y][X].piece;
        grid[Y + 1][X + 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                        // Movement complete
        if (grid[Y + 1][X + 1].piece = 'O' && Y == 6) // Checking if can be made king
        {
            grid[Y + 1][X + 1].piece = 'W'; // Turns white normal piece(O) into black king(W)
            grid[Y + 1][X + 1].type = 'K';
        }

    case 3: // bottom left
        grid[Y + 1][X - 1].piece = grid[Y][X].piece;
        grid[Y + 1][X - 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                        // Movement complete
        if (grid[Y + 1][X - 1].piece = 'O' && Y == 6) // Checking if can be made king
        {
            grid[Y + 1][X - 1].piece = 'W'; // Turns white normal piece(O) into black king(W)
            grid[Y + 1][X - 1].type = 'K';
        }

    default:
    }
}

int input(Cell grid[8][8], bool turn) // turn 1 white(O,W), 0 black(X,B)
{
    char letter;
    int X, Y;
    printf("Enter the position of the piece which you wish to move: ");
    scanf("%c", &letter);
    scanf("%d", &Y);
    X = letter - '0'; //j index of grid
    Y = Y - 1;        //i index of grid

    if (X >= 8 || Y >= 8 || X < 0 || Y < 0)
        return -1;

    if (turn)
    {
        if (grid[Y][X].piece != 'O')
            return -1;
    }
    else
    {
        if (grid[Y][X].piece != 'X')
            return -1;
    }

    if (grid[Y][X].type == 'N') // Checks for the type of the piece(normal or king)
    {
        if (grid[Y][X].piece == 'O') // Checks if white or black as direction of movement is different
        {
            if (X <= 6 && Y <= 6 && grid[Y + 1][X + 1].piece == ' ')
                printf("1) Move bottom right\n");
            else if (X <= 5 && Y <= 5 && (grid[Y + 1][X + 1].piece == 'X' || grid[Y + 1][X + 1].piece == 'B') && grid[Y + 2][X + 2].piece == ' ')
                printf("2) Take piece on bottom right\n");

            if (X >= 1 && Y <= 6 && grid[Y + 1][X - 1].piece == ' ')
                printf("3) Move bottom left\n");
            else if (X >= 2 && Y <= 5 && (grid[Y + 1][X - 1].piece == 'X' || grid[Y + 1][X - 1].piece == 'B') && grid[Y + 2][X - 2].piece == ' ')
                printf("4) Take piece on bottom left\n");

            int inp;
            scanf("%d", &inp);
        }
        else //black
        {
            if (X >= 1 && Y >= 1 && grid[Y - 1][X - 1].piece == ' ')
                printf("1) Move top left\n");
            else if (X >= 2 && Y >= 2 && (grid[Y - 1][X - 1].piece == 'O' || grid[Y - 1][X - 1].piece == 'W') && grid[Y - 2][X - 2].piece == ' ')
                printf("2) Take piece on top left\n");

            if (X <= 6 && Y >= 1 && grid[Y - 1][X + 1].piece == ' ')
                printf("3) Move top right\n");
            else if (X <= 5 && Y >= 2 && (grid[Y - 1][X + 1].piece == 'O' || grid[Y - 1][X + 1].piece == 'W') && grid[Y - 2][X + 2].piece == ' ')
                printf("4) Take piece on top right\n");
        }
    }
    else // King piece
    {
        if (grid[Y][X].piece == 'O') // Checks if white or black as direction of movement is different
        {
            if (X <= 6 && Y <= 6 && grid[Y + 1][X + 1].piece == ' ')
                printf("1) Move bottom right\n");
            else if (X <= 5 && Y <= 5 && (grid[Y + 1][X + 1].piece == 'X' || grid[Y + 1][X + 1].piece == 'B') && grid[Y + 2][X + 2].piece == ' ')
                printf("2) Take piece on bottom right\n");

            if (X >= 1 && Y <= 6 && grid[Y + 1][X - 1].piece == ' ')
                printf("3) Move bottom left\n");
            else if (X >= 2 && Y <= 5 && (grid[Y + 1][X - 1].piece == 'X' || grid[Y + 1][X - 1].piece == 'B') && grid[Y + 2][X - 2].piece == ' ')
                printf("4) Take piece on bottom left\n");

            if (X >= 1 && Y >= 1 && grid[Y - 1][X - 1].piece == ' ')
                printf("5) Move top left\n");
            else if (X >= 2 && Y >= 2 && (grid[Y - 1][X - 1].piece == 'X' || grid[Y - 1][X - 1].piece == 'B') && grid[Y - 2][X - 2].piece == ' ')
                printf("6) Take piece on top left\n");

            if (X <= 6 && Y >= 1 && grid[Y - 1][X + 1].piece == ' ')
                printf("7) Move top right\n");
            else if (X <= 5 && Y >= 2 && (grid[Y - 1][X + 1].piece == 'X' || grid[Y - 1][X + 1].piece == 'B') && grid[Y - 2][X + 2].piece == ' ')
                printf("8) Take piece on top right\n");
        }
        else //black
        {
            if (X >= 1 && Y >= 1 && grid[Y - 1][X - 1].piece == ' ')
                printf("1) Move top left\n");
            else if (X >= 2 && Y >= 2 && (grid[Y - 1][X - 1].piece == 'O' || grid[Y - 1][X - 1].piece == 'W') && grid[Y - 2][X - 2].piece == ' ')
                printf("2) Take piece on top left\n");

            if (X <= 6 && Y >= 1 && grid[Y - 1][X + 1].piece == ' ')
                printf("3) Move top right\n");
            else if (X <= 5 && Y >= 2 && (grid[Y - 1][X + 1].piece == 'O' || grid[Y - 1][X + 1].piece == 'W') && grid[Y - 2][X + 2].piece == ' ')
                printf("4) Take piece on top right\n");

            if (X <= 6 && Y <= 6 && grid[Y + 1][X + 1].piece == ' ')
                printf("5) Move bottom right\n");
            else if (X <= 5 && Y <= 5 && (grid[Y + 1][X + 1].piece == 'O' || grid[Y + 1][X + 1].piece == 'W') && grid[Y + 2][X + 2].piece == ' ')
                printf("6) Take piece on bottom right\n");

            if (X >= 1 && Y <= 6 && grid[Y + 1][X - 1].piece == ' ')
                printf("7) Move bottom left\n");
            else if (X >= 2 && Y <= 5 && (grid[Y + 1][X - 1].piece == 'O' || grid[Y + 1][X - 1].piece == 'W') && grid[Y + 2][X - 2].piece == ' ')
                printf("8) Take piece on bottom left\n");
        }
    }

    /* int inp;
    printf("Enter the number corresponding to the move you wish to make:");
    scanf("%d", &inp); */
}

int main()
{
    Cell grid[8][8];
    intializeboard(grid);
    display(grid);
}