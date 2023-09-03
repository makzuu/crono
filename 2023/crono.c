#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WIN_WIDTH  640
#define WIN_HEIGHT 480
#define SECOND 1000.0
#define FPS 30.0
#define TARGET_FRAME_TIME (SECOND / FPS)
#define FONTSIZE 16

#define SECOND_COOLDOWN SECOND

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

    font = TTF_OpenFont("./fonts/Minecraft.ttf", FONTSIZE);
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

void update(SDL_Renderer *renderer, TTF_Font *font, int *minutes, int *seconds) {
    static int last_render = 0;
    static int second_cooldown = SECOND_COOLDOWN;

    int elapsed = SDL_GetTicks() - last_render;
    if (elapsed < TARGET_FRAME_TIME) {
        int delay = TARGET_FRAME_TIME - elapsed;
        SDL_Delay(delay);
    }
    elapsed = SDL_GetTicks() - last_render;

    last_render = SDL_GetTicks();

    SDL_Color bg = { 0x18, 0x18, 0x18, SDL_ALPHA_OPAQUE };
    SDL_Color fg = {0xcc, 0xcc, 0xcc, SDL_ALPHA_OPAQUE };

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    char time[6];
    sprintf(time, "%02d:%02d", *minutes, *seconds);

    SDL_Surface *sur = TTF_RenderText_Solid(font, time, fg);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sur);

    const int scale = 8;
    const int x_center = WIN_WIDTH / 2 - sur->w * scale / 2;
    const int y_center = WIN_HEIGHT / 2 - sur->h * scale / 2;

    SDL_Rect dst = { x_center, y_center, sur->w * scale, sur->h * scale };
    SDL_RenderCopy(renderer, tex, NULL, &dst);

    if (*minutes != 0 || *seconds != 0) {
        second_cooldown -= elapsed;
        if (second_cooldown <= 0) {
            if (*seconds == 0) (*minutes)--;
            *seconds = (*seconds + 60 - 1) % 60;

            second_cooldown = SECOND_COOLDOWN;
        }
    }

    SDL_FreeSurface(sur);
    SDL_DestroyTexture(tex);

    SDL_RenderPresent(renderer);
}
