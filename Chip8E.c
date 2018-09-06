/* file Chip8E.c */

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "chip8.h"
#include "view.h"

int poll();

// #define TESTING

#ifdef TESTING
	#include "minunit.h"
#endif /* TESTING */

SDL_Event e;

int main(int argc, char **argv)
{
	#ifdef TESTING
		testmain(0, 0);
		exit(0);
	#endif /* TESTING */

	if(argc != 2) {
        printf("Usage: Chip8E.exe <chip8 game file>\n\n");
        exit(EXIT_FAILURE);
	}

	initialize();           // Initialize Chip8 system
	if(loadGame(argv[1]) == -1) {
        exit(EXIT_FAILURE);
    }

    if(!windowInit()) {     // Set up SDL rendering
        exit(EXIT_FAILURE);
    }

	// Main emulation loop
	int quit = 0;
	while(!quit) {
		emulateCycle();

		// Update SDL window
		if(*(getDrawFlag())) {
            windowDraw(getGfx(), NUM_OF_PIXEL_COLS, NUM_OF_PIXEL_ROWS);
			*(getDrawFlag()) = 0;

            SDL_Delay(DELAY_MS);  // A delay of 16ms between draw operations gives ~60 fps
		}

		if(poll() == -1)    // Handle keyboard events (press and release), and check if user exited window
            quit = 1;
	}

	windowClose();
	exit(EXIT_SUCCESS);
}

int poll() {
    while(SDL_PollEvent(&e) != 0) {     // Handle all SDL events on queue
        if(e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_1:
                    setKey(0, 1);
                    break;
                case SDLK_2:
                    setKey(1, 1);
                    break;
                case SDLK_3:
                    setKey(2, 1);
                    break;
                case SDLK_4:
                    setKey(3, 1);
                    break;
                case SDLK_q:
                    setKey(4, 1);
                    break;
                case SDLK_w:
                    setKey(5, 1);
                    break;
                case SDLK_e:
                    setKey(6, 1);
                    break;
                case SDLK_r:
                    setKey(7, 1);
                    break;
                case SDLK_a:
                    setKey(8, 1);
                    break;
                case SDLK_s:
                    setKey(9, 1);
                    break;
                case SDLK_d:
                    setKey(10, 1);
                    break;
                case SDLK_f:
                    setKey(11, 1);
                    break;
                case SDLK_z:
                    setKey(12, 1);
                    break;
                case SDLK_x:
                    setKey(13, 1);
                    break;
                case SDLK_c:
                    setKey(14, 1);
                    break;
                case SDLK_v:
                    setKey(15, 1);
                    break;
            }
        } else if(e.type == SDL_KEYUP) {
                switch(e.key.keysym.sym) {
                case SDLK_1:
                    setKey(0, 0);
                    break;
                case SDLK_2:
                    setKey(1, 0);
                    break;
                case SDLK_3:
                    setKey(2, 0);
                    break;
                case SDLK_4:
                    setKey(3, 0);
                    break;
                case SDLK_q:
                    setKey(4, 0);
                    break;
                case SDLK_w:
                    setKey(5, 0);
                    break;
                case SDLK_e:
                    setKey(6, 0);
                    break;
                case SDLK_r:
                    setKey(7, 0);
                    break;
                case SDLK_a:
                    setKey(8, 0);
                    break;
                case SDLK_s:
                    setKey(9, 0);
                    break;
                case SDLK_d:
                    setKey(10, 0);
                    break;
                case SDLK_f:
                    setKey(11, 0);
                    break;
                case SDLK_z:
                    setKey(12, 0);
                    break;
                case SDLK_x:
                    setKey(13, 0);
                    break;
                case SDLK_c:
                    setKey(14, 0);
                    break;
                case SDLK_v:
                    setKey(15, 0);
            }
        } else if(e.type == SDL_QUIT)
            return -1;
    }

    return 0;
}
