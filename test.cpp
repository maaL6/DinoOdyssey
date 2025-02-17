#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Kích thước màn hình
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Màu sắc (RGB)
const SDL_Color COLOR_PINK = {255, 105, 180, 255};
const SDL_Color COLOR_GREEN = {0, 255, 0, 255};
const SDL_Color COLOR_BLUE = {0, 0, 255, 255};
const SDL_Color COLOR_RED = {255, 0, 0, 255};
const SDL_Color COLOR_TEAL = {0, 128, 128, 255}; // Màu nền
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};

// Cấu trúc để đại diện cho một đối tượng (player hoặc chướng ngại vật)
struct GameObject {
    SDL_Rect rect;
    SDL_Color color;
    float x_velocity;
    float y_velocity;
};

// Hàm kiểm tra va chạm
bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_HasIntersection(&a, &b);
}

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Endless Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Khởi tạo bộ sinh số ngẫu nhiên
    srand(time(nullptr));

    // Hằng số
    const int PLAYER_SIZE = 25;
    const int GROUND_LEVEL = SCREEN_HEIGHT - PLAYER_SIZE - 20;
    const float OBSTACLE_SPEED = 5.0f;
    const float JUMP_FORCE = -12.0f;
    const float GRAVITY = 0.5f;
    const int TOP_LANDING_TOLERANCE = 15; // Cho phép player hạ cánh trên mặt trên khi khoảng cách <= 10 pixels
    const int MIN_OBSTACLE_HEIGHT = 75;  //Giúp game dễ thở hơn
    const int MAX_OBSTACLE_HEIGHT = 150;

    // Tạo player
    GameObject player;
    player.rect = {50, GROUND_LEVEL, PLAYER_SIZE, PLAYER_SIZE};
    player.color = COLOR_PINK;
    player.x_velocity = 0;
    player.y_velocity = 0;

    // Danh sách các chướng ngại vật
    std::vector<GameObject> obstacles;

    // Tạo mặt đất
    SDL_Rect groundRect = {0, GROUND_LEVEL + PLAYER_SIZE, SCREEN_WIDTH, 10};

    // Biến trò chơi
    bool running = true;
    bool isJumping = false;
    int score = 0;
    Uint32 scoreTimer = SDL_GetTicks();
    const Uint32 SCORE_INTERVAL = 500;
    float obstacleSpeed = OBSTACLE_SPEED;
    Uint32 obstacleTimer = SDL_GetTicks();
    const Uint32 OBSTACLE_SPAWN_INTERVAL = 1500;

    // Vòng lặp trò chơi
    while (running) {
        // Xử lý sự kiện
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE && !isJumping && player.rect.y <= GROUND_LEVEL) { //allow jump a bit high
                    isJumping = true;
                    player.y_velocity = JUMP_FORCE;
                    std::cout << "Nhảy!" << std::endl;
                }
            }
        }

        // Di chuyển và tạo chướng ngại vật
        if (SDL_GetTicks() - obstacleTimer > OBSTACLE_SPAWN_INTERVAL) {
            obstacleTimer = SDL_GetTicks();
            GameObject obstacle;
            obstacle.rect = {SCREEN_WIDTH, GROUND_LEVEL - MIN_OBSTACLE_HEIGHT - (rand() % MAX_OBSTACLE_HEIGHT), 50 + (rand() % 50), 50};
            obstacle.color = (rand() % 2 == 0) ? COLOR_RED : COLOR_BLUE;
            obstacle.x_velocity = -obstacleSpeed;
            obstacle.y_velocity = 0;
            obstacles.push_back(obstacle);
        }

        // Cập nhật chướng ngại vật
        for (auto it = obstacles.begin(); it != obstacles.end();) {
            it->rect.x += it->x_velocity;
            if (it->rect.x < -it->rect.w) {
                it = obstacles.erase(it);
            } else {
                ++it;
            }
        }

        //Cập nhật player theo chiều dọc, sử dụng trọng lực
        player.y_velocity += GRAVITY;
        player.rect.y += player.y_velocity;

        //Xử lý va chạm với mặt đất
        if (player.rect.y > GROUND_LEVEL) {
            player.rect.y = GROUND_LEVEL;
            isJumping = false;
            player.y_velocity = 0;
        }

        //Kiểm tra và xử lý tiếp đất từ phía trên
        bool landedOnObstacle = false; // Cờ để đảm bảo chỉ chạm 1 vật cản
        for (auto& obstacle : obstacles) {
          if (player.rect.x + player.rect.w > obstacle.rect.x &&
              player.rect.x < obstacle.rect.x + obstacle.rect.w &&
              player.rect.y + player.rect.h > obstacle.rect.y &&  //gần chạm đỉnh trên
              player.y_velocity > 0 && //đang rơi xuống
              player.rect.y + player.rect.h <= obstacle.rect.y + 5 //đang nằm trong khoảng dung sai
              ) {

              std::cout << "Đang cố tiếp đất trên chướng ngại vật!" << std::endl;
              std::cout << "Vận tốc y của player: " << player.y_velocity << std::endl;
              std::cout << "Khoảng cách tới đỉnh chướng ngại vật: " << (obstacle.rect.y - (player.rect.y + PLAYER_SIZE)) << std::endl;

              //Coi như đã hạ cánh trên đỉnh
              player.rect.y = obstacle.rect.y - PLAYER_SIZE;
              player.y_velocity = 0;
              isJumping = false;
              landedOnObstacle = true; //Đảm bảo không nhảy liên tục lên các vật cản khác

              // Đảm bảo nhảy ra khỏi chướng ngại vật
              player.rect.y -= 1;

              std::cout << "Đã tiếp đất trên đỉnh!" << std::endl;
              std::cout << "Vị trí y của player: " << player.rect.y << std::endl;
              std::cout << "Vị trí y của obstacle: " << obstacle.rect.y << std::endl;
              break;
            }
        }

        // Kiểm tra va chạm với các mặt bên của chướng ngại vật (Sau khi di chuyển)
        if(!landedOnObstacle){ //Nếu không, tiếp tục kiểm tra va chạm
          for (const auto& obstacle : obstacles) {
              if (checkCollision(player.rect, obstacle.rect) && player.rect.y != obstacle.rect.y - PLAYER_SIZE) {
                  std::cout << "Va chạm mặt bên! Game Over!" << std::endl;
                  //Đặt lại trò chơi
                  score = 0;
                  obstacleSpeed = OBSTACLE_SPEED;
                  obstacles.clear();
                  player.rect = {50, GROUND_LEVEL, PLAYER_SIZE, PLAYER_SIZE};
                  player.y_velocity = 0;
                  isJumping = false;
                  obstacleTimer = SDL_GetTicks();
                  break;
              }
          }
        }

        // Cập nhật điểm
        if (SDL_GetTicks() - scoreTimer > SCORE_INTERVAL) {
            scoreTimer = SDL_GetTicks();
            score++;
            obstacleSpeed += 0.05f;
        }

        // Vẽ
        SDL_SetRenderDrawColor(renderer, COLOR_TEAL.r, COLOR_TEAL.g, COLOR_TEAL.b, COLOR_TEAL.a);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b, COLOR_GREEN.a);
        SDL_RenderFillRect(renderer, &groundRect);

        SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, player.color.a);
        SDL_RenderFillRect(renderer, &player.rect);

        for (const auto& obstacle : obstacles) {
            SDL_SetRenderDrawColor(renderer, obstacle.color.r, obstacle.color.g, obstacle.color.b, obstacle.color.a);
            SDL_RenderFillRect(renderer, &obstacle.rect);
        }

        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Surface* textSurface = SDL_CreateRGBSurface(0, scoreText.length() * 10, 20, 32, 0, 0, 0, 0);
        SDL_Color textColor = COLOR_BLACK;

        if (textSurface != nullptr) {
            SDL_FillRect(textSurface, NULL, SDL_MapRGB(textSurface->format, 0, 0, 0));
            SDL_Rect textRect = {0, 0, textSurface->w, textSurface->h};

            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect renderRect = {10, 10, textSurface->w, textSurface->h};

            SDL_RenderCopy(renderer, textTexture, NULL, &renderRect);

            SDL_DestroyTexture(textTexture);
            SDL_FreeSurface(textSurface);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    // Giải phóng tài nguyên
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
