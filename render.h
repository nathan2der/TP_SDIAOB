#ifndef RENDER_H
#define RENDER_H

#include "types.h"
#include <SDL2/SDL.h>

void render_initialisation (SDL_Window **window, SDL_Renderer **renderer);
void clear_window (SDL_Renderer *renderer);
void draw_body (SDL_Renderer *renderer, i32 x_position, i32 y_position);
i32 render_and_event (SDL_Renderer *renderer, SDL_Event *event);
void close_and_clean (SDL_Renderer *renderer, SDL_Window *window);

#endif
