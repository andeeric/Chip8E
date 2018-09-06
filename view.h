/* file view.h */

#ifndef VIEW_H
#define VIEW_H

SDL_Window* window;
SDL_Surface* screenSurface;
SDL_Renderer* renderer;

int windowInit();
int loadMedia();
void windowDraw();
void windowClose();

#endif /* VIEW_H */
