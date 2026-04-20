#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "WatchFaces/7_SEG/Watchy_7_SEG.h"

static const int LOGICAL_W = DISPLAY_WIDTH;
static const int LOGICAL_H = DISPLAY_HEIGHT;
static const int WINDOW_SCALE = 4;

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "WatchySim",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LOGICAL_W * WINDOW_SCALE, LOGICAL_H * WINDOW_SCALE,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!renderer) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_RenderSetLogicalSize(renderer, LOGICAL_W, LOGICAL_H);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
        LOGICAL_W, LOGICAL_H);
    if (!texture) {
        std::fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Watchy7SEG watchy;

    auto pushFaceToTexture = [&]() {
        watchy.showWatchFace();
        SDL_UpdateTexture(texture, NULL, watchy.display.getFramebuffer(), LOGICAL_W * 3);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    };

    // Initial draw
    {
        time_t now = time(NULL);
        struct tm tm_now;
        localtime_r(&now, &tm_now);
        watchy.setTime(tm_now);
        pushFaceToTexture();
    }

    int lastMinute = -1;
    bool running = true;
    Uint32 lastTick = SDL_GetTicks();

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (ev.key.keysym.sym == SDLK_ESCAPE || ev.key.keysym.sym == SDLK_q) {
                        running = false;
                    } else if (ev.key.keysym.sym == SDLK_r) {
                        pushFaceToTexture();
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (ev.window.event == SDL_WINDOWEVENT_EXPOSED ||
                        ev.window.event == SDL_WINDOWEVENT_RESIZED ||
                        ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        SDL_RenderCopy(renderer, texture, NULL, NULL);
                        SDL_RenderPresent(renderer);
                    }
                    break;
            }
        }

        Uint32 now = SDL_GetTicks();
        if (now - lastTick >= 1000) {
            lastTick = now;
            time_t t = time(NULL);
            struct tm tm_now;
            localtime_r(&t, &tm_now);
            watchy.setTime(tm_now);
            if (tm_now.tm_min != lastMinute) {
                lastMinute = tm_now.tm_min;
                pushFaceToTexture();
            }
        }

        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
