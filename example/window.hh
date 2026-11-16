
#ifndef ECS_EXAMPLE_WINDOW_H
#define ECS_EXAMPLE_WINDOW_H

#include "defer.hh"

#include <SDL3/SDL.h>
#include <iostream>

namespace window {

    inline void run()
    {
        if (not SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "Failed to initialise SDL: " << SDL_GetError() << "\n";
            return;
        }
        defer{ SDL_Quit(); };

        SDL_Window* window = SDL_CreateWindow("ECS Example", 1280, 720, 0);
        defer{ SDL_DestroyWindow(window); };

        if (not window) {
            std::cerr << "Failed to create SDL window: " << SDL_GetError() << "\n";
            return;
        }

        SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
        defer{ SDL_DestroyRenderer(renderer); };

        if (not renderer) {
            std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << "\n";
            return;
        }

        // NB: Material Grey 700
        SDL_SetRenderDrawColor(renderer, 61, 61, 61, 255);

        bool running = true;
        SDL_Event event;

        while (running)
        {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                }
            }

            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            SDL_Delay(16);
        }
    }

} // namespace window

#endif /* ECS_EXAMPLE_WINDOW_H */
