#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WIN_WIDTH  640
#define WIN_HEIGHT 480
#define SECOND 1000.0
#define FPS 30.0
#define TARGET_FRAME_TIME (SECOND / FPS)
#define FONTSIZE 32

void input(SDL_Event *ev);
void update(SDL_Renderer *renderer, TTF_Font *font, int *minutes, int *second);

int running = 1;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage:\n\tcrono MINUTES\n");
        return 1;
    }

    int minutes = atoi(argv[1]);
    int seconds = 0;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "%s\n", TTF_GetError());
        return 1;
    }

    font = TTF_OpenFont("./fonts/vt323/VT323-Regular.ttf", FONTSIZE);
    if (font == NULL) {
        fprintf(stderr, "%s\n", TTF_GetError());
        return 1;
    }

    window = SDL_CreateWindow("crono", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIN_WIDTH, WIN_HEIGHT, 0);
    if (window == NULL) {
        fprintf(stderr, "%s\n", SDL_GetError());
        TTF_CloseFont(font);
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        TTF_CloseFont(font);
        SDL_DestroyWindow(window);
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    SDL_Event ev;
    while (running) {
        input(&ev);
        update(renderer, font, &minutes, &seconds);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

void input(SDL_Event *ev) {
    while (SDL_PollEvent(ev)) {
        switch (ev->type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                if (ev->key.keysym.sym == SDLK_q) {
                    running = 0;
                    break;
                }
                break;
        }
    }
}

int last_render = 0;
int last_second = 0;
int time_paused = 0;

void update(SDL_Renderer *renderer, TTF_Font *font, int *minutes, int *seconds) {
    int elapsed = SDL_GetTicks() - last_render;
    if (elapsed < TARGET_FRAME_TIME) {
        int delay = TARGET_FRAME_TIME - elapsed;
        SDL_Delay(delay);
    }
    last_render = SDL_GetTicks();


    SDL_Color bg = { 0x18, 0x18, 0x18, SDL_ALPHA_OPAQUE };
    SDL_Color fg = {0xcc, 0xcc, 0xcc, SDL_ALPHA_OPAQUE };

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    char time[6];
    sprintf(time, "%02d:%02d", *minutes, *seconds);

    SDL_Surface *sur = TTF_RenderText_Solid(font, time, fg);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sur);

    SDL_Rect rect = { 100, 100, 440, 280 };
    SDL_RenderCopy(renderer, tex, NULL, &rect);

    if (*minutes == 0 && *seconds == 0) time_paused = 1;

    if (SDL_GetTicks() - last_second >= SECOND && !time_paused) {
        if (*seconds == 0) (*minutes)--;
        *seconds = (*seconds + 60 - 1) % 60;

        last_second = SDL_GetTicks();
    }

    SDL_FreeSurface(sur);
    SDL_DestroyTexture(tex);

    SDL_RenderPresent(renderer);
}
