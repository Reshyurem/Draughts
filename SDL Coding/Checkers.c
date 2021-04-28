#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (960)
#define WINDOW_HEIGHT (960)

SDL_Window *Window;
SDL_Renderer *Rend;
SDL_Surface *Surface;
SDL_Texture *Bg;
SDL_Texture *Black_tex;
SDL_Texture *White_tex;
SDL_Rect Blacks[12];
SDL_Rect Whites[12];

int turn = 0;

int grid[9];

struct Pos
{
    int x;
    int y;
    int color;
    int pieceNo;
    int King;
} pos[8][8];

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

void init()
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
            pos[i][j].color = 2;
            pos[i][j].King = 0;
            pos[i][j].pieceNo = 0;
        }
    }
}

void makeBG()
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

void makeBlack()
{
    Surface = IMG_Load("resources/black_piece.png");
    int i;

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

void makeWhite()
{
    Surface = IMG_Load("resources/white_piece.png");
    int i;

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
    int i;
    // Clear the Window
    SDL_RenderClear(Rend);
    // Write all image to the window
    if (turn == 0)
        SDL_SetTextureColorMod(Bg, 220, 36, 36);
    else if (turn == 1)
        SDL_SetTextureColorMod(Bg, 74, 86, 157);
    SDL_RenderCopy(Rend, Bg, NULL, NULL);
    SDL_SetTextureColorMod(Bg, 255, 255, 255);

    switch (color)
    {
    case 0:
        for (i = 0; i < 12; i++)
        {
            if (i == pieceNo)
                continue;
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        }
        for (i = 0; i < 12; i++)
        {
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
        SDL_SetTextureColorMod(Black_tex, 52, 231, 238);
        SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[pieceNo]);
        SDL_SetTextureColorMod(Black_tex, 255, 255, 255);
        break;
    case 1:
        for (i = 0; i < 12; i++)
        {
            if (i == pieceNo)
                continue;
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
        for (i = 0; i < 12; i++)
        {
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        }
        SDL_SetTextureColorMod(White_tex, 52, 231, 238);
        SDL_RenderCopy(Rend, White_tex, NULL, &Whites[pieceNo]);
        SDL_SetTextureColorMod(White_tex, 255, 255, 255);
        break;
    default:
        for (i = 0; i < 12; i++)
        {
            SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
            SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
        }
    }
    // Upload Render to the Window
    SDL_RenderPresent(Rend);
}

int validMove(int x, int y, int select_x, int select_y)
{ // Needs to be changed to accomodate CLI version of the game
    return 1;
}

void move(int pieceNo, int color, int x, int y)
{
    // Values to be used for calculating position and relative postion and movement for the pieces
    int x_pos, y_pos, x_final, y_final, distx, disty, currx, curry;

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
    }
    else if(color == 1)
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
    }
}

void destroy()
{
    // Destroy all textures and renderers and windows
    int i;
    SDL_DestroyTexture(Bg);
    for (i = 0; i < 12; i++)
    {
        SDL_DestroyTexture(Black_tex);
        SDL_DestroyTexture(White_tex);
    }

    SDL_DestroyRenderer(Rend);
    SDL_DestroyWindow(Window);

    SDL_Quit();
}

void makemove(int x, int y)
{
    int undo = 0, select_x = -1, select_y = -1;
    while (!undo)
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
                //displayValidMoves(x, y);
                if (event.button.button == SDL_BUTTON_LEFT)
                {                                                                 // Only true if the mouse is in the window and left button is clicked
                    select(event.button.x, event.button.y, &select_x, &select_y); // Assigns the row and column position mouse is currently in
                    if (select_x == x && select_y == y)
                    {
                        undo = 1;
                    }
                    else if (pos[select_y][select_x].color == 2 && validMove(x, y, select_x, select_y))
                    {
                        move(pos[y][x].pieceNo, pos[y][x].color, select_x, select_y);
                        undo = 1;
                    }
                }
                break;
            }
        }
        displayTint(pos[y][x].pieceNo, pos[y][x].color); // Refreshes the display
    }
}

void process()
{
    // The function that combines everything
    int close_req = 0, select_x, select_y;

    init();

    makeBG();
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
                //displayValidMoves(x, y);
                if (event.button.button == SDL_BUTTON_LEFT)
                {                                                                 // Only true if the mouse is in the window and left button is clicked
                    select(event.button.x, event.button.y, &select_x, &select_y); // Assigns the row and column position mouse is currently in
                    if (pos[select_y][select_x].color == 0 || pos[select_y][select_x].color == 1)
                    {
                        makemove(select_x, select_y);
                    }
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