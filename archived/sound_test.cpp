#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SLIDER_START_X = 100;
const int SLIDER_END_X = 500;
const int SLIDER_Y = 300;
const int KNOB_SIZE = 20;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mixer initialization failed: " << Mix_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Music Volume Control",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Mix_Music* backgroundMusic = Mix_LoadMUS("background_music.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        return 1;
    }

    if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
        return 1;
    }

    int currentX = (SLIDER_START_X + SLIDER_END_X) / 2; // Vị trí nút ban đầu
    bool isDragging = false;
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    SDL_Point mousePos = { e.button.x, e.button.y };

                    if (mousePos.y >= SLIDER_Y - 10 &&
                        mousePos.y <= SLIDER_Y + 10 &&
                        mousePos.x >= SLIDER_START_X &&
                        mousePos.x <= SLIDER_END_X) {
                        currentX = mousePos.x;
                        currentX = SDL_clamp(currentX, SLIDER_START_X, SLIDER_END_X);
                        isDragging = true;
                    }
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    isDragging = false;
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                if (isDragging) {
                    currentX = SDL_clamp(e.motion.x, SLIDER_START_X, SLIDER_END_X);
                }
            }
        }

        int volume = ((currentX - SLIDER_START_X) * MIX_MAX_VOLUME) /
                   (SLIDER_END_X - SLIDER_START_X);
        Mix_VolumeMusic(volume);

        // render
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // slider
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderDrawLine(renderer, SLIDER_START_X, SLIDER_Y, SLIDER_END_X, SLIDER_Y);

        // knob
        SDL_Rect knob = {
            currentX - KNOB_SIZE/2,
            SLIDER_Y - KNOB_SIZE/2,
            KNOB_SIZE,
            KNOB_SIZE
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &knob);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
