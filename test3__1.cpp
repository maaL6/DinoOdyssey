#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> // Include SDL_image
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

// Hàm kiểm tra va chạm (AABB)
bool checkAABBCollision(const SDL_Rect& a, const SDL_Rect& b) {
// Kiểm tra xem có khoảng trống giữa các hình chữ nhật trên mỗi trục không
    if (a.x + a.w < b.x || b.x + b.w < a.x || a.y + a.h < b.y || b.y + b.h < a.y) {
        return false;
    }
    return true;
}

// Hàm đặt lại trò chơi (tùy chọn)
void resetGame(GameObject& player, std::vector<GameObject>& obstacles, bool& running, bool& isJumping, int& score, float& obstacleSpeed, int& currentGroundLevel) {
    score = 0;
    obstacleSpeed = 5.0f; // Đặt lại tốc độ
    obstacles.clear();
    running = true;
    isJumping = false;

    const int PLAYER_SIZE = 25; // Định nghĩa lại PLAYER_SIZE ở đây
    const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE - 20;  // Nền thấp
    const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100; // Nền cao (ví dụ)

// Tạo nền ban đầu
    currentGroundLevel = (rand() % 2 == 0) ? GROUND_LEVEL_LOW : GROUND_LEVEL_HIGH; //Random nền low hay high

//Đặt lại vị trí người chơi trên nền ban đầu
    player.rect = {50, currentGroundLevel - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE};
    player.y_velocity = 0;

// Tạo nền ban đầu
    GameObject initialPlatform;
    initialPlatform.rect = {0, currentGroundLevel, 150, 10};
    initialPlatform.color = COLOR_RED;
    initialPlatform.x_velocity = -obstacleSpeed;
    initialPlatform.y_velocity = 0;
    obstacles.push_back(initialPlatform);

}

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    //Khởi tạo SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Endless Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        IMG_Quit(); // Đóng SDL_image nếu SDL_CreateWindow lỗi.
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit(); // Đóng SDL_image nếu SDL_CreateRenderer lỗi.
        return 1;
    }

    // Khởi tạo bộ sinh số ngẫu nhiên
    srand(time(nullptr));

    // Load texture "ground.png"
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
        IMG_Quit(); // Đóng SDL_image nếu SDL_CreateTextureFromSurface lỗi.
        return 1;
    }

    // Hằng số
    const int PLAYER_SIZE = 25;
    const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE - 20;  // Nền thấp
    const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100; // Nền cao (ví dụ)
    const float OBSTACLE_SPEED = 5.0f;
    const float JUMP_FORCE = -12.0f;
    const float GRAVITY = 0.5f;
    const Uint32 OBSTACLE_SPAWN_INTERVAL = 1500;
    const int PLATFORM_HEIGHT = 10;
    const int MAX_PLATFORM_GAP = 10; // Khoảng cách tối đa giữa các nền

    // Tạo player
    GameObject player;
    player.color = COLOR_PINK;
    player.x_velocity = 0;
    player.y_velocity = 0;

    // Danh sách các chướng ngại vật
    std::vector<GameObject> obstacles;

    // Biến trò chơi
    bool running = true;
    bool isJumping = false;
    int score = 0;
    Uint32 scoreTimer = SDL_GetTicks();
    const Uint32 SCORE_INTERVAL = 500;
    float obstacleSpeed = OBSTACLE_SPEED;
    Uint32 obstacleTimer = SDL_GetTicks();
    int currentGroundLevel = GROUND_LEVEL_LOW; // Khởi tạo với nền thấp

    // Tạo nền ban đầu
    resetGame(player, obstacles, running, isJumping, score, obstacleSpeed, currentGroundLevel);

    //Theo dõi vị trí nền cuối cùng
    float lastPlatformX = 0;
    if (!obstacles.empty()) {
        lastPlatformX = obstacles.back().rect.x + obstacles.back().rect.w;
    }

    //Khai báo biến để theo dõi offset cuộn
    float scrollOffset = 0.0f;

    //Khai báo groundWidth ở scope lớn hơn
    int groundWidth;

    // Vòng lặp trò chơi
    while (running) {
        // Xử lý sự kiện
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

        // Tạo nền mới khi cần thiết
        // if (lastPlatformX < SCREEN_WIDTH + MAX_PLATFORM_GAP) {
        GameObject obstacle;
        int obstacleWidth = 500 + (rand() % 50);

        // Luân phiên giữa nền cao và thấp
        if (currentGroundLevel == GROUND_LEVEL_LOW) {
            currentGroundLevel = GROUND_LEVEL_HIGH;
        } else {
            currentGroundLevel = GROUND_LEVEL_LOW;
        }

        //Tính toán vị trí X cho nền mới, đảm bảo không quá xa nền trước đó
        int newPlatformX = lastPlatformX + (rand() % MAX_PLATFORM_GAP) + 75; //50 là khoảng cách tối thiểu
        obstacle.rect = {newPlatformX, currentGroundLevel, obstacleWidth, PLATFORM_HEIGHT};
        obstacle.color = (rand() % 2 == 0) ? COLOR_RED : COLOR_BLUE;
        obstacle.x_velocity = -obstacleSpeed;
        obstacle.y_velocity = 0;
        obstacles.push_back(obstacle);
        lastPlatformX = obstacle.rect.x + obstacle.rect.w;
        // }

        // Cập nhật chướng ngại vật
        for (auto it = obstacles.begin(); it != obstacles.end();) {
            it->rect.x += it->x_velocity;

            // Xóa nền nếu nó đã đi ra khỏi màn hình
            if (it->rect.x + it->rect.w < 0) {
                it = obstacles.erase(it);
            } else {
                ++it;
            }
        }

        //Cập nhật player theo chiều dọc, sử dụng trọng lực
        player.y_velocity += GRAVITY;
        player.rect.y += player.y_velocity;

        // Biến để theo dõi xem player có đang đứng trên nền nào không
        bool isOnGround = false;
        bool collided = true;

        //Kiểm tra va chạm và tiếp đất trên nền
        for (const auto& obstacle : obstacles) {
            if (checkAABBCollision(player.rect, obstacle.rect)) {
                collided = false;

                //Nếu player đang rơi
                if (player.y_velocity >= 0) {
                    // Nếu cạnh dưới của player chạm vào cạnh trên của nền
                    if (player.rect.y + player.rect.h > obstacle.rect.y/* && player.rect.y + player.rect.h < obstacle.rect.y + obstacle.rect.h*/) {
                        player.rect.y = obstacle.rect.y - player.rect.h;
                        player.y_velocity = 0;
                        isJumping = false;
                        isOnGround = true;
                    }
                }
            }
        }

        //Nếu không có nền nào đỡ, thì game over
        if (!isOnGround && player.rect.y > SCREEN_HEIGHT) {
            std::cout << "Rơi khỏi nền! Game Over!" << std::endl;
            running = false; // Dừng vòng lặp trò chơi
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

        // Vẽ các nền (đã sửa để cuộn ảnh)
        for (const auto& obstacle : obstacles) {
            SDL_QueryTexture(groundTexture, NULL, NULL, &groundWidth, NULL); // Lấy chiều rộng của texture

            //Tạo hình chữ nhật nguồn
            SDL_Rect sourceRect;
            sourceRect.x = (int)scrollOffset % groundWidth; // Cuộn theo chiều ngang
            sourceRect.y = 0;
            sourceRect.w = obstacle.rect.w; // Chiều rộng bằng chiều rộng của nền
            sourceRect.h = obstacle.rect.h; // Chiều cao bằng chiều cao của nền

            // Vẽ ảnh với hình chữ nhật nguồn đã chỉ định
            SDL_RenderCopy(renderer, groundTexture, &sourceRect, &obstacle.rect);
        }

        //Cập nhật offset cuộn
        scrollOffset -= 0.5f; // Tốc độ cuộn (có thể điều chỉnh)
        if (scrollOffset < 0) {
            scrollOffset += groundWidth;
        }

        //Vẽ player
        SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, player.color.a);
        SDL_RenderFillRect(renderer, &player.rect);

        // Vẽ điểm
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
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    //Giải phóng texture
    SDL_DestroyTexture(groundTexture);

    //Hiển thị thông báo game over (tùy chọn)
    if (!running) {
        std::cout << "Game Over! Score: " << score << std::endl;
        SDL_Delay(1000); // Dừng 2 giây trước khi thoát
    }

    // Giải phóng tài nguyên
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit(); //Đóng SDL_image trước khi SDL_Quit
    SDL_Quit();

    return 0;
}
