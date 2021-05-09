#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (960)
#define WINDOW_HEIGHT (960)

SDL_Window *Window;
SDL_Renderer *Rend;
SDL_Surface *Surface;
SDL_Texture *Piece;
SDL_Texture *Bg;
SDL_Texture *Black_tex;
SDL_Texture *White_tex;
SDL_Texture *Nothing;
SDL_Texture *Victory;
SDL_Rect Moves[4];
SDL_Rect Blacks[12];
SDL_Rect Whites[12];

char blackPiece[30] = "resources/pieces/";
char whitePiece[30] = "resources/pieces/";

int turn = 0;

int noOfBlacks = 12;
int noOfWhites = 12;

int grid[9];

struct Pos
{
    int x;
    int y;
    int color;
    int pieceNo;
    int King;
} pos[8][8];

struct State
{
    struct Pos pos[8][8];
    struct State *next;
    struct State *prev;
    int noOfBlacks;
    int noOfWhites;
    int turn;
};

typedef struct State *state;

state front = NULL, back = NULL;   // Will act as a doubly linked list used as a stack for review and undo
state kfront = NULL, kback = NULL; // Will store the next k moves as a doubly linked list

void addState() // Takes the current state of the board, allocates memory and stores it in the stack
{               // Acts as the push function of states
    int i, j;
    state temp = (state)malloc(sizeof(struct State));
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            temp->pos[i][j].x = pos[i][j].x;
            temp->pos[i][j].y = pos[i][j].y;
            temp->pos[i][j].color = pos[i][j].color;
            temp->pos[i][j].pieceNo = pos[i][j].pieceNo;
            temp->pos[i][j].King = pos[i][j].King;
        }
    }
    temp->noOfBlacks = noOfBlacks;
    temp->noOfWhites = noOfWhites;
    temp->turn = turn;
    temp->next = NULL;
    temp->prev = NULL;
    if (front == back && back == NULL)
    {
        front = temp;
        back = temp;
    }
    else if (front == back)
    {
        front->next = temp;
        temp->prev = front;
        back = temp;
    }
    else
    {
        temp->prev = back;
        back->next = temp;
        back = temp;
    }
}

void removeState() // Restores the state of the board to a previous state
{                  // Acts as the pop function of states
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            pos[i][j].color = back->pos[i][j].color;
            pos[i][j].pieceNo = back->pos[i][j].pieceNo;
            pos[i][j].x = back->pos[i][j].x;
            pos[i][j].y = back->pos[i][j].y;
            pos[i][j].King = back->pos[i][j].King;
            noOfWhites = back->noOfWhites;
            noOfBlacks = back->noOfBlacks;
            turn = back->turn;
            if (pos[i][j].color != -1)
            {
                if (pos[i][j].color == 0)
                {
                    Blacks[pos[i][j].pieceNo].x = pos[i][j].x - Blacks[pos[i][j].pieceNo].w / 2;
                    Blacks[pos[i][j].pieceNo].y = pos[i][j].y - Blacks[pos[i][j].pieceNo].h / 2;
                }
                else
                {
                    Whites[pos[i][j].pieceNo].x = pos[i][j].x - Whites[pos[i][j].pieceNo].w / 2;
                    Whites[pos[i][j].pieceNo].y = pos[i][j].y - Whites[pos[i][j].pieceNo].h / 2;
                }
            }
        }
    }
    state temp = back;
    if (back == front)
    {
        back = NULL;
        front = NULL;
    }
    else
    {
        back = back->prev;
    }
    // Free Memory
    free(temp);
}

void uswapPos(struct Pos *a, struct Pos *b)
{ //Swaps the non position related information between positions
    int temp;
    temp = a->color;
    a->color = b->color;
    b->color = temp;
    temp = a->pieceNo;
    a->pieceNo = b->pieceNo;
    b->pieceNo = temp;
    temp = a->King;
    a->King = b->King;
    b->King = temp;
}

void init() // Initializes all the main SDL objects that will be used as well as the board
{
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    Window = SDL_CreateWindow("Game Board",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!Window)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // create a renderer, which sets up the graphics hardware
    Rend = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    if (!Rend)
    {
        printf("error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // Precomputes the locations on the board for easy access and use
    int inc = WINDOW_HEIGHT / 8;
    int i, j;

    for (i = 0; i < 9; i++)
    {
        grid[i] = i * inc;
    }

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            pos[i][j].x = (grid[j] + grid[j + 1]) / 2;
            pos[i][j].y = (grid[i] + grid[i + 1]) / 2;
        }
    }
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            pos[i][j].color = -1;
            pos[i][j].King = -1;
            pos[i][j].pieceNo = -1;
        }
    }
}

void makeBG() // Sets up the background
{
    // load the image into memory using SDL_image library function
    Surface = IMG_Load("resources/board.png");
    if (!Surface)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // load the image data into the graphics hardware's memory
    Bg = SDL_CreateTextureFromSurface(Rend, Surface);
    SDL_FreeSurface(Surface);
    if (!Bg)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }
}

void makeMoves() // Sets up make move pieces used to find which moves can be made
{
    int i;
    // load the image into memory using SDL_image library function
    Surface = IMG_Load("resources/nothing.png");
    if (!Surface)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // load the image data into the graphics hardware's memory
    Nothing = SDL_CreateTextureFromSurface(Rend, Surface);
    SDL_FreeSurface(Surface);
    if (!Nothing)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    for (i = 0; i < 4; i++)
    {
        SDL_QueryTexture(Nothing, NULL, NULL, &Moves[i].w, &Moves[i].h);
        Moves[i].w = WINDOW_WIDTH / 9;
        Moves[i].h = WINDOW_HEIGHT / 9;
    }
}

void makeBlack() // Initializes the black piece setup with positions
{
    Surface = IMG_Load(blackPiece);
    int i;
    if (!Surface)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // load black piece image into memory
    Black_tex = SDL_CreateTextureFromSurface(Rend, Surface);
    SDL_FreeSurface(Surface);
    if (!Black_tex)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // load image of black piece onto each piece
    for (i = 0; i < 12; i++)
    {
        SDL_QueryTexture(Black_tex, NULL, NULL, &Blacks[i].w, &Blacks[i].h);
        Blacks[i].w = WINDOW_WIDTH / 9;
        Blacks[i].h = WINDOW_HEIGHT / 9;
    }

    // calculate initial position of each black piece and assign it
    for (i = 0; i < 4; i++)
    {
        Blacks[i].x = pos[7][i * 2].x - Blacks[i].w / 2;
        Blacks[i].y = pos[7][i].y - Blacks[i].h / 2;
        Blacks[i + 8].x = pos[5][i * 2].x - Blacks[i].w / 2;
        Blacks[i + 8].y = pos[5][i].y - Blacks[i].h / 2;
        Blacks[i + 4].x = pos[6][1 + i * 2].x - Blacks[i].w / 2;
        Blacks[i + 4].y = pos[6][i].y - Blacks[i].h / 2;
        pos[7][i * 2].color = 0;
        pos[6][1 + i * 2].color = 0;
        pos[5][i * 2].color = 0;
        pos[7][i * 2].pieceNo = i;
        pos[6][1 + i * 2].pieceNo = i + 4;
        pos[5][i * 2].pieceNo = i + 8;
    }
}

void makeWhite() // Initializes the White piece setup with positions
{
    Surface = IMG_Load(whitePiece);
    int i;
    if (!Surface)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // load black piece image into memory
    White_tex = SDL_CreateTextureFromSurface(Rend, Surface);
    SDL_FreeSurface(Surface);
    if (!White_tex)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // load image of white piece onto each piece
    for (i = 0; i < 12; i++)
    {
        SDL_QueryTexture(White_tex, NULL, NULL, &Whites[i].w, &Whites[i].h);
        Whites[i].w = WINDOW_WIDTH / 9;
        Whites[i].h = WINDOW_HEIGHT / 9;
    }

    // calculate initial position of each white piece and assign it
    for (i = 0; i < 4; i++)
    {
        Whites[i].x = pos[0][1 + i * 2].x - Whites[i].w / 2;
        Whites[i].y = pos[0][i].y - Whites[i].h / 2;
        Whites[i + 8].x = pos[2][1 + i * 2].x - Whites[i].w / 2;
        Whites[i + 8].y = pos[2][i].y - Whites[i].h / 2;
        Whites[i + 4].x = pos[1][i * 2].x - Whites[i].w / 2;
        Whites[i + 4].y = pos[1][i].y - Whites[i].h / 2;
        pos[0][1 + i * 2].color = 1;
        pos[1][i * 2].color = 1;
        pos[2][1 + i * 2].color = 1;
        pos[0][1 + i * 2].pieceNo = i;
        pos[1][i * 2].pieceNo = i + 4;
        pos[2][1 + i * 2].pieceNo = i + 8;
    }
}

int bs(int element)
{
    // Basic Binary Search of ranges
    // Used to efficiently find which row or column the mouse is in
    int beg = 0, end = 7;
    int mid = (beg + end) / 2;

    while (beg <= end)
    {
        if (element > grid[mid] && element < grid[mid + 1])
            return mid;

        else if (element < grid[mid])
            end = mid;

        else if (element > grid[mid + 1])
            beg = mid + 1;

        mid = (beg + end) / 2;
    }
    return mid;
}

void select(int mouse_x, int mouse_y, int *select_x, int *select_y)
{
    // Based on mouse position, assigns select the value of the row and column the mouse is currently in
    *select_x = bs(mouse_x);
    *select_y = bs(mouse_y);
}

void displayTint(int pieceNo, int color)
{
    // Displays the board along with a tint to indicate who's turn it is
    int i;
    // Clear the Window
    SDL_RenderClear(Rend);
    // Write all image to the window
    if (turn == 0)
        SDL_SetTextureColorMod(Bg, 74, 86, 157);
    else if (turn == 1)
        SDL_SetTextureColorMod(Bg, 220, 36, 36);
    SDL_RenderCopy(Rend, Bg, NULL, NULL);
    SDL_SetTextureColorMod(Bg, 255, 255, 255);

    switch (color)
    {
    case 0: // Based on the color, it will render some parts first and others later for animation purposes
        for (i = 0; i < noOfBlacks; i++)
        {
            if (i == pieceNo)
                continue;
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        }
        for (i = 0; i < noOfWhites; i++)
        {
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
        SDL_SetTextureColorMod(Black_tex, 52, 231, 238);
        SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[pieceNo]);
        SDL_SetTextureColorMod(Black_tex, 255, 255, 255);
        break;
    case 1:
        for (i = 0; i < noOfWhites; i++)
        {
            if (i == pieceNo)
                continue;
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
        for (i = 0; i < noOfBlacks; i++)
        {
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        }
        SDL_SetTextureColorMod(White_tex, 52, 231, 238);
        SDL_RenderCopy(Rend, White_tex, NULL, &Whites[pieceNo]);
        SDL_SetTextureColorMod(White_tex, 255, 255, 255);
        break;
    default: // If used normally, the complexity can be decreased
        for (i = 0; i < noOfWhites; i++)
        {
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
        for (i = 0; i < noOfBlacks; i++)
        {
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        }
    }
    // Upload Render to the Window
    SDL_RenderPresent(Rend);
}

int validMove(int x, int y, int select_x, int select_y)
{
    // Most basic function in checking if a move is possible
    // Checking all take piece moves
    if (x - select_x == 2 && y - select_y == 2 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (pos[y - 1][x - 1].color != turn && pos[y - 1][x - 1].color != -1 && pos[select_y][select_x].pieceNo == -1 && ((pos[y][x].color == 0) || (pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (select_x - x == 2 && y - select_y == 2 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (pos[y - 1][x + 1].color != turn && pos[y - 1][x + 1].color != -1 && pos[select_y][select_x].pieceNo == -1 && ((pos[y][x].color == 0) || (pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (x - select_x == 2 && select_y - y == 2 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (pos[y + 1][x - 1].color != turn && pos[y + 1][x - 1].color != -1 && pos[select_y][select_x].pieceNo == -1 && ((pos[y][x].color == 1) || (pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (select_x - x == 2 && select_y - y == 2 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (pos[y + 1][x + 1].color != turn && pos[y + 1][x + 1].color != -1 && pos[select_y][select_x].pieceNo == -1 && ((pos[y][x].color == 1) || (pos[y][x].King == 1)))
        {
            return 1;
        }
    }

    // Checking all normal moves
    if (select_x - x == 1 && y - select_y == 1 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (pos[select_y][select_x].pieceNo == -1 && ((pos[y][x].color == 0) || (pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (x - select_x == 1 && y - select_y == 1 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (pos[select_y][select_x].pieceNo == -1 && ((pos[y][x].color == 0) || (pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (select_x - x == 1 && select_y - y == 1 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (pos[select_y][select_x].pieceNo == -1 && ((pos[y][x].color == 1) || (pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (x - select_x == 1 && select_y - y == 1 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (pos[select_y][select_x].pieceNo == -1 && ((pos[y][x].color == 1) || (pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    return 0;
}

void findValidMoves(int valid[], int x, int y)
{
    // Function to store an array with 1 at positions where movement is possible
    // Uses validMove for its processes
    // Valid[0] - Take piece top left
    // Valid[1] - Move piece top left
    // Valid[2] - Move piece top right
    // Valid[3] - Take piece top right
    // Valid[4] - Take piece bottom left
    // Valid[5] - Move piece bottom left
    // Valid[6] - Move piece bottom right
    // Valid[7] - Take piece bottom right
    int flag = 1, i;

    for (i = 0; i < 8; i++)
    {
        valid[i] = 0;
    }
    if (validMove(x, y, x - 2, y - 2) == 1)
    {
        valid[0] = 1;
        flag = 0;
    }
    if (validMove(x, y, x + 2, y - 2) == 1)
    {
        valid[3] = 1;
        flag = 0;
    }
    if (validMove(x, y, x - 2, y + 2) == 1)
    {
        valid[4] = 1;
        flag = 0;
    }
    if (validMove(x, y, x + 2, y + 2) == 1)
    {
        valid[7] = 1;
        flag = 0;
    }
    if (flag)
    {
        if (validMove(x, y, x - 1, y - 1) == 1)
        {
            valid[1] = 1;
        }
        if (validMove(x, y, x + 1, y - 1) == 1)
        {
            valid[2] = 1;
        }
        if (validMove(x, y, x - 1, y + 1) == 1)
        {
            valid[5] = 1;
        }
        if (validMove(x, y, x + 1, y + 1) == 1)
        {
            valid[6] = 1;
        }
    }
}

int attackMode()
{
    // Checks the state of all pieces and whether an attack is possible
    int x, y, i, total = 0, valid[8] = {0};
    if (turn == 0)
    {
        for (i = 0; i < noOfBlacks; i++) // Checking for jump moves for black pieces
        {
            select(Blacks[i].x, Blacks[i].y, &x, &y);
            findValidMoves(valid, x, y);
            total += valid[0] + valid[3] + valid[4] + valid[7]; // These indexes store if a jump move is possible
        }
    }
    else if (turn == 1)
    {
        for (i = 0; i < noOfWhites; i++) // Checking for jump moves for white pieces
        {
            select(Whites[i].x, Whites[i].y, &x, &y);
            findValidMoves(valid, x, y);
            total += valid[0] + valid[3] + valid[4] + valid[7]; // These indexes store if a jump move is possible
        }
    }
    return total; // The value returned will be non zero if a take piece move can be performed
}

void takePiece(int x, int y)
{
    // Function to display one less piece after a take piece move
    int i, j, flag = 1, check, attack;

    // Check color of the piece to remove and the piece number of the last element in the array to replace with
    if (pos[y][x].color == 0)
    {
        check = noOfBlacks - 1;
        noOfBlacks--;
    }
    else if (pos[y][x].color == 1)
    {
        check = noOfWhites - 1;
        noOfWhites--;
    }

    // Find the last element in the array of SDL_Rect to replace with
    // This is done for GUI purposes
    for (i = 0; i < 8 && flag; i++)
    {
        for (j = 0; j < 8 && flag; j++)
        {
            if (pos[i][j].pieceNo == check && pos[i][j].color == pos[y][x].color)
            {
                flag = 1;
                if (pos[y][x].color == 0)
                {
                    Blacks[pos[y][x].pieceNo].h = Blacks[pos[i][j].pieceNo].h;
                    Blacks[pos[y][x].pieceNo].w = Blacks[pos[i][j].pieceNo].w;
                    Blacks[pos[y][x].pieceNo].x = Blacks[pos[i][j].pieceNo].x;
                    Blacks[pos[y][x].pieceNo].y = Blacks[pos[i][j].pieceNo].y;
                }
                else if (pos[y][x].color == 1)
                {
                    Whites[pos[y][x].pieceNo].h = Whites[pos[i][j].pieceNo].h;
                    Whites[pos[y][x].pieceNo].w = Whites[pos[i][j].pieceNo].w;
                    Whites[pos[y][x].pieceNo].x = Whites[pos[i][j].pieceNo].x;
                    Whites[pos[y][x].pieceNo].y = Whites[pos[i][j].pieceNo].y;
                }
                pos[i][j].pieceNo = pos[y][x].pieceNo;
                pos[y][x].color = -1;
                pos[y][x].King = -1;
                pos[y][x].pieceNo = -1;
            }
        }
    }

    attack = attackMode();
    // Inorder to make another move, set change turns once here and it'll be changed once more in the following function leading to no turn change
    if (turn == 0 && attack != 0)
        turn = 1;
    else if (turn == 1 && attack != 0)
        turn = 0;
}

int movementPossible(int color)
{
    // Victory Condition
    // Checks if any move is possible by any piece of a given color
    int valid[8], i, j, x, y, total = 0;
    if (color == 0)
    {
        for (i = 0; i < noOfBlacks; i++)
        {
            select(Blacks[i].x, Blacks[i].y, &x, &y);
            findValidMoves(valid, x, y);
            for (j = 0; j < 8; j++)
                total += valid[j];
        }
    }
    if (color == 1)
    {
        for (i = 0; i < noOfWhites; i++)
        {
            select(Whites[i].x, Whites[i].y, &x, &y);
            findValidMoves(valid, x, y);
            for (j = 0; j < 8; j++)
                total += valid[j];
        }
    }
    return total;
}

int isItValid(int x, int y, int select_x, int select_y, int attack)
{
    // Checks if a move that is being made is valid
    int validMoves[8] = {0};
    findValidMoves(validMoves, x, y);

    // Checking the jump moves first because of priority
    if (validMoves[0] || validMoves[3] || validMoves[4] || validMoves[7])
    {
        if (validMoves[0] && x - select_x == 2 && y - select_y == 2)
        {
            return 1;
        }
        if (validMoves[3] && select_x - x == 2 && y - select_y == 2)
        {
            return 1;
        }
        if (validMoves[4] && x - select_x == 2 && select_y - y == 2)
        {
            return 1;
        }
        if (validMoves[7] && select_x - x == 2 && select_y - y == 2)
        {
            return 1;
        }
    }

    // Checks the remaining normal moves
    else if (attack == 0)
    {

        if (validMoves[1] && x - select_x == 1 && y - select_y == 1)
        {
            return 1;
        }
        if (validMoves[2] && select_x - x == 1 && y - select_y == 1)
        {
            return 1;
        }
        if (validMoves[5] && x - select_x == 1 && select_y - y == 1)
        {
            return 1;
        }
        if (validMoves[6] && select_x - x == 1 && select_y - y == 1)
        {
            return 1;
        }
    }

    return 0;
}

void displayValidMoves(int pieceNo, int color, int x, int y, int attack)
{
    // Normal display tint function with added functionality to display next possible moves
    int validMoves[8] = {0}, i = 0;

    // Clear the Window
    SDL_RenderClear(Rend);

    // Write all image to the window
    if (turn == 0)
        SDL_SetTextureColorMod(Bg, 74, 86, 157);
    else if (turn == 1)
        SDL_SetTextureColorMod(Bg, 220, 36, 36);
    SDL_RenderCopy(Rend, Bg, NULL, NULL);
    SDL_SetTextureColorMod(Bg, 255, 255, 255);

    switch (color)
    {
    case 0:
        for (i = 0; i < noOfBlacks; i++)
        {
            if (i == pieceNo)
                continue;
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        }
        for (i = 0; i < noOfWhites; i++)
        {
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
        SDL_SetTextureColorMod(Black_tex, 52, 231, 238);
        SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[pieceNo]);
        SDL_SetTextureColorMod(Black_tex, 255, 255, 255);
        break;
    case 1:
        for (i = 0; i < noOfWhites; i++)
        {
            if (i == pieceNo)
                continue;
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
        for (i = 0; i < noOfBlacks; i++)
        {
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        }
        SDL_SetTextureColorMod(White_tex, 52, 231, 238);
        SDL_RenderCopy(Rend, White_tex, NULL, &Whites[pieceNo]);
        SDL_SetTextureColorMod(White_tex, 255, 255, 255);
        break;
    default:
        for (i = 0; i < noOfWhites; i++)
        {
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
        for (i = 0; i < noOfBlacks; i++)
        {
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        }
    }

    i = 0;

    // Finds the valid moves possible from a given position and stores in an array
    findValidMoves(validMoves, x, y);

    // Checks if move is a take piece move and displays accordingly
    if (validMoves[0] || validMoves[3] || validMoves[4] || validMoves[7])
    {
        SDL_SetTextureColorMod(Nothing, 255, 130, 53);
        if (validMoves[0])
        {
            Moves[i].x = pos[y - 2][x - 2].x - Moves[i].w / 2;
            Moves[i].y = pos[y - 2][x - 2].y - Moves[i].h / 2;
            SDL_RenderCopy(Rend, Nothing, NULL, &Moves[i]);
            i++;
        }
        if (validMoves[3])
        {
            Moves[i].x = pos[y - 2][x + 2].x - Moves[i].w / 2;
            Moves[i].y = pos[y - 2][x + 2].y - Moves[i].h / 2;
            SDL_RenderCopy(Rend, Nothing, NULL, &Moves[i]);
            i++;
        }
        if (validMoves[4])
        {
            Moves[i].x = pos[y + 2][x - 2].x - Moves[i].w / 2;
            Moves[i].y = pos[y + 2][x - 2].y - Moves[i].h / 2;
            SDL_RenderCopy(Rend, Nothing, NULL, &Moves[i]);
            i++;
        }
        if (validMoves[7])
        {
            Moves[i].x = pos[y + 2][x + 2].x - Moves[i].w / 2;
            Moves[i].y = pos[y + 2][x + 2].y - Moves[i].h / 2;
            SDL_RenderCopy(Rend, Nothing, NULL, &Moves[i]);
            i++;
        }
        SDL_SetTextureColorMod(Nothing, 255, 255, 255);
    }

    // Checks the remaining normal moves
    else if (attack == 0)
    {
        SDL_SetTextureColorMod(Nothing, 48, 232, 191);
        if (validMoves[1])
        {
            Moves[i].x = pos[y - 1][x - 1].x - Moves[i].w / 2;
            Moves[i].y = pos[y - 1][x - 1].y - Moves[i].h / 2;
            SDL_RenderCopy(Rend, Nothing, NULL, &Moves[i]);
            i++;
        }
        if (validMoves[2])
        {
            Moves[i].x = pos[y - 1][x + 1].x - Moves[i].w / 2;
            Moves[i].y = pos[y - 1][x + 1].y - Moves[i].h / 2;
            SDL_RenderCopy(Rend, Nothing, NULL, &Moves[i]);
            i++;
        }
        if (validMoves[5])
        {
            Moves[i].x = pos[y + 1][x - 1].x - Moves[i].w / 2;
            Moves[i].y = pos[y + 1][x - 1].y - Moves[i].h / 2;
            SDL_RenderCopy(Rend, Nothing, NULL, &Moves[i]);
            i++;
        }
        if (validMoves[6])
        {
            Moves[i].x = pos[y + 1][x + 1].x - Moves[i].w / 2;
            Moves[i].y = pos[y + 1][x + 1].y - Moves[i].h / 2;
            SDL_RenderCopy(Rend, Nothing, NULL, &Moves[i]);
            i++;
        }
        SDL_SetTextureColorMod(Nothing, 255, 255, 255);
    }
    // Upload Render to the Window
    SDL_RenderPresent(Rend);
}

void move(int pieceNo, int color, int x, int y)
{
    // Values to be used for calculating position and relative postion and movement for the pieces
    int x_pos, y_pos, x_final, y_final, distx, disty, currx, curry;

    // Save the previous state
    addState();

    // Chooses which Color Piece to move
    if (color == 0)
    {
        select(Blacks[pieceNo].x, Blacks[pieceNo].y, &currx, &curry);
        x_pos = Blacks[pieceNo].x;
        y_pos = Blacks[pieceNo].y;
        x_final = pos[y][x].x - Blacks[pieceNo].w / 2;
        y_final = pos[y][x].y - Blacks[pieceNo].h / 2;

        distx = (x_final - x_pos) / 60; // Amount to move by in x direction
        disty = (y_final - y_pos) / 60; // Amount to move by in y direction

        while (x_pos != x_final || y_pos != y_final)
        {
            x_pos += distx;
            y_pos += disty;

            Blacks[pieceNo].x = x_pos;
            Blacks[pieceNo].y = y_pos;
            displayTint(pieceNo, 0);
            SDL_Delay(500 / 60);
        }
        uswapPos(&pos[curry][currx], &pos[y][x]);
        if (currx - x == 2 && curry - y == 2)
            takePiece(x + 1, y + 1);
        else if (x - currx == 2 && curry - y == 2)
            takePiece(x - 1, y + 1);
        else if (currx - x == 2 && y - curry == 2)
            takePiece(x + 1, y - 1);
        else if (x - currx == 2 && y - curry == 2)
            takePiece(x - 1, y - 1);
        if (y == 0)
        {
            pos[y][x].King = 1;
            if (attackMode() != 0)
            {
                turn = 0;
            }
        }
    }
    else if (color == 1)
    {
        select(Whites[pieceNo].x, Whites[pieceNo].y, &currx, &curry);
        x_pos = Whites[pieceNo].x;
        y_pos = Whites[pieceNo].y;
        x_final = pos[y][x].x - Whites[pieceNo].w / 2;
        y_final = pos[y][x].y - Whites[pieceNo].h / 2;

        distx = (x_final - x_pos) / 60; // Amount to move by in x direction
        disty = (y_final - y_pos) / 60; // Amount to move by in y direction

        while (x_pos != x_final || y_pos != y_final)
        {
            x_pos += distx;
            y_pos += disty;
            Whites[pieceNo].x = x_pos;
            Whites[pieceNo].y = y_pos;
            displayTint(pieceNo, 1);
            SDL_Delay(500 / 60);
        }
        uswapPos(&pos[curry][currx], &pos[y][x]);
        if (currx - x == 2 && curry - y == 2)
            takePiece(x + 1, y + 1);
        else if (x - currx == 2 && curry - y == 2)
            takePiece(x - 1, y + 1);
        else if (currx - x == 2 && y - curry == 2)
            takePiece(x + 1, y - 1);
        else if (x - currx == 2 && y - curry == 2)
            takePiece(x - 1, y - 1);
        if (y == 7)
        {
            pos[y][x].King = 1;
            if (attackMode() != 0)
            {
                turn = 1;
            }
        }
    }
}

void destroy()
{
    // Destroy all textures and renderers and windows
    SDL_DestroyTexture(Bg);
    SDL_DestroyTexture(Black_tex);
    SDL_DestroyTexture(White_tex);
    SDL_DestroyTexture(Nothing);
    SDL_DestroyTexture(Victory);

    SDL_DestroyRenderer(Rend);
    SDL_DestroyWindow(Window);

    SDL_Quit();
}

void makemove(int x, int y)
{
    // UI function that is accessed after selecting a piece
    int selection = 0, select_x = -1, select_y = -1, attack;
    attack = attackMode();
    while (!selection)
    {
        // Checking events
        SDL_Event event;
        while (SDL_PollEvent(&event)) // If there is an event, it is then assigned to event and becomes true
        {
            switch (event.type)
            {
            case SDL_QUIT: // If the event was a quit window event
                destroy();
                exit(0);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {                                                                 // Only true if the mouse is in the window and left button is clicked
                    select(event.button.x, event.button.y, &select_x, &select_y); // Assigns the row and column position mouse is currently in
                    if (select_x == x && select_y == y)
                    {
                        selection = 1;
                    }
                    else if (pos[select_y][select_x].color == -1 && isItValid(x, y, select_x, select_y, attack))
                    {
                        move(pos[y][x].pieceNo, pos[y][x].color, select_x, select_y);
                        if (turn == 0)
                            turn = 1;
                        else if (turn == 1)
                            turn = 0;
                        selection = 1;
                    }
                }
                break;
            }
        }
        displayValidMoves(pos[y][x].pieceNo, pos[y][x].color, x, y, attack);
        // Refreshes the display
    }
}

void victoryDisplay(int color)
{
    // Function to display the victor after the condition for victory is attained
    SDL_RenderClear(Rend);
    // Write all image to the window
    // load the image into memory using SDL_image library function
    if (color == 0)
    {
        Surface = IMG_Load("resources/black_victory.png");
    }
    else if (color == 1)
    {
        Surface = IMG_Load("resources/white_victory.png");
    }
    else if (color == 2)
    {
        Surface = IMG_Load("resources/draw.png");
    }
    if (!Surface)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // load the image data into the graphics hardware's memory
    Victory = SDL_CreateTextureFromSurface(Rend, Surface);
    SDL_FreeSurface(Surface);
    if (!Victory)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // Copy the Victory picture to the Render screen
    SDL_RenderCopy(Rend, Victory, NULL, NULL);

    // Upload Render to the Window
    SDL_RenderPresent(Rend);
}

void setPos(state curr)
{
    // Takes a state of the baord stored in memory and pushes it to the global displayed board variable
    int i, j;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            pos[i][j].color = curr->pos[i][j].color;
            pos[i][j].pieceNo = curr->pos[i][j].pieceNo;
            pos[i][j].x = curr->pos[i][j].x;
            pos[i][j].y = curr->pos[i][j].y;
            pos[i][j].King = curr->pos[i][j].King;
            noOfWhites = curr->noOfWhites;
            noOfBlacks = curr->noOfBlacks;
            turn = curr->turn;
            if (pos[i][j].color != -1)
            {
                if (pos[i][j].color == 0)
                {
                    Blacks[pos[i][j].pieceNo].x = pos[i][j].x - Blacks[pos[i][j].pieceNo].w / 2;
                    Blacks[pos[i][j].pieceNo].y = pos[i][j].y - Blacks[pos[i][j].pieceNo].h / 2;
                }
                else
                {
                    Whites[pos[i][j].pieceNo].x = pos[i][j].x - Whites[pos[i][j].pieceNo].w / 2;
                    Whites[pos[i][j].pieceNo].y = pos[i][j].y - Whites[pos[i][j].pieceNo].h / 2;
                }
            }
        }
    }
}

int validMoveK(int x, int y, int select_x, int select_y, state curr)
{
    // Modified function of normal validMove function but for k moves
    // Most basic function in checking if a move is possible
    // Checking all take piece moves
    if (x - select_x == 2 && y - select_y == 2 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (curr->pos[y - 1][x - 1].color != curr->turn && curr->pos[y - 1][x - 1].color != -1 && curr->pos[select_y][select_x].pieceNo == -1 && ((curr->pos[y][x].color == 0) || (curr->pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (select_x - x == 2 && y - select_y == 2 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (curr->pos[y - 1][x + 1].color != curr->turn && curr->pos[y - 1][x + 1].color != -1 && curr->pos[select_y][select_x].pieceNo == -1 && ((curr->pos[y][x].color == 0) || (curr->pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (x - select_x == 2 && select_y - y == 2 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (curr->pos[y + 1][x - 1].color != curr->turn && curr->pos[y + 1][x - 1].color != -1 && curr->pos[select_y][select_x].pieceNo == -1 && ((curr->pos[y][x].color == 1) || (curr->pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (select_x - x == 2 && select_y - y == 2 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (curr->pos[y + 1][x + 1].color != curr->turn && curr->pos[y + 1][x + 1].color != -1 && curr->pos[select_y][select_x].pieceNo == -1 && ((curr->pos[y][x].color == 1) || (curr->pos[y][x].King == 1)))
        {
            return 1;
        }
    }

    // Checking all normal moves
    if (select_x - x == 1 && y - select_y == 1 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (curr->pos[select_y][select_x].pieceNo == -1 && ((curr->pos[y][x].color == 0) || (curr->pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (x - select_x == 1 && y - select_y == 1 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (curr->pos[select_y][select_x].pieceNo == -1 && ((curr->pos[y][x].color == 0) || (curr->pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (select_x - x == 1 && select_y - y == 1 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (curr->pos[select_y][select_x].pieceNo == -1 && ((curr->pos[y][x].color == 1) || (curr->pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    if (x - select_x == 1 && select_y - y == 1 && select_y > -1 && select_y < 8 && select_x > -1 && select_x < 8)
    {
        if (curr->pos[select_y][select_x].pieceNo == -1 && ((curr->pos[y][x].color == 1) || (curr->pos[y][x].King == 1)))
        {
            return 1;
        }
    }
    return 0;
}

void findValidMovesK(int valid[], int x, int y, state curr)
{
    // Modified function of normal findValidMoves function but for k moves
    // Function to store an array with 1 at positions where movement is possible
    // Valid[0] - Take piece top left
    // Valid[1] - Move piece top left
    // Valid[2] - Move piece top right
    // Valid[3] - Take piece top right
    // Valid[4] - Take piece bottom left
    // Valid[5] - Move piece bottom left
    // Valid[6] - Move piece bottom right
    // Valid[7] - Take piece bottom right
    int flag = 1, i;

    for (i = 0; i < 8; i++)
    {
        valid[i] = 0;
    }
    if (validMoveK(x, y, x - 2, y - 2, curr) == 1)
    {
        valid[0] = 1;
        flag = 0;
    }
    if (validMoveK(x, y, x + 2, y - 2, curr) == 1)
    {
        valid[3] = 1;
        flag = 0;
    }
    if (validMoveK(x, y, x - 2, y + 2, curr) == 1)
    {
        valid[4] = 1;
        flag = 0;
    }
    if (validMoveK(x, y, x + 2, y + 2, curr) == 1)
    {
        valid[7] = 1;
        flag = 0;
    }
    if (flag)
    {
        if (validMoveK(x, y, x - 1, y - 1, curr) == 1)
        {
            valid[1] = 1;
        }
        if (validMoveK(x, y, x + 1, y - 1, curr) == 1)
        {
            valid[2] = 1;
        }
        if (validMoveK(x, y, x - 1, y + 1, curr) == 1)
        {
            valid[5] = 1;
        }
        if (validMoveK(x, y, x + 1, y + 1, curr) == 1)
        {
            valid[6] = 1;
        }
    }
}

int attackModeK(state curr)
{
    // Modified function of normal attackMode function but for k moves
    // Checks the state of all pieces and whether an attack is possible
    int i, j, total = 0, valid[8] = {0};
    if (curr->turn == 0)
    {
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (curr->pos[i][j].color == curr->turn)
                {
                    findValidMovesK(valid, j, i, curr);
                    total += valid[0] + valid[3] + valid[4] + valid[7];
                }
            }
        }
    }
    else if (curr->turn == 1)
    {
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (curr->pos[i][j].color == curr->turn)
                {
                    findValidMovesK(valid, j, i, curr);
                    total += valid[0] + valid[3] + valid[4] + valid[7];
                }
            }
        }
    }
    return total;
}

void copyPos(state curr, state to)
{
    // Copies one state stored in memory to another
    int i, j;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            to->pos[i][j].color = curr->pos[i][j].color;
            to->pos[i][j].pieceNo = curr->pos[i][j].pieceNo;
            to->pos[i][j].x = curr->pos[i][j].x;
            to->pos[i][j].y = curr->pos[i][j].y;
            to->pos[i][j].King = curr->pos[i][j].King;
            to->noOfWhites = curr->noOfWhites;
            to->noOfBlacks = curr->noOfBlacks;
            to->turn = curr->turn;
        }
    }
}

void review()
{
    // UI Function to go through the previous states of the game
    int quit = 0;
    state curr = front;

    setPos(front);

    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) // If there is an event, it is then assigned to event and becomes true
        {
            switch (event.type)
            {
            case SDL_QUIT: // If the event was a quit window event
                destroy();
                exit(0);
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_x)
                {
                    quit = 1;
                }
                else if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_h) && curr != front)
                {
                    curr = curr->prev;
                    setPos(curr);
                }
                else if ((event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_l) && curr != back)
                {
                    curr = curr->next;
                    setPos(curr);
                }
                break;
            }
        }
        displayTint(-1, -1); // Refreshes the display
    }
}

void deletek()
{
    // Deletes the doubly linked list created by createkq for next k moves
    state temp = kfront;
    while (temp != NULL)
    {
        if (kfront != kback)
        {
            kfront = kfront->next;
            kfront->prev = NULL;
            free(temp);
        }
        else if (kfront == kback && kfront != NULL)
        {
            kfront = NULL;
            kback = NULL;
            free(temp);
        }
        temp = kfront;
    }
}

void createkq(state curr, int k)
{
    // Recursive function meant to go through k states or moves and finally add them to the doubly linked list
    if (k == 0)
    {
        state store = (state)malloc(sizeof(struct State));
        copyPos(curr, store);
        if (kfront == kback && kfront == NULL)
        {
            kfront = store;
            kback = store;
        }
        else if (kfront == kback)
        {
            kfront->next = store;
            store->prev = kfront;
            kback = store;
        }
        else
        {
            kback->next = store;
            store->prev = kback;
            kback = store;
        }
    }
    else
    {
        int attack = attackModeK(curr), i, j, valid[8] = {0}, p, q, flag, check, kingrow = 0;
        if (attack != 0) // Checks if an attack is possible, then goes into those states
        {
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (curr->pos[j][i].color == curr->turn)
                    {
                        findValidMovesK(valid, i, j, curr);
                        // Checks the if an attack can be made in each direction, If true, then executes that move and calls recursively
                        if (valid[0] == 1)
                        {
                            state nextMove = (state)malloc(sizeof(struct State));
                            copyPos(curr, nextMove);
                            nextMove->pos[j - 2][i - 2].color = nextMove->pos[j][i].color;
                            nextMove->pos[j - 2][i - 2].King = nextMove->pos[j][i].King;
                            nextMove->pos[j - 2][i - 2].pieceNo = nextMove->pos[j][i].pieceNo;
                            if (nextMove->pos[j - 2][i - 2].color == 0 && j - 2 == 0 && nextMove->pos[j - 2][i - 2].King == 0)
                            {
                                nextMove->pos[j - 2][i - 2].King = 1;
                                kingrow = 1;
                            }
                            nextMove->pos[j][i].color = -1;
                            nextMove->pos[j][i].King = -1;
                            nextMove->pos[j][i].pieceNo = -1;
                            if (nextMove->turn == 0)
                            {
                                nextMove->noOfWhites--;
                                check = nextMove->noOfWhites;
                                nextMove->turn = 1;
                            }
                            else if (nextMove->turn == 1)
                            {
                                nextMove->noOfBlacks--;
                                check = nextMove->noOfBlacks;
                                nextMove->turn = 0;
                            }
                            flag = 1;

                            for (p = 0; p < 8 && flag == 1; p++)
                            {
                                for (q = 0; q < 8 && flag == 1; q++)
                                {
                                    if (nextMove->pos[q][p].color == nextMove->turn && nextMove->pos[q][p].pieceNo == check)
                                    {
                                        nextMove->pos[q][p].pieceNo = nextMove->pos[j - 1][i - 1].pieceNo;
                                        nextMove->pos[j - 1][i - 1].color = -1;
                                        nextMove->pos[j - 1][i - 1].King = -1;
                                        nextMove->pos[j - 1][i - 1].pieceNo = -1;
                                        flag = 0;
                                    }
                                }
                            }

                            state doubleCheck = (state)malloc(sizeof(struct State));
                            copyPos(nextMove, doubleCheck);
                            if (doubleCheck->turn == 0)
                            {
                                doubleCheck->turn = 1;
                            }
                            else if (doubleCheck->turn == 1)
                            {
                                doubleCheck->turn = 0;
                            }

                            if (attackModeK(doubleCheck) != 0 && kingrow == 0)
                            {
                                nextMove->turn = doubleCheck->turn;
                            }

                            free(doubleCheck);
                            kingrow = 0;

                            createkq(nextMove, k - 1);
                            free(nextMove);
                        }
                        if (valid[3] == 1)
                        {
                            state nextMove = (state)malloc(sizeof(struct State));
                            copyPos(curr, nextMove);
                            nextMove->pos[j - 2][i + 2].color = nextMove->pos[j][i].color;
                            nextMove->pos[j - 2][i + 2].King = nextMove->pos[j][i].King;
                            nextMove->pos[j - 2][i + 2].pieceNo = nextMove->pos[j][i].pieceNo;
                            if (nextMove->pos[j - 2][i + 2].color == 0 && j - 2 == 0 && nextMove->pos[j - 2][i + 2].King == 0)
                            {
                                nextMove->pos[j - 2][i + 2].King = 1;
                                kingrow = 1;
                            }
                            nextMove->pos[j][i].color = -1;
                            nextMove->pos[j][i].King = -1;
                            nextMove->pos[j][i].pieceNo = -1;
                            if (nextMove->turn == 0)
                            {
                                nextMove->noOfWhites--;
                                check = nextMove->noOfWhites;
                                nextMove->turn = 1;
                            }
                            else if (nextMove->turn == 1)
                            {
                                nextMove->noOfBlacks--;
                                check = nextMove->noOfBlacks;
                                nextMove->turn = 0;
                            }
                            flag = 1;

                            for (p = 0; p < 8 && flag == 1; p++)
                            {
                                for (q = 0; q < 8 && flag == 1; q++)
                                {
                                    if (nextMove->pos[q][p].color == nextMove->turn && nextMove->pos[q][p].pieceNo == check)
                                    {
                                        nextMove->pos[q][p].pieceNo = nextMove->pos[j - 1][i + 1].pieceNo;
                                        nextMove->pos[j - 1][i + 1].color = -1;
                                        nextMove->pos[j - 1][i + 1].King = -1;
                                        nextMove->pos[j - 1][i + 1].pieceNo = -1;
                                        flag = 0;
                                    }
                                }
                            }

                            state doubleCheck = (state)malloc(sizeof(struct State));
                            copyPos(nextMove, doubleCheck);
                            if (doubleCheck->turn == 0)
                            {
                                doubleCheck->turn = 1;
                            }
                            else if (doubleCheck->turn == 1)
                            {
                                doubleCheck->turn = 0;
                            }

                            if (attackModeK(doubleCheck) != 0 && kingrow == 0)
                            {
                                nextMove->turn = doubleCheck->turn;
                            }

                            free(doubleCheck);
                            kingrow = 0;

                            createkq(nextMove, k - 1);
                            free(nextMove);
                        }
                        if (valid[4] == 1)
                        {
                            state nextMove = (state)malloc(sizeof(struct State));
                            copyPos(curr, nextMove);
                            nextMove->pos[j + 2][i - 2].color = nextMove->pos[j][i].color;
                            nextMove->pos[j + 2][i - 2].King = nextMove->pos[j][i].King;
                            nextMove->pos[j + 2][i - 2].pieceNo = nextMove->pos[j][i].pieceNo;
                            if (nextMove->pos[j + 2][i - 2].color == 1 && j + 2 == 7 && nextMove->pos[j + 2][i - 2].King == 0)
                            {
                                nextMove->pos[j + 2][i - 2].King = 1;
                                kingrow = 1;
                            }
                            nextMove->pos[j][i].color = -1;
                            nextMove->pos[j][i].King = -1;
                            nextMove->pos[j][i].pieceNo = -1;
                            if (nextMove->turn == 0)
                            {
                                nextMove->noOfWhites--;
                                check = nextMove->noOfWhites;
                                nextMove->turn = 1;
                            }
                            else if (nextMove->turn == 1)
                            {
                                nextMove->noOfBlacks--;
                                check = nextMove->noOfBlacks;
                                nextMove->turn = 0;
                            }
                            flag = 1;

                            for (p = 0; p < 8 && flag == 1; p++)
                            {
                                for (q = 0; q < 8 && flag == 1; q++)
                                {
                                    if (nextMove->pos[q][p].color == nextMove->turn && nextMove->pos[q][p].pieceNo == check)
                                    {
                                        nextMove->pos[q][p].pieceNo = nextMove->pos[j + 1][i - 1].pieceNo;
                                        nextMove->pos[j + 1][i - 1].color = -1;
                                        nextMove->pos[j + 1][i - 1].King = -1;
                                        nextMove->pos[j + 1][i - 1].pieceNo = -1;
                                        flag = 0;
                                    }
                                }
                            }

                            state doubleCheck = (state)malloc(sizeof(struct State));
                            copyPos(nextMove, doubleCheck);
                            if (doubleCheck->turn == 0)
                            {
                                doubleCheck->turn = 1;
                            }
                            else if (doubleCheck->turn == 1)
                            {
                                doubleCheck->turn = 0;
                            }

                            if (attackModeK(doubleCheck) != 0 && kingrow == 0)
                            {
                                nextMove->turn = doubleCheck->turn;
                            }

                            free(doubleCheck);
                            kingrow = 0;

                            createkq(nextMove, k - 1);
                            free(nextMove);
                        }
                        if (valid[7] == 1)
                        {
                            state nextMove = (state)malloc(sizeof(struct State));
                            copyPos(curr, nextMove);
                            nextMove->pos[j + 2][i + 2].color = nextMove->pos[j][i].color;
                            nextMove->pos[j + 2][i + 2].King = nextMove->pos[j][i].King;
                            nextMove->pos[j + 2][i + 2].pieceNo = nextMove->pos[j][i].pieceNo;
                            if (nextMove->pos[j + 2][i + 2].color == 1 && j + 2 == 7 && nextMove->pos[j + 2][i + 2].King == 0)
                            {
                                nextMove->pos[j + 2][i + 2].King = 1;
                            }
                            nextMove->pos[j][i].color = -1;
                            nextMove->pos[j][i].King = -1;
                            nextMove->pos[j][i].pieceNo = -1;
                            if (nextMove->turn == 0)
                            {
                                nextMove->noOfWhites--;
                                check = nextMove->noOfWhites;
                                nextMove->turn = 1;
                            }
                            else if (nextMove->turn == 1)
                            {
                                nextMove->noOfBlacks--;
                                check = nextMove->noOfBlacks;
                                nextMove->turn = 0;
                            }
                            flag = 1;

                            for (p = 0; p < 8 && flag == 1; p++)
                            {
                                for (q = 0; q < 8 && flag == 1; q++)
                                {
                                    if (nextMove->pos[q][p].color == nextMove->turn && nextMove->pos[q][p].pieceNo == check)
                                    {
                                        nextMove->pos[q][p].pieceNo = nextMove->pos[j + 1][i + 1].pieceNo;
                                        nextMove->pos[j + 1][i + 1].color = -1;
                                        nextMove->pos[j + 1][i + 1].King = -1;
                                        nextMove->pos[j + 1][i + 1].pieceNo = -1;
                                        flag = 0;
                                    }
                                }
                            }

                            state doubleCheck = (state)malloc(sizeof(struct State));
                            copyPos(nextMove, doubleCheck);
                            if (doubleCheck->turn == 0)
                            {
                                doubleCheck->turn = 1;
                            }
                            else if (doubleCheck->turn == 1)
                            {
                                doubleCheck->turn = 0;
                            }

                            if (attackModeK(doubleCheck) != 0)
                            {
                                nextMove->turn = doubleCheck->turn;
                            }

                            free(doubleCheck);
                            kingrow = 0;

                            createkq(nextMove, k - 1);
                            free(nextMove);
                        }
                    }
                }
            }
        }
        else // Simplified version of the above code for normal moves
        {
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (curr->pos[j][i].color == curr->turn)
                    {
                        findValidMovesK(valid, i, j, curr);
                        if (valid[1] == 1)
                        {
                            state nextMove = (state)malloc(sizeof(struct State));
                            copyPos(curr, nextMove);
                            nextMove->pos[j - 1][i - 1].color = nextMove->pos[j][i].color;
                            nextMove->pos[j - 1][i - 1].King = nextMove->pos[j][i].King;
                            nextMove->pos[j - 1][i - 1].pieceNo = nextMove->pos[j][i].pieceNo;
                            if (nextMove->pos[j - 1][i - 1].color == 0 && j - 1 == 0 && nextMove->pos[j - 1][i - 1].King == 0)
                            {
                                nextMove->pos[j - 1][i - 1].King = 1;
                            }
                            nextMove->pos[j][i].color = -1;
                            nextMove->pos[j][i].King = -1;
                            nextMove->pos[j][i].pieceNo = -1;
                            if (nextMove->turn == 0)
                            {
                                nextMove->turn = 1;
                            }
                            else if (nextMove->turn == 1)
                            {
                                nextMove->turn = 0;
                            }
                            createkq(nextMove, k - 1);
                            free(nextMove);
                        }
                        if (valid[2] == 1)
                        {
                            state nextMove = (state)malloc(sizeof(struct State));
                            copyPos(curr, nextMove);
                            nextMove->pos[j - 1][i + 1].color = nextMove->pos[j][i].color;
                            nextMove->pos[j - 1][i + 1].King = nextMove->pos[j][i].King;
                            nextMove->pos[j - 1][i + 1].pieceNo = nextMove->pos[j][i].pieceNo;
                            if (nextMove->pos[j - 1][i + 1].color == 0 && j - 1 == 0 && nextMove->pos[j - 1][i + 1].King == 0)
                            {
                                nextMove->pos[j - 1][i + 1].King = 1;
                            }
                            nextMove->pos[j][i].color = -1;
                            nextMove->pos[j][i].King = -1;
                            nextMove->pos[j][i].pieceNo = -1;
                            if (nextMove->turn == 0)
                            {
                                nextMove->turn = 1;
                            }
                            else if (nextMove->turn == 1)
                            {
                                nextMove->turn = 0;
                            }
                            createkq(nextMove, k - 1);
                            free(nextMove);
                        }
                        if (valid[5] == 1)
                        {
                            state nextMove = (state)malloc(sizeof(struct State));
                            copyPos(curr, nextMove);
                            nextMove->pos[j + 1][i - 1].color = nextMove->pos[j][i].color;
                            nextMove->pos[j + 1][i - 1].King = nextMove->pos[j][i].King;
                            nextMove->pos[j + 1][i - 1].pieceNo = nextMove->pos[j][i].pieceNo;
                            if (nextMove->pos[j + 1][i - 1].color == 1 && j + 1 == 7 && nextMove->pos[j + 1][i - 1].King == 0)
                            {
                                nextMove->pos[j + 1][i - 1].King = 1;
                            }
                            nextMove->pos[j][i].color = -1;
                            nextMove->pos[j][i].King = -1;
                            nextMove->pos[j][i].pieceNo = -1;
                            if (nextMove->turn == 0)
                            {
                                nextMove->turn = 1;
                            }
                            else if (nextMove->turn == 1)
                            {
                                nextMove->turn = 0;
                            }
                            createkq(nextMove, k - 1);
                            free(nextMove);
                        }
                        if (valid[6] == 1)
                        {
                            state nextMove = (state)malloc(sizeof(struct State));
                            copyPos(curr, nextMove);
                            nextMove->pos[j + 1][i + 1].color = nextMove->pos[j][i].color;
                            nextMove->pos[j + 1][i + 1].King = nextMove->pos[j][i].King;
                            nextMove->pos[j + 1][i + 1].pieceNo = nextMove->pos[j][i].pieceNo;
                            if (nextMove->pos[j + 1][i + 1].color == 1 && j + 1 == 7 && nextMove->pos[j + 1][i + 1].King == 0)
                            {
                                nextMove->pos[j + 1][i + 1].King = 1;
                            }
                            nextMove->pos[j][i].color = -1;
                            nextMove->pos[j][i].King = -1;
                            nextMove->pos[j][i].pieceNo = -1;
                            if (nextMove->turn == 0)
                            {
                                nextMove->turn = 1;
                            }
                            else if (nextMove->turn == 1)
                            {
                                nextMove->turn = 0;
                            }
                            createkq(nextMove, k - 1);
                            free(nextMove);
                        }
                    }
                }
            }
        }
    }
}

void kmoves()
{
    // UI Function to go through the next k moves
    int k, quit;
    state curr;
    printf("Enter the value of k(Please let it be under 3 or Laptop go Boom):\n");
    scanf("%d", &k);
    createkq(back, k);
    curr = kfront;
    setPos(curr);
    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) // If there is an event, it is then assigned to event and becomes true
        {
            switch (event.type)
            {
            case SDL_QUIT: // If the event was a quit window event
                deletek();
                destroy();
                exit(0);
                break;

            case SDL_KEYDOWN:                       // If a keyboard button is pressed
                if (event.key.keysym.sym == SDLK_x) // If said button is x
                {
                    deletek();
                    quit = 1;
                }
                // Vim for fun
                else if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_h) && curr != kfront) // If said button is Left Arrow Key or h
                {
                    curr = curr->prev;
                    setPos(curr);
                }
                else if ((event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_l) && curr != kback) // If said button is Right Arrow Key or l
                {
                    curr = curr->next;
                    setPos(curr);
                }
                break;
            }
        }
        displayTint(-1, -1); // Refreshes the display
    }
    deletek();
}

void choosePiece()
{
    // load the image into memory using SDL_image library function
    Surface = IMG_Load("resources/Pokeball.png");
    if (!Surface)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    // load the image data into the graphics hardware's memory
    Piece = SDL_CreateTextureFromSurface(Rend, Surface);
    SDL_FreeSurface(Surface);
    if (!Piece)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(Rend);
        SDL_DestroyWindow(Window);
        SDL_Quit();
        exit(1);
    }

    SDL_RenderClear(Rend);
    SDL_RenderCopy(Rend, Piece, NULL, NULL);
    SDL_RenderPresent(Rend);

    char b[10], w[10];
    do
    {
        printf("\nBlack Player:\nChoose an icon to use(Use first character for row and second character for column)\n");
        scanf("%s", b);
    } while (strlen(b) != 2 || strcmp(b, "aa") < 0 || strcmp(b, "ee") > 0);

    strcat(blackPiece, b);
    strcat(blackPiece, ".png");

    do
    {
        printf("\nWhite Player:\nChoose an icon to use(Use first character for row and second character for column)\n");
        scanf("%s", w);
    } while (strlen(w) != 2 || strcmp(w, "aa") < 0 || strcmp(w, "ee") > 0 || strcmp(b, w) == 0);

    strcat(whitePiece, w);
    strcat(whitePiece, ".png");
}

void process()
{
    // The main function where the game lies
    // The function that combines everything
    int close_req = 0, select_x, select_y;

    init();

    choosePiece();

    makeBG();
    makeMoves();
    makeBlack();
    makeWhite();

    while (!close_req)
    {
        // Checking events
        SDL_Event event;
        while (SDL_PollEvent(&event)) // If there is an event, it is then assigned to event and becomes true
        {
            switch (event.type)
            {
            case SDL_QUIT: // If the event was a quit window event
                close_req = 1;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {                                                                 // Only true if the mouse is in the window and left button is clicked
                    select(event.button.x, event.button.y, &select_x, &select_y); // Assigns the row and column position mouse is currently in
                    if (pos[select_y][select_x].color == turn)
                    {
                        makemove(select_x, select_y);
                        if (movementPossible(0) == 0)
                        {
                            victoryDisplay(1);
                            SDL_Delay(15000);
                            close_req = 1;
                        }
                        else if (movementPossible(1) == 0)
                        {
                            victoryDisplay(0);
                            SDL_Delay(15000);
                            close_req = 1;
                        }
                    }
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_u)
                {
                    removeState();
                }
                else if (event.key.keysym.sym == SDLK_r)
                {
                    addState();
                    review();
                    removeState();
                }
                else if (event.key.keysym.sym == SDLK_k)
                {
                    addState();
                    kmoves();
                    removeState();
                }
                else if (event.key.keysym.sym == SDLK_d)
                {
                    victoryDisplay(2);
                    SDL_Delay(15000);
                    close_req = 1;
                }
                break;
            }
        }
        displayTint(-1, -1); // Refreshes the display
    }
    destroy();
}

int main(void)
{
    process();
}