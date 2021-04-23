#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (1440)
#define WINDOW_HEIGHT (1440)

SDL_Window* Window;
SDL_Renderer* Rend;
SDL_Surface* Surface;
SDL_Texture* Bg;
SDL_Texture* Black_tex;
SDL_Texture* White_tex;
SDL_Rect Blacks[12];
SDL_Rect Whites[12];

int turn = 0;

int grid[9];

struct Pos{
    int x;
    int y;
}pos[8][8];

void init(){
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    Window = SDL_CreateWindow("Game Board",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT,0);
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

    for(i = 0; i < 9; i++){
        grid[i] = i * inc;
    }

    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            pos[i][j].x = (grid[j] + grid[j + 1]) / 2;
            pos[i][j].y = (grid[i] + grid[i + 1]) / 2;
        }
    }
}

void makeBG(){
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

void makeBlack(){
    Surface = IMG_Load("resources/black_piece.png");
    int i;

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

    for(i = 0; i < 12; i++){
        SDL_QueryTexture(Black_tex, NULL, NULL, &Blacks[i].w, &Blacks[i].h);
        Blacks[i].w = WINDOW_WIDTH / 9;
        Blacks[i].h = WINDOW_HEIGHT / 9;
    }

    for(i = 0; i < 4; i++){
        Blacks[i].x = pos[7][i * 2].x - Blacks[i].w / 2;
        Blacks[i].y = pos[7][i].y - Blacks[i].h / 2;
        Blacks[i + 8].x = pos[5][i * 2].x - Blacks[i].w / 2;
        Blacks[i + 8].y = pos[5][i].y - Blacks[i].h / 2;
        Blacks[i + 4].x = pos[6][1 + i * 2].x - Blacks[i].w / 2;
        Blacks[i + 4].y = pos[6][i].y - Blacks[i].h / 2;
    }
}

void makeWhite(){
    Surface = IMG_Load("resources/white_piece.png");
    int i;

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

    for(i = 0; i < 12; i++){
        SDL_QueryTexture(White_tex, NULL, NULL, &Whites[i].w, &Whites[i].h);
        Whites[i].w = WINDOW_WIDTH / 9;
        Whites[i].h = WINDOW_HEIGHT / 9;
    }

    for(i = 0; i < 4; i++){
        Whites[i].x = pos[0][1 + i * 2].x - Whites[i].w / 2;
        Whites[i].y = pos[0][i].y - Whites[i].h / 2;
        Whites[i + 8].x = pos[2][1 + i * 2].x - Whites[i].w / 2;
        Whites[i + 8].y = pos[2][i].y - Whites[i].h / 2;
        Whites[i + 4].x = pos[1][i * 2].x - Whites[i].w / 2;
        Whites[i + 4].y = pos[1][i].y - Whites[i].h / 2;
    }
}

void display(){
    int i;
    // Clear the Window
    SDL_RenderClear(Rend);

    // Write all image to the window

    SDL_RenderCopy(Rend, Bg, NULL, NULL);    

    for(i = 0; i < 12; i++){
        SDL_RenderCopy(Rend, Black_tex, NULL, &Blacks[i]);
        SDL_RenderCopy(Rend, White_tex, NULL, &Whites[i]);
    }

    SDL_RenderPresent(Rend);
}

void move(int PieceNo, int BlackorWhite, int x, int y){
    // Values to be used for calculating position and relative postion and movement for the pieces
    int x_pos, y_pos, x_final, y_final, distx, disty;

    // Chooses which Color Piece to move
    if(BlackorWhite == 0){
        x_pos = Blacks[PieceNo].x;
        y_pos = Blacks[PieceNo].y;
        x_final = pos[y][x].x - Blacks[PieceNo].w / 2;
        y_final = pos[y][x].y - Blacks[PieceNo].h / 2;

        distx = (x_final - x_pos) / 60;                 // Amount to move by in x direction
        disty = (y_final - y_pos) / 60;                 // Amount to move by in y direction

        while(x_pos != x_final || y_pos != y_final){    // May Bug out if distx and disty are not equal
            x_pos += distx;                             // Need to check and fix if problem exists
            y_pos += disty;

            Blacks[PieceNo].x = x_pos;
            Blacks[PieceNo].y = y_pos;
            display();
            SDL_Delay(500 / 60);
        }
    }
    else{
        x_pos = Whites[PieceNo].x;
        y_pos = Whites[PieceNo].y;
        x_final = pos[y][x].x - Whites[PieceNo].w / 2;
        y_final = pos[y][x].y - Whites[PieceNo].h / 2;

        distx = (x_final - x_pos) / 60;                 // Amount to move by in x direction
        disty = (y_final - y_pos) / 60;                 // Amount to move by in y direction

        while(x_pos != x_final && y_pos != y_final){    // May Bug out if distx and disty are not equal
            x_pos += distx;                             // Need to check and fix if problem exists
            y_pos += disty;
            Whites[PieceNo].x = x_pos;
            Whites[PieceNo].y = y_pos;
            display();
            SDL_Delay(500 / 60);
        }
    }
}

void destroy(){
    // Destroy all textures and renderers and windows
    int i;
    SDL_DestroyTexture(Bg);
    for(i = 0; i < 12; i++){
        SDL_DestroyTexture(Black_tex);
        SDL_DestroyTexture(White_tex);
    }

    SDL_DestroyRenderer(Rend);
    SDL_DestroyWindow(Window);

    SDL_Quit();
}

int bs(int element){            //Not Working, need to debug
    int beg = 0, end = 7;
    int mid = (beg + end) / 2;

    while(beg <= end){
        if(element > grid[mid] && element < grid[mid + 1])
            return mid;

        else if(element < grid[mid])
            end = mid;
        
        else if(element > grid[mid + 1])
            beg = mid + 1;

        mid = (beg + end) / 2;
    }
    return mid;
}

void select(int mouse_x, int mouse_y, int* select_x, int* select_y){
    *select_x = bs(mouse_x);
    *select_y = bs(mouse_y);
    printf("%d %d\n", *select_x, *select_y);
}

void process(){
    int close_req = 0, mouse_x, mouse_y, button, select_x, select_y;

    init();

    makeBG();
    makeBlack();
    makeWhite();

    while(!close_req){
        // Checking events
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                close_req = 1;
            }
        }
        button = SDL_GetMouseState(&mouse_x, &mouse_y);
        if(button & SDL_BUTTON(SDL_BUTTON_LEFT)){
            select(mouse_x, mouse_y, &select_x, &select_y);
        }
        display();
        move(11, 0, 4, 3);
    }
    destroy();
}

int main(void)
{
    process();
}
