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

#define WHITE 1
#define BLACK 0

#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMRIGHT 2
#define BOTTOMLEFT 3

int noofwhitepieces = 12; // Keeps track of the number of white and black pieces
int noofblackpieces = 12;

struct cell__
{
    char piece;
    // O for white, W for white king
    // X for black, B for black king
    char type; // K for king , N for normal
    int pieceNo;
};

struct Pos{
    int x;
    int y;
};

typedef struct Pos pos;
typedef struct cell__ Cell;

struct stackelem{
    char piece;
    int oldposY, oldposX;
    int newposY, newposX;
    struct stackelem* next;
};
typedef struct stackelem Stackelem;
typedef struct stackelem* Ptrtostackelem;

pos Blacks[12], Whites[12];

void stackpush(char piece, int oldposY, int oldposX, int newposY, int newposX, Ptrtostackelem* stack)
{
    Ptrtostackelem element = (Ptrtostackelem)malloc(sizeof(Stackelem));
    element->piece = piece;
    element->oldposY = oldposY;
    element->oldposX = oldposX;
    element->newposY = newposY;
    element->newposX = newposX;
    element->next = *stack;
    (*stack) = element;
}

void stacktop(Ptrtostackelem stack)
{
    if(stack != NULL)
        printf("You moved %c, from %d%d to %d%d\n", stack->piece, stack->oldposY-1, stack->oldposX + 'A', stack->newposY-1, stack->newposX + 'A');
    else   
        printf("You haven't performed any moves yet\n");
}
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
    int k = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if ((i + j) % 2 == 1)
            {
                grid[i][j].piece = 'O';
                grid[i][j].type = 'N';
                grid[i][j].pieceNo = k;
                Whites[k].x = j;
                Whites[k].y = i;
                k++;
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
    k = 11;
    for (int i = 5; i < 8; i++)
    {
        for (int j = 7; j >= 0; j--)
        {
            if ((i + j) % 2 == 1)
            {
                grid[i][j].piece = 'X';
                grid[i][j].type = 'N';
                grid[i][j].pieceNo = k;
                Blacks[k].x = j;
                Blacks[k].y = i;
                k--;
            }
            else
            {
                grid[i][j].piece = ' ';
                grid[i][j].type = ' ';
            }
        }
    }
}

int validMovepiece(Cell grid[8][8], int Y, int X, int direction)
{
    //0 top left
    //1 top right
    //2 bottom right
    //3 bottom left
    switch (direction)
    {
    case TOPLEFT:
        if (grid[Y - 1][X - 1].piece == ' ')
            return 1;
        break;

    case TOPRIGHT:
        if (grid[Y - 1][X + 1].piece == ' ')
            return 1;
        break;

    case BOTTOMRIGHT:
        if (grid[Y + 1][X + 1].piece == ' ')
            return 1;
        break;

    case BOTTOMLEFT:
        if (grid[Y + 1][X - 1].piece == ' ')
            return 1;
        break;

    default:
        break;
    }
    return 0;
}

int validTakepiece(Cell grid[8][8], int Y, int X, int turn, int direction)
{
    //0 top left
    //1 top right
    //2 bottom right
    //3 bottom left
    char opp1, opp2;

    if (turn) //white
    {
        opp1 = 'X';
        opp2 = 'B';
    }
    else //black
    {
        opp1 = 'O';
        opp2 = 'W';
    }

    switch (direction)
    {
    case TOPLEFT:
        if ((grid[Y - 1][X - 1].piece == opp1 || grid[Y - 1][X - 1].piece == opp2) && grid[Y - 2][X - 2].piece == ' ')
            return 1;
        break;

    case TOPRIGHT:
        if ((grid[Y - 1][X + 1].piece == opp1 || grid[Y - 1][X + 1].piece == opp2) && grid[Y - 2][X + 2].piece == ' ')
            return 1;
        break;

    case BOTTOMRIGHT:
        if ((grid[Y + 1][X + 1].piece == opp1 || grid[Y + 1][X + 1].piece == opp2) && grid[Y + 2][X + 2].piece == ' ')
            return 1;
        break;

    case BOTTOMLEFT:
        if ((grid[Y + 1][X - 1].piece == opp1 || grid[Y + 1][X - 1].piece == opp2) && grid[Y + 2][X - 2].piece == ' ')
            return 1;
        break;

    default:
        break;
    }
    return 0;
}

void checkallpieces(Cell grid[8][8], int turn)
{
    int white = 0;
    int black = 0;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (grid[i][j].piece == 'O' || grid[i][j].piece == 'W')
            {
                if (j <= 6 && i <= 6 && validMovepiece(grid, i, j, BOTTOMRIGHT))
                    white = 1;
                else if (j <= 5 && i <= 5 && validTakepiece(grid, i, j, WHITE, BOTTOMRIGHT))
                {
                    white = 1;
                    if (turn)
                        printf("ATTACKING PIECE ON %d%c\n", i + 1, j + 'A');
                }
                if (j >= 1 && i <= 6 && validMovepiece(grid, i, j, BOTTOMLEFT))
                    white = 1;
                else if (j >= 2 && i <= 5 && validTakepiece(grid, i, j, WHITE, BOTTOMLEFT))
                {
                    white = 1;
                    if (turn)
                        printf("ATTACKING PIECE ON %d%c\n", i + 1, j + 'A');
                }
                if (grid[i][j].type == 'K')
                {
                    if (j >= 1 && i >= 1 && validMovepiece(grid, i, j, TOPLEFT))
                        white = 1;
                    else if (j >= 2 && i >= 2 && validTakepiece(grid, i, j, WHITE, TOPLEFT))
                    {
                        white = 1;
                        if (turn)
                            printf("ATTACKING PIECE ON %d%c\n", i + 1, j + 'A');
                    }
                    if (j <= 6 && i >= 1 && validMovepiece(grid, i, j, TOPRIGHT))
                        white = 1;
                    else if (j <= 5 && i >= 2 && validTakepiece(grid, i, j, WHITE, TOPRIGHT))
                    {
                        white = 1;
                        if (turn)
                            printf("ATTACKING PIECE ON %d%c\n", i + 1, j + 'A');
                    }
                }
            }
            else //black
            {
                if (j >= 1 && i >= 1 && validMovepiece(grid, i, j, TOPLEFT))
                    black = 1;
                else if (j >= 2 && i >= 2 && validTakepiece(grid, i, j, BLACK, TOPLEFT))
                {
                    black = 1;
                    if (!turn)
                        printf("ATTACKING PIECE ON %d%c\n", i + 1, j + 'A');
                }
                if (j <= 6 && i >= 1 && validMovepiece(grid, i, j, TOPRIGHT))
                    black = 1;
                else if (j <= 5 && i >= 2 && validTakepiece(grid, i, j, BLACK, TOPRIGHT))
                {
                    black = 1;
                    if (!turn)
                        printf("ATTACKING PIECE ON %d%c\n", i + 1, j + 'A');
                }
                if (grid[i][j].type == 'K')
                {
                    if (j <= 6 && i <= 6 && validMovepiece(grid, i, j, BOTTOMRIGHT))
                        black = 1;
                    else if (j <= 5 && i <= 5 && validTakepiece(grid, i, j, BLACK, BOTTOMRIGHT))
                    {
                        black = 1;
                        if (!turn)
                            printf("ATTACKING PIECE ON %d%c\n", i + 1, j + 'A');
                    }
                    if (j >= 1 && i <= 6 && validMovepiece(grid, i, j, BOTTOMRIGHT))
                        black = 1;
                    else if (j >= 2 && i <= 5 && validTakepiece(grid, i, j, BLACK, BOTTOMLEFT))
                    {
                        black = 1;
                        if (!turn)
                            printf("Attacking piece on %d%c\n", i + 1, j + 'A');
                    }
                }
            }
        }
    }

    if (white == 0)
    {
        printf("BLACK HAS WON THE GAME!\n");
        exit(0);
    }
    else if (black == 0)
    {
        printf("WHITE HAS WON THE GAME!\n");
        exit(0);
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
    case TOPLEFT:
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

    case TOPRIGHT:
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

    case BOTTOMRIGHT:
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

    case BOTTOMLEFT:
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
    case TOPLEFT:
        grid[Y - 1][X - 1].piece = grid[Y][X].piece;
        grid[Y - 1][X - 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                         // Movement complete
        if (grid[Y - 1][X - 1].piece == 'X' && Y == 1) // Checking if can be made king
        {
            grid[Y - 1][X - 1].piece = 'B'; // Turns black normal piece(X) into black king(B)
            grid[Y - 1][X - 1].type = 'K';
            printf("PIECE HAS BEEN PROMOTED TO KING!\n");
        }
        break;

    case TOPRIGHT:
        grid[Y - 1][X + 1].piece = grid[Y][X].piece;
        grid[Y - 1][X + 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                         // Movement complete
        if (grid[Y - 1][X + 1].piece == 'X' && Y == 1) // Checking if can be made king
        {
            grid[Y - 1][X + 1].piece = 'B'; // Turns black normal piece(X) into black king(B)
            grid[Y - 1][X + 1].type = 'K';
            printf("PIECE HAS BEEN PROMOTED TO KING!\n");
        }
        break;

    case BOTTOMRIGHT:
        grid[Y + 1][X + 1].piece = grid[Y][X].piece;
        grid[Y + 1][X + 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';                         // Movement complete
        if (grid[Y + 1][X + 1].piece == 'O' && Y == 6) // Checking if can be made king
        {
            grid[Y + 1][X + 1].piece = 'W'; // Turns white normal piece(O) into white king(W)
            grid[Y + 1][X + 1].type = 'K';
            printf("PIECE HAS BEEN PROMOTED TO KING!\n");
        }
        break;

    case BOTTOMLEFT:
        grid[Y + 1][X - 1].piece = grid[Y][X].piece;
        grid[Y + 1][X - 1].type = grid[Y][X].type;
        grid[Y][X].piece = ' ';
        grid[Y][X].type = ' ';
        if (grid[Y + 1][X - 1].piece == 'O' && Y == 6) // Checking if can be made king
        {
            grid[Y + 1][X - 1].piece = 'W'; // Turns white normal piece(O) into white king(W)
            grid[Y + 1][X - 1].type = 'K';
            printf("PIECE HAS BEEN PROMOTED TO KING!\n");
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
    scanf("%d", &Y); //Scan number first since it will help avoid buffer problems
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
    if (turn) // Checks if white or black as direction of movement is different
    {
        if (X <= 6 && Y <= 6 && validMovepiece(grid, Y, X, BOTTOMRIGHT))
        {
            printf("1) Move bottom right\n");
            inp[1] = 1;
            flag = 1;
        }

        else if (X <= 5 && Y <= 5 && validTakepiece(grid, Y, X, WHITE, BOTTOMRIGHT))
        {
            printf("2) Take piece on bottom right\n");
            inp[2] = 1;
            flag = 1;
        }

        if (X >= 1 && Y <= 6 && validMovepiece(grid, Y, X, BOTTOMLEFT))
        {

            printf("3) Move bottom left\n");
            inp[3] = 1;
            flag = 1;
        }
        else if (X >= 2 && Y <= 5 && validTakepiece(grid, Y, X, WHITE, BOTTOMLEFT))
        {
            printf("4) Take piece on bottom left\n");
            inp[4] = 1;
            flag = 1;
        }
        if (grid[Y][X].type == 'K')
        {
            if (X >= 1 && Y >= 1 && validMovepiece(grid, Y, X, TOPLEFT))
            {
                printf("5) Move top left\n");
                inp[5] = 1;
                flag = 1;
            }
            else if (X >= 2 && Y >= 2 && validTakepiece(grid, Y, X, WHITE, TOPLEFT))
            {
                printf("6) Take piece on top left\n");
                inp[6] = 1;
                flag = 1;
            }
            if (X <= 6 && Y >= 1 && validMovepiece(grid, Y, X, TOPRIGHT))
            {
                printf("7) Move top right\n");
                inp[7] = 1;
                flag = 1;
            }
            else if (X <= 5 && Y >= 2 && validTakepiece(grid, Y, X, WHITE, TOPRIGHT))
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
                movepiece(grid, BOTTOMRIGHT, Y, X);
                break;

            case 2:
                takepiece(grid, BOTTOMRIGHT, Y, X);
                break;

            case 3:
                movepiece(grid, BOTTOMLEFT, Y, X);
                break;

            case 4:
                takepiece(grid, BOTTOMLEFT, Y, X);
                break;

            case 5:
                movepiece(grid, TOPLEFT, Y, X);
                break;

            case 6:
                takepiece(grid, TOPLEFT, Y, X);
                break;

            case 7:
                movepiece(grid, TOPRIGHT, Y, X);
                break;

            case 8:
                takepiece(grid, TOPRIGHT, Y, X);
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
        if (X >= 1 && Y >= 1 && validMovepiece(grid, Y, X, TOPLEFT))
        {
            printf("1) Move top left\n");
            inp[1] = 1;
            flag = 1;
        }
        else if (X >= 2 && Y >= 2 && validTakepiece(grid, Y, X, BLACK, TOPLEFT))
        {
            printf("2) Take piece on top left\n");
            inp[2] = 1;
            flag = 1;
        }
        if (X <= 6 && Y >= 1 && validMovepiece(grid, Y, X, TOPRIGHT))
        {
            printf("3) Move top right\n");
            inp[3] = 1;
            flag = 1;
        }
        else if (X <= 5 && Y >= 2 && validTakepiece(grid, Y, X, BLACK, TOPRIGHT))
        {
            printf("4) Take piece on top right\n");
            inp[4] = 1;
            flag = 1;
        }

        if (grid[Y][X].type == 'K')
        {
            if (X <= 6 && Y <= 6 && validMovepiece(grid, Y, X, BOTTOMRIGHT))
            {
                printf("5) Move bottom right\n");
                inp[5] = 1;
                flag = 1;
            }

            else if (X <= 5 && Y <= 5 && validTakepiece(grid, Y, X, BLACK, BOTTOMRIGHT))
            {
                printf("6) Take piece on bottom right\n");
                inp[6] = 1;
                flag = 1;
            }
            if (X >= 1 && Y <= 6 && validMovepiece(grid, Y, X, BOTTOMRIGHT))
            {
                printf("7) Move bottom left\n");
                inp[7] = 1;
                flag = 1;
            }
            else if (X >= 2 && Y <= 5 && validTakepiece(grid, Y, X, BLACK, BOTTOMLEFT))
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
                movepiece(grid, TOPLEFT, Y, X);
                break;

            case 2:
                takepiece(grid, TOPLEFT, Y, X);
                break;

            case 3:
                movepiece(grid, TOPRIGHT, Y, X);
                break;

            case 4:
                takepiece(grid, TOPRIGHT, Y, X);
                break;
            case 5:
                movepiece(grid, BOTTOMRIGHT, Y, X);
                break;

            case 6:
                takepiece(grid, BOTTOMRIGHT, Y, X);
                break;

            case 7:
                movepiece(grid, BOTTOMLEFT, Y, X);
                break;

            case 8:
                takepiece(grid, BOTTOMLEFT, Y, X);
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

void NextKMoves(Cell grid[8][8], int k){
    
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
            exit(0);
        }
        else if (noofwhitepieces == 0)
        {
            printf("BLACK HAS WON THE GAME!\n");
            exit(0);
        }
        if (turn)
            printf("WHITE'S TURN\n");
        else
            printf("BLACK'S TURN\n");

        checkallpieces(grid, turn);

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