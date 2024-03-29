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

#define SDL_ERROR 0
#define TTF_ERROR 1

void perr(int error)
{
    if (error == SDL_ERROR)
        fprintf(stderr, "%s\n", SDL_GetError());
    else
        fprintf(stderr, "%s\n", TTF_GetError());
}

void cnull(void *p, int error)
{
    if(p == NULL) {
        perr(error);
        exit(1);
    }
}

void ccode(int c, int error)
{
    if (c < 0) {
        perr(error);
        exit(1);
    }
}

void usage(void)
{
    char usage[] =
        "USAGE:\n"
        "\t./crono MINUTES [START_COMMAND PAUSE/END_COMMAND]\n"
        "EXAMPLES:\n"
        "\t ./crono 30\n"
        "\t ./crono 30 \"timew start task1\" \"timew stop\"\n";

    printf("%s", usage);
}

int running = 1;
int time_paused = 0;

#define ARG_START_COMMAND 2
#define ARG_PAUSE_COMMAND 3

#define START_COMMAND 0
#define PAUSE_COMMAND 1

char *commands[2];

int main(int argc, char *argv[]) {
    char font_src[46];
    if (strcmp(argv[0], "./crono") == 0) {
        strcpy(font_src, "./fonts/Minecraft.ttf");
    } else if (strcmp(argv[0], "crono") == 0) {
        strcpy(font_src, "/usr/local/share/fonts/truetype/Minecraft.ttf");
    } else {
        printf("ERROR\n");
        return 1;
    }

    if (argc == 4) {
        size_t start_command_len = strlen(argv[ARG_START_COMMAND]);
        size_t pause_command_len = strlen(argv[ARG_PAUSE_COMMAND]);

        commands[START_COMMAND] = malloc(start_command_len);
        commands[PAUSE_COMMAND] = malloc(pause_command_len);

        strcpy(commands[START_COMMAND], argv[ARG_START_COMMAND]);
        strcpy(commands[PAUSE_COMMAND], argv[ARG_PAUSE_COMMAND]);

        system(commands[START_COMMAND]);
    }

    if (argc < 2) {
        usage();
        return 1;
    }

    int minutes = atoi(argv[1]);
    int seconds = 0;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;

    ccode(SDL_Init(SDL_INIT_VIDEO), SDL_ERROR);

    ccode(TTF_Init(), TTF_ERROR);

    cnull(font = TTF_OpenFont(font_src, FONTSIZE), TTF_ERROR);

    cnull(window = SDL_CreateWindow("crono", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIN_WIDTH, WIN_HEIGHT, 0), SDL_ERROR);

    cnull(renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED), SDL_ERROR);

    SDL_Event ev;
    while (running) {
        input(&ev);
        update(renderer, font, &minutes, &seconds);
    }

    system(commands[PAUSE_COMMAND]);

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
                switch (ev->key.keysym.sym) {
                    case SDLK_q: {
                        running = 0;
                    } break;
                    case SDLK_SPACE: {
                        time_paused = !time_paused;
                        if (time_paused) {
                            system(commands[PAUSE_COMMAND]);
                        } else {
                            system(commands[START_COMMAND]);
                        }
                    } break;
                }
                break;
        }
    }
}

SDL_Color bg = { 0x18, 0x18, 0x18, SDL_ALPHA_OPAQUE };
SDL_Color fg = {0xcc, 0xcc, 0xcc, SDL_ALPHA_OPAQUE };

void render_background(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);
}

void render_time(char *time, TTF_Font *font, SDL_Renderer *renderer) {
    SDL_Surface *sur = TTF_RenderText_Solid(font, time, fg);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sur);

    const int scale = 8;
    const int x_center = WIN_WIDTH / 2 - sur->w * scale / 2;
    const int y_center = WIN_HEIGHT / 2 - sur->h * scale / 2;

    SDL_Rect dst = { x_center, y_center, sur->w * scale, sur->h * scale };
    SDL_RenderCopy(renderer, tex, NULL, &dst);

    SDL_FreeSurface(sur);
    SDL_DestroyTexture(tex);
}

void update_time(int *minutes, int *seconds, int *second_cooldown, const int elapsed) {
    if (*minutes != 0 || *seconds != 0) {
        if (!time_paused) {
            *second_cooldown -= elapsed;
            if (*second_cooldown <= 0) {
                if (*seconds == 0) (*minutes)--;
                *seconds = (*seconds + 60 - 1) % 60;

                *second_cooldown = SECOND_COOLDOWN;
            }
        }
    }
}
void render_pause_status(TTF_Font *font,SDL_Renderer *renderer, int x, int y) {
    if (time_paused) {
        const int scale = 2;

        SDL_Surface *sur = TTF_RenderText_Solid(font, "paused", fg);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sur);

        y -= sur->h * scale;

        SDL_Rect dst = { x, y, sur->w * scale, sur->h * scale };
        SDL_RenderCopy(renderer, tex, NULL, &dst);

        SDL_FreeSurface(sur);
        SDL_DestroyTexture(tex);
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

    char time[6];
    sprintf(time, "%02d:%02d", *minutes, *seconds);

    render_background(renderer);
    render_time(time, font, renderer);
    int rbc_x = 10;
    int rbc_y = WIN_HEIGHT - 10;
    render_pause_status(font, renderer, rbc_x, rbc_y);
    update_time(minutes, seconds, &second_cooldown, elapsed);

    SDL_RenderPresent(renderer);
}
