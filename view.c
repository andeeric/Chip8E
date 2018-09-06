/* file view.c */

#include <SDL.h>
#include <stdio.h>
#include "view.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

int windowInit() {
    int success = 1;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "Error: SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = 0;
    } else {
        // Create window
        window = SDL_CreateWindow("Chip8E", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL) {
            printf( "Error: Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = 0;
        } else {
            // Get window surface
            screenSurface = SDL_GetWindowSurface(window);
            // Renderer
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if(renderer == NULL) {
                printf( "Error: Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
                success = 0;
            }
        }
    }

    return success;
}

void windowDraw(unsigned char* gfx, int NUM_OF_PIXEL_COLS, int NUM_OF_PIXEL_ROWS) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Pixel position
    SDL_Rect r;
    int x = 0;
    int y = 0;
    int w = SCREEN_WIDTH / NUM_OF_PIXEL_COLS;
    int h = SCREEN_HEIGHT / NUM_OF_PIXEL_ROWS;

    // Rendering
    int i = 0;
    for(int j = 0; j < NUM_OF_PIXEL_ROWS; j++) {
        x = 0;
        y = j * h;
        for(int k = 0; k < NUM_OF_PIXEL_COLS; k++) {
            i++;
            x = k * w;
            if(gfx[i] == 1) {
                r.x = x;
                r.y = y;
                r.w = w;
                r.h = h;
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(renderer, &r);
                SDL_RenderPresent(renderer);
            }
        }
    }
}

void windowClose() {
    SDL_DestroyWindow(window);
    window = NULL;
    screenSurface = NULL;
    renderer = NULL;

    SDL_Quit();
}
