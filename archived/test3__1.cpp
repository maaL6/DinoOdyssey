#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
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

struct GameObject {
    SDL_Rect rect;
    SDL_Color color;
    float x_velocity;
    float y_velocity;
};

bool checkAABBCollision(const SDL_Rect& a, const SDL_Rect& b) {
    if (a.x + a.w < b.x || b.x + b.w < a.x || a.y + a.h < b.y || b.y + b.h < a.y) {
        return false;
    }
    return true;
}

void resetGame(GameObject& player, std::vector<GameObject>& obstacles, bool& running, bool& isJumping, int& score, float& obstacleSpeed, int& currentGroundLevel) {
    score = 0;
    obstacleSpeed = 5.0f; 
    obstacles.clear();
    running = true;
    isJumping = false;

    const int PLAYER_SIZE = 25; 
    const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE - 20;  
    const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100; 


    currentGroundLevel = (rand() % 2 == 0) ? GROUND_LEVEL_LOW : GROUND_LEVEL_HIGH; 
    player.rect = {50, currentGroundLevel - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE};
    player.y_velocity = 0;

    GameObject initialPlatform;
    initialPlatform.rect = {0, currentGroundLevel, 150, 10};
    initialPlatform.color = COLOR_RED;
    initialPlatform.x_velocity = -obstacleSpeed;
    initialPlatform.y_velocity = 0;
    obstacles.push_back(initialPlatform);

}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Endless Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        IMG_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit(); 
        return 1;
    }

    srand(time(nullptr));

    SDL_Texture* groundTexture = nullptr;
    SDL_Surface* groundSurface = IMG_Load("ground.png");
    if (groundSurface == nullptr) {
        std::cerr << "Failed to load ground.png!" << std::endl;
        std::cerr << "SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit();
        return 1;
    }
    groundTexture = SDL_CreateTextureFromSurface(renderer, groundSurface);
    SDL_FreeSurface(groundSurface);

    if (groundTexture == nullptr) {
        std::cerr << "Failed to create texture from ground.png! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit(); 
        return 1;
    }

    const int PLAYER_SIZE = 25;
    const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE - 20;  
    const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100; 
    const float OBSTACLE_SPEED = 5.0f;
    const float JUMP_FORCE = -12.0f;
    const float GRAVITY = 0.5f;
    const Uint32 OBSTACLE_SPAWN_INTERVAL = 1500;
    const int PLATFORM_HEIGHT = 10;
    const int MAX_PLATFORM_GAP = 10;

    GameObject player;
    player.color = COLOR_PINK;
    player.x_velocity = 0;
    player.y_velocity = 0;

    std::vector<GameObject> obstacles;

    bool running = true;
    bool isJumping = false;
    int score = 0;
    Uint32 scoreTimer = SDL_GetTicks();
    const Uint32 SCORE_INTERVAL = 500;
    float obstacleSpeed = OBSTACLE_SPEED;
    Uint32 obstacleTimer = SDL_GetTicks();
    int currentGroundLevel = GROUND_LEVEL_LOW;

    resetGame(player, obstacles, running, isJumping, score, obstacleSpeed, currentGroundLevel);

    float lastPlatformX = 0;
    if (!obstacles.empty()) {
        lastPlatformX = obstacles.back().rect.x + obstacles.back().rect.w;
    }

    float scrollOffset = 0.0f;

    int groundWidth;

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

        // if (lastPlatformX < SCREEN_WIDTH + MAX_PLATFORM_GAP) {
        GameObject obstacle;
        int obstacleWidth = 500 + (rand() % 50);

        if (currentGroundLevel == GROUND_LEVEL_LOW) {
            currentGroundLevel = GROUND_LEVEL_HIGH;
        } else {
            currentGroundLevel = GROUND_LEVEL_LOW;
        }

        int newPlatformX = lastPlatformX + (rand() % MAX_PLATFORM_GAP) + 75; //50 min height
        obstacle.rect = {newPlatformX, currentGroundLevel, obstacleWidth, PLATFORM_HEIGHT};
        obstacle.color = (rand() % 2 == 0) ? COLOR_RED : COLOR_BLUE;
        obstacle.x_velocity = -obstacleSpeed;
        obstacle.y_velocity = 0;
        obstacles.push_back(obstacle);
        lastPlatformX = obstacle.rect.x + obstacle.rect.w;
        // }

        for (auto it = obstacles.begin(); it != obstacles.end();) {
            it->rect.x += it->x_velocity;

            if (it->rect.x + it->rect.w < 0) {
                it = obstacles.erase(it);
            } else {
                ++it;
            }
        }

        player.y_velocity += GRAVITY;
        player.rect.y += player.y_velocity;

        bool isOnGround = false;
        bool collided = true;

        for (const auto& obstacle : obstacles) {
            if (checkAABBCollision(player.rect, obstacle.rect)) {
                collided = false;

                if (player.y_velocity >= 0) {

                    if (player.rect.y + player.rect.h > obstacle.rect.y/* && player.rect.y + player.rect.h < obstacle.rect.y + obstacle.rect.h*/) {
                        player.rect.y = obstacle.rect.y - player.rect.h;
                        player.y_velocity = 0;
                        isJumping = false;
                        isOnGround = true;
                    }
                }
            }
        }

        if (!isOnGround && player.rect.y > SCREEN_HEIGHT) {
            std::cout << "Rơi khỏi nền! Game Over!" << std::endl;
            running = false; 
        }

        if (SDL_GetTicks() - scoreTimer > SCORE_INTERVAL) {
            scoreTimer = SDL_GetTicks();
            score++;
            obstacleSpeed += 0.05f;
        }

        SDL_SetRenderDrawColor(renderer, COLOR_TEAL.r, COLOR_TEAL.g, COLOR_TEAL.b, COLOR_TEAL.a);
        SDL_RenderClear(renderer);

        for (const auto& obstacle : obstacles) {
            SDL_QueryTexture(groundTexture, NULL, NULL, &groundWidth, NULL); 

            SDL_Rect sourceRect;
            sourceRect.x = (int)scrollOffset % groundWidth; 
            sourceRect.y = 0;
            sourceRect.w = obstacle.rect.w; 
            sourceRect.h = obstacle.rect.h;

            SDL_RenderCopy(renderer, groundTexture, &sourceRect, &obstacle.rect);
        }

        scrollOffset -= 0.5f;
        if (scrollOffset < 0) {
            scrollOffset += groundWidth;
        }

        SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, player.color.a);
        SDL_RenderFillRect(renderer, &player.rect);

        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Surface* textSurface = SDL_CreateRGBSurface(0, scoreText.length() * 10, 20, 32, 0, 0, 0, 0);
        SDL_Color textColor = COLOR_BLACK;

        if (textSurface != nullptr) {
            SDL_FillRect(textSurface, NULL, SDL_MapRGB(textSurface->format, 255, 255, 255)); 
            SDL_Rect textRect = {0, 0, textSurface->w, textSurface->h};

            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            if (textTexture != nullptr) {
                SDL_Rect renderRect = {10, 10, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &renderRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    SDL_DestroyTexture(groundTexture);

    if (!running) {
        std::cout << "Game Over! Score: " << score << std::endl;
        SDL_Delay(1000); 
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit(); 
    SDL_Quit();

    return 0;
}
