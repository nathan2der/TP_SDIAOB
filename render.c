#include "render.h"

/*
  This function initialize a window and the renderer from SDL library.
  Open a window input :
    - pointer adress of SDL_WINDOW
    - pointer adress of SDL_RENDERER
  output : void
*/
void
render_initialisation (SDL_Window **window, SDL_Renderer **renderer)
{

  SDL_Init (SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer (DIM_WINDOW, DIM_WINDOW, SDL_WINDOW_OPENGL,
                               window, renderer);
}

/*
  This function clears the renderer
  input :
    - pointer of SDL_RENDERER
  output : void
*/
void
clear_window (SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
  SDL_RenderClear (renderer);
}

/*
  This function draws a pixel on the renderer
  input :
    - adress of SDL_RENDERER
    - position x of the particule
    - position y of the particule
  output : void
*/
void
draw_body (SDL_Renderer *renderer, i32 x_position, i32 y_position)
{
  SDL_SetRenderDrawColor (renderer, 0, 255, 255, 255);
  SDL_RenderDrawPoint (renderer, x_position, y_position);
}

/*
  This function makes the render of the renderer and get event to escape
  input :
    - adress of SDL_RENDERER
    - adress of SDL_EVENT
  output :
    - escape value (int)
*/
int
render_and_event (SDL_Renderer *renderer, SDL_Event *event)
{
  SDL_RenderPresent (renderer);

  SDL_Delay (10);

  unsigned char quit = 0;
  // check if event (quit)
  while (SDL_PollEvent (event))
    if (event->type == SDL_QUIT)
      quit = 1;
    else if (event->type == SDL_KEYDOWN)
      if (event->key.keysym.sym == SDLK_q)
        quit = 1;
  return quit;
}

/*
  This function removes all sdl objects and close the window
  input :
    - adress of SDL_RENDERER
    - adress of SDL_EVENT
  output : void
*/
void
close_and_clean (SDL_Renderer *renderer, SDL_Window *window)
{
  SDL_DestroyRenderer (renderer);
  SDL_DestroyWindow (window);
  SDL_Quit ();
}
