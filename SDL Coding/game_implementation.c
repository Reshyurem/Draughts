#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <stdio.h>
#define clrscr() printf("\e[1;1H\e[2J")
#endif

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

int toss()
{
    int toss = rand() % 2;

    if (toss == 0)
    {
        printf("BLACK HAS WON THE TOSS!\n");
        return 0;
    }
    else
    {
        printf("WHITE HAS WON THE TOSS!\n");
        return 1;
    }
}

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
        if (grid[Y - 2][X + 2].piece == 'X' && Y == 2) // Checking if can be made king
        {
            grid[Y - 2][X + 2].piece = 'B'; // Turns black normal piece(X) into black king(B)
            grid[Y - 2][X + 2].type = 'K';
        }
        break;

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
        if (grid[Y - 2][X + 2].piece == 'X' && Y == 2) // Checking if can be made king
        {
            grid[Y - 2][X + 2].piece = 'B'; // Turns black normal piece(X) into black king(B)
            grid[Y - 2][X + 2].type = 'K';
        }
        break;

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
        if (grid[Y + 2][X + 2].piece == 'O' && Y == 5) // Checking if can be made king
        {
            grid[Y + 2][X + 2].piece = 'W'; // Turns white normal piece(O) into white king(W)
            grid[Y + 2][X + 2].type = 'K';
        }
        break;

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
        if (grid[Y + 2][X - 2].piece == 'O' && Y == 5) // Checking if can be made king
        {
            grid[Y + 2][X - 2].piece = 'W'; // Turns white normal piece(O) into white king(W)
            grid[Y + 2][X - 2].type = 'K';
        }
        break;

    default:
        break;
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
        grid[Y][X].type = ' ';                         // Movement complete
        if (grid[Y - 1][X - 1].piece == 'X' && Y == 1) // Checking if can be made king
        {
            grid[Y - 1][X - 1].piece = 'B'; // Turns black normal piece(X) into black king(B)
            grid[Y - 1][X - 1].type = 'K';
        }
        break;

    case 1: // top right
        grid[Y - 1][X + 1].piece = grid[Y][X].piece;
        grid[Y - 1][X + 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                         // Movement complete
        if (grid[Y - 1][X + 1].piece == 'X' && Y == 1) // Checking if can be made king
        {
            grid[Y - 1][X + 1].piece = 'B'; // Turns black normal piece(X) into black king(B)
            grid[Y - 1][X + 1].type = 'K';
        }
        break;

    case 2: // bottom right
        grid[Y + 1][X + 1].piece = grid[Y][X].piece;
        grid[Y + 1][X + 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                        // Movement complete
        if (grid[Y + 1][X + 1].piece == 'O' && Y == 6) // Checking if can be made king
        {
            grid[Y + 1][X + 1].piece = 'W'; // Turns white normal piece(O) into white king(W)
            grid[Y + 1][X + 1].type = 'K';
        }
        break;

    case 3: // bottom left
        grid[Y + 1][X - 1].piece = grid[Y][X].piece;
        grid[Y + 1][X - 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';
        if (grid[Y + 1][X - 1].piece == 'O' && Y == 6) // Checking if can be made king
        {
            grid[Y + 1][X - 1].piece = 'W'; // Turns white normal piece(O) into white king(W)
            grid[Y + 1][X - 1].type = 'K';
        }
        break;

    default:
        break;
    }
}

int input(Cell grid[8][8], int turn) // turn 1 white(O,W), 0 black(X,B)
{
    char letter;
    int X, Y;
    printf("Enter the position of the piece which you wish to move: ");
    scanf("%d", &Y);            //Scan number first since it will help avoid buffer problems
    scanf("%c", &letter);
    letter = toupper(letter);
    X = letter - 'A'; //j index of grid
    Y = Y - 1;        //i index of grid

    // printf("%d %d %c %c\n", X, Y, grid[Y][X].piece, grid[Y][X].type);

    if (X >= 8 || Y >= 8 || X < 0 || Y < 0)
        return -1;
    if (turn)
    {
        if (grid[Y][X].piece != 'O' && grid[Y][X].piece != 'W')
            return -1;
    }
    else
    {
        if (grid[Y][X].piece != 'X' && grid[Y][X].piece != 'B')
            return -1;
    }

    int inp[9] = {0};
    int flag = 0;
    int input;
    if (grid[Y][X].piece == 'O') // Checks if white or black as direction of movement is different
    {
        if (X <= 6 && Y <= 6 && grid[Y + 1][X + 1].piece == ' ')
        {
            printf("1) Move bottom right\n");
            inp[1] = 1;
            flag = 1;
        }

        else if (X <= 5 && Y <= 5 && (grid[Y + 1][X + 1].piece == 'X' || grid[Y + 1][X + 1].piece == 'B') && grid[Y + 2][X + 2].piece == ' ')
        {
            printf("2) Take piece on bottom right\n");
            inp[2] = 1;
            flag = 1;
        }

        if (X >= 1 && Y <= 6 && grid[Y + 1][X - 1].piece == ' ')
        {

            printf("3) Move bottom left\n");
            inp[3] = 1;
            flag = 1;
        }
        else if (X >= 2 && Y <= 5 && (grid[Y + 1][X - 1].piece == 'X' || grid[Y + 1][X - 1].piece == 'B') && grid[Y + 2][X - 2].piece == ' ')
        {
            printf("4) Take piece on bottom left\n");
            inp[4] = 1;
            flag = 1;
        }
        if (grid[Y][X].type == 'K')
        {
            if (X >= 1 && Y >= 1 && grid[Y - 1][X - 1].piece == ' ')
            {
                printf("5) Move top left\n");
                inp[5] = 1;
                flag = 1;
            }
            else if (X >= 2 && Y >= 2 && (grid[Y - 1][X - 1].piece == 'X' || grid[Y - 1][X - 1].piece == 'B') && grid[Y - 2][X - 2].piece == ' ')
            {
                printf("6) Take piece on top left\n");
                inp[6] = 1;
                flag = 1;
            }
            if (X <= 6 && Y >= 1 && grid[Y - 1][X + 1].piece == ' ')
            {
                printf("7) Move top right\n");
                inp[7] = 1;
                flag = 1;
            }
            else if (X <= 5 && Y >= 2 && (grid[Y - 1][X + 1].piece == 'X' || grid[Y - 1][X + 1].piece == 'B') && grid[Y - 2][X + 2].piece == ' ')
            {
                printf("8) Take piece on top right\n");
                inp[8] = 1;
                flag = 1;
            }
        }
        if (!flag)
            return -1;
        scanf("%d", &input);
        if (inp[input] == 1)
        {
            switch (input)
            {
            case 1:
                movepiece(grid, 2, Y, X);
                break;

            case 2:
                takepiece(grid, 2, Y, X);
                break;

            case 3:
                movepiece(grid, 3, Y, X);
                break;

            case 4:
                takepiece(grid, 3, Y, X);
                break;

            case 5:
                movepiece(grid, 0, Y, X);
                break;

            case 6:
                takepiece(grid, 0, Y, X);
                break;

            case 7:
                movepiece(grid, 1, Y, X);
                break;

            case 8:
                takepiece(grid, 1, Y, X);
                break;

            default:
                break;
            }
        }
        else
            return -1;
    }
    else //black
    {
        if (X >= 1 && Y >= 1 && grid[Y - 1][X - 1].piece == ' ')
        {
            printf("1) Move top left\n");
            inp[1] = 1;
            flag = 1;
        }
        else if (X >= 2 && Y >= 2 && (grid[Y - 1][X - 1].piece == 'O' || grid[Y - 1][X - 1].piece == 'W') && grid[Y - 2][X - 2].piece == ' ')
        {
            printf("2) Take piece on top left\n");
            inp[2] = 1;
            flag = 1;
        }
        if (X <= 6 && Y >= 1 && grid[Y - 1][X + 1].piece == ' ')
        {
            printf("3) Move top right\n");
            inp[3] = 1;
            flag = 1;
        }
        else if (X <= 5 && Y >= 2 && (grid[Y - 1][X + 1].piece == 'O' || grid[Y - 1][X + 1].piece == 'W') && grid[Y - 2][X + 2].piece == ' ')
        {
            printf("4) Take piece on top right\n");
            inp[4] = 1;
            flag = 1;
        }

        if (grid[Y][X].type == 'K')
        {
            if (X <= 6 && Y <= 6 && grid[Y + 1][X + 1].piece == ' ')
            {
                printf("5) Move bottom right\n");
                inp[5] = 1;
                flag = 1;
            }

            else if (X <= 5 && Y <= 5 && (grid[Y + 1][X + 1].piece == 'O' || grid[Y + 1][X + 1].piece == 'W') && grid[Y + 2][X + 2].piece == ' ')
            {
                printf("6) Take piece on bottom right\n");
                inp[6] = 1;
                flag = 1;
            }
            if (X >= 1 && Y <= 6 && grid[Y + 1][X - 1].piece == ' ')
            {
                printf("7) Move bottom left\n");
                inp[7] = 1;
                flag = 1;
            }
            else if (X >= 2 && Y <= 5 && (grid[Y + 1][X - 1].piece == 'O' || grid[Y + 1][X - 1].piece == 'W') && grid[Y + 2][X - 2].piece == ' ')
            {
                printf("8) Take piece on bottom left\n");
                inp[8] = 1;
                flag = 1;
            }
        }
        if (!flag)
            return -1;
        scanf("%d", &input);
        if (inp[input] == 1)
        {
            switch (input)
            {
            case 1:
                movepiece(grid, 0, Y, X);
                break;

            case 2:
                takepiece(grid, 0, Y, X);
                break;

            case 3:
                movepiece(grid, 1, Y, X);
                break;

            case 4:
                takepiece(grid, 1, Y, X);
                break;
            case 5:
                movepiece(grid, 2, Y, X);
                break;

            case 6:
                takepiece(grid, 2, Y, X);
                break;

            case 7:
                movepiece(grid, 3, Y, X);
                break;

            case 8:
                takepiece(grid, 3, Y, X);
                break;

            default:
                break;
            }
        }
        else
            return -1;
    }
    return 0;
}

int main()
{
    srand(time(NULL));
    Cell grid[8][8];
    intializeboard(grid);
    display(grid);
    int turn = toss(); // 1 for white, 0 for black
    for (;;)
    {
        if (noofblackpieces == 0) //Winning condition
        {
            printf("WHITE HAS WON THE GAME!\n");
            break;
        }
        else if (noofwhitepieces == 0)
        {
            printf("BLACK HAS WON THE GAME!\n");
            break;
        }
        if (turn)
            printf("WHITE'S TURN\n");
        else
            printf("BLACK'S TURN\n");

        if (input(grid, turn) == -1)
            printf("INVALID MOVE, IT IS AGAIN ");
        else
        {
            // Stalemate checking function with winning function
            if (turn)
                turn = 0;
            else
                turn = 1;
            clrscr();
            display(grid);
        }
    }
    return 0;
}