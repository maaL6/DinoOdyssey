#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const SDL_Color COLOR_PINK = {255, 105, 180, 255};
const SDL_Color COLOR_GREEN = {0, 255, 0, 255};
const SDL_Color COLOR_BLUE = {0, 0, 255, 255};
const SDL_Color COLOR_RED = {255, 0, 0, 255};
const SDL_Color COLOR_TEAL = {0, 128, 128, 255}; 
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color COLOR_SPIKE = {200, 200, 200, 255}; 

struct GameObject {
    SDL_Rect rect;
    SDL_Color color;
    float x_velocity;
    float y_velocity;
    bool isLethal;
};

bool checkAABBCollision(const SDL_Rect& a, const SDL_Rect& b) {
    if (a.x + a.w < b.x || b.x + b.w < a.x || a.y + a.h < b.y || b.y + b.h < a.y) {
        return false;
    }
    return true;
}

void resetGame(GameObject& player, std::vector<GameObject>& obstacles, std::vector<GameObject>& spikes, bool& running, bool& isJumping, int& score, float& obstacleSpeed, int& currentGroundLevel) {
    score = 0;
    obstacleSpeed = 5.0f;
    obstacles.clear();
    spikes.clear(); 
    running = true;
    isJumping = false;

    const int PLAYER_SIZE = 25;
    const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE - 20;
    const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100;

    currentGroundLevel = (rand() % 2 == 0) ? GROUND_LEVEL_LOW : GROUND_LEVEL_HIGH;

    player.rect = {50, currentGroundLevel - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE};
    player.y_velocity = 0;

    GameObject initialPlatform;
    initialPlatform.rect = {0, currentGroundLevel, 300, 10};
    initialPlatform.color = COLOR_RED;
    initialPlatform.x_velocity = -obstacleSpeed;
    initialPlatform.y_velocity = 0;
    initialPlatform.isLethal = false;
    obstacles.push_back(initialPlatform);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Endless Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    srand(time(nullptr));

    const int PLAYER_SIZE = 25;
    const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE - 20;
    const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100;
    const float OBSTACLE_SPEED = 5.0f;
    const float JUMP_FORCE = -12.0f;
    const float GRAVITY = 0.5f;
    const int PLATFORM_HEIGHT = 10;
    const int SPIKE_SIZE = 15; 
    const float SPIKE_PROBABILITY = 0.9f;
    const int MAX_SPIKES_PER_PLATFORM = 3; 
    const int MAX_PLATFORM_GAP = 10;

    GameObject player;
    player.color = COLOR_PINK;
    player.x_velocity = 0;
    player.y_velocity = 0;

    std::vector<GameObject> obstacles;

    std::vector<GameObject> spikes;

    bool running = true;
    bool isJumping = false;
    int score = 0;
    Uint32 scoreTimer = SDL_GetTicks();
    const Uint32 SCORE_INTERVAL = 500;
    float obstacleSpeed = OBSTACLE_SPEED;
    Uint32 obstacleTimer = SDL_GetTicks();
    int currentGroundLevel = GROUND_LEVEL_LOW;

    resetGame(player, obstacles, spikes, running, isJumping, score, obstacleSpeed, currentGroundLevel);

    float lastPlatformX = 0;
    if (!obstacles.empty()) {
        lastPlatformX = obstacles.back().rect.x + obstacles.back().rect.w;
    }

    while (running) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE && !isJumping) {
                    isJumping = true;
                    player.y_velocity = JUMP_FORCE;
                    std::cout << "Nhảy!" << std::endl;
                }
            }
        }

        GameObject obstacle;
        int obstacleWidth = 1000 + (rand() % 50);

        if (currentGroundLevel == GROUND_LEVEL_LOW) {
            currentGroundLevel = GROUND_LEVEL_HIGH;
        } else {
            currentGroundLevel = GROUND_LEVEL_LOW;
        }

        int newPlatformX = lastPlatformX + (rand() % MAX_PLATFORM_GAP) + 75;
        obstacle.rect = {newPlatformX, currentGroundLevel, obstacleWidth, PLATFORM_HEIGHT};
        obstacle.color = (rand() % 2 == 0) ? COLOR_RED : COLOR_BLUE;
        obstacle.x_velocity = -obstacleSpeed;
        obstacle.y_velocity = 0;
        obstacle.isLethal = false;
        obstacles.push_back(obstacle);
        lastPlatformX = obstacle.rect.x + obstacle.rect.w;

        if ((float)rand() / RAND_MAX < SPIKE_PROBABILITY) {

            int numSpikes = 1 + (rand() % MAX_SPIKES_PER_PLATFORM);

            int spikeSpacing = obstacleWidth / (numSpikes + 1);

            for (int i = 1; i <= numSpikes; ++i) {
                GameObject spike;
                spike.rect = {newPlatformX + i * spikeSpacing - SPIKE_SIZE / 2, currentGroundLevel - SPIKE_SIZE, SPIKE_SIZE, SPIKE_SIZE}; // Căn giữa
                spike.color = COLOR_SPIKE;
                spike.x_velocity = -obstacleSpeed; // Di chuyển cùng nền
                spike.y_velocity = 0;
                spike.isLethal = true;
                spikes.push_back(spike);
            }
        }

        for (auto it = obstacles.begin(); it != obstacles.end();) {
            it->rect.x += it->x_velocity;

            if (it->rect.x + it->rect.w < 0) {
                it = obstacles.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = spikes.begin(); it != spikes.end();) {
            it->rect.x += it->x_velocity;

            if (it->rect.x + it->rect.w < 0) {
                it = spikes.erase(it);
            } else {
                ++it;
            }
        }

        player.y_velocity += GRAVITY;
        player.rect.y += player.y_velocity;

        bool isOnGround = false;

        for (const auto& obstacle : obstacles) {
            if (checkAABBCollision(player.rect, obstacle.rect)) {

                if (player.y_velocity >= 0) {

                    if (player.rect.y + player.rect.h > obstacle.rect.y) {
                        player.rect.y = obstacle.rect.y - player.rect.h;
                        player.y_velocity = 0;
                        isJumping = false;
                        isOnGround = true;
                    }
                }
            }
        }

        for (const auto& spike : spikes) {
            if (checkAABBCollision(player.rect, spike.rect)) {
                std::cout << "Chạm gai! Game Over!" << std::endl;
                running = false;
                break; 
            }
        }

        if (!isOnGround && player.rect.y > SCREEN_HEIGHT) {
            std::cout << "Rơi khỏi nền! Game Over!" << std::endl;
            running = false; 
        }

        // Cập nhật điểm
        if (SDL_GetTicks() - scoreTimer > SCORE_INTERVAL) {
            scoreTimer = SDL_GetTicks();
            score++;
            //obstacleSpeed += 0.05f;
        }

        SDL_SetRenderDrawColor(renderer, COLOR_TEAL.r, COLOR_TEAL.g, COLOR_TEAL.b, COLOR_TEAL.a);
        SDL_RenderClear(renderer);

        for (const auto& obstacle : obstacles) {
            SDL_SetRenderDrawColor(renderer, obstacle.color.r, obstacle.color.g, obstacle.color.b, obstacle.color.a);
            SDL_RenderFillRect(renderer, &obstacle.rect);
        }

        for (const auto& spike : spikes) {
            SDL_SetRenderDrawColor(renderer, spike.color.r, spike.color.g, spike.color.b, spike.color.a);
            SDL_RenderFillRect(renderer, &spike.rect);
        }

        SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, player.color.a);
        SDL_RenderFillRect(renderer, &player.rect);

        // Vẽ điểm
        /*
        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Surface* textSurface = SDL_CreateRGBSurface(0, scoreText.length() * 10, 20, 32, 0, 0, 0, 0);
        SDL_Color textColor = COLOR_BLACK;

        if (textSurface != nullptr) {
            SDL_FillRect(textSurface, NULL, SDL_MapRGB(textSurface->format, 255, 255, 255)); // White background
            SDL_Rect textRect = {0, 0, textSurface->w, textSurface->h};

            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            if (textTexture != nullptr) {
                SDL_Rect renderRect = {10, 10, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &renderRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }*/

        SDL_RenderPresent(renderer);
        SDL_Delay(9);
    }

    if (!running) {
        std::cout << "Game Over! Score: " << score << std::endl;
        SDL_Delay(1000);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
