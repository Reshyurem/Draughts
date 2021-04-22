#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH (1080)
#define WINDOW_HEIGHT (1080)

SDL_Window* Window;
SDL_Renderer* Rend;
SDL_Surface* Surface;
SDL_Texture* Bg;
SDL_Texture* Black_tex;
SDL_Texture* White_tex;
SDL_Rect Blacks[12];
SDL_Rect Whites[12];

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
    int start = WINDOW_HEIGHT / 16, inc = WINDOW_HEIGHT / 8;
    
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            pos[i][j].x = start + j * inc;
            pos[i][j].y = WINDOW_HEIGHT - start - (7 - i) * inc;
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
        Blacks[i].w = WINDOW_HEIGHT / 8;
        Blacks[i].h = WINDOW_HEIGHT / 8;
        SDL_QueryTexture(Black_tex, NULL, NULL, &Blacks[i].w, &Blacks[i].h);
    }

    for(i = 0; i < 4; i++){
        Blacks[i].x = pos[7][i * 2].x;
        Blacks[i].y = pos[7][i].y;
        Blacks[i + 8].x = pos[5][i * 2].x;
        Blacks[i + 8].y = pos[5][i].y;
        Blacks[i + 4].x = pos[6][1 + i * 2].x;
        Blacks[i + 4].y = pos[6][i].y;
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
        Whites[i].w = 135;
        Whites[i].h = WINDOW_HEIGHT / 8;
        SDL_QueryTexture(White_tex, NULL, NULL, &Whites[i].w, &Whites[i].h);
    }

    for(i = 0; i < 4; i++){
        Whites[i].x = pos[0][1 + i * 2].x;
        Whites[i].y = pos[0][i].y;
        Whites[i + 8].x = pos[2][1 + i * 2].x;
        Whites[i + 8].y = pos[2][i].y;
        Whites[i + 4].x = pos[1][i * 2].x;
        Whites[i + 4].y = pos[1][i].y;
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

int main(void)
{
    init();

    makeBG();
    makeBlack();
    makeWhite();

    display();

    SDL_Delay(5000);

    destroy();
}
