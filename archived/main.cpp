#include "Game_Base.h"
#include "Game_Utils.h"
#include "LTexture.h"
#include "Elements.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

void waitUntilKeyPressed() {
    SDL_Event e;
    while (true) {
        if ( SDL_PollEvent(&e) != 0 &&
                (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

const SDL_Color COLOR_RED = {255, 0, 0, 255};
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};

int SPIKE_SIZE = 50;
const int PLAYER_SIZE = 57;
const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100;  // Nền thấp
const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE; // Nền cao (ví dụ)
const float OBSTACLE_SPEED = 5.0f;
const float JUMP_FORCE = -14.0f;
const float GRAVITY = 0.5f;
const Uint32 OBSTACLE_SPAWN_INTERVAL = 1500;
const int PLATFORM_HEIGHT = 10;
const float SPIKE_PROBABILITY = 0.9f;
const int MAX_SPIKES_PER_PLATFORM = 7;
const int MAX_PLATFORM_GAP = 10;
const int SPIKE2_FRAME_COUNT = 5; // Số frame trong sprite sheet của spike2
const int ANIMATION_SPEED = 10; // Thời gian chuyển frame (ms)
const int BULLET_SIZE = 10; // Kích thước đạn
const float BULLET_SPEED = 10.0f; // Tốc độ đạn
const int BULLET_RANGE = 500;


int currentGroundLevel = GROUND_LEVEL_LOW;

int characterSize = 57;
int characterHeigh = characterSize;
int characterWidth = characterSize;
int characterPosX = SCREEN_WIDTH - 800;
int characterPosY = currentGroundLevel;
float characterVelocity = 0;

int JUMP_SPEED = 8;
int FALL_SPEED = 8;

bool isOnGround = false;
bool collided = true;
bool running = true;
bool isJumping = false;
float obstacleSpeed = 5.0f;
const Uint32 SCORE_INTERVAL = 500;
Uint32 obstacleTimer = SDL_GetTicks();
Uint32 animationTimer = SDL_GetTicks();

SDL_Texture* groundTexture = nullptr;
SDL_Texture* spikeTexture = nullptr;
SDL_Texture* spike2SheetTexture = nullptr;

void LoadRunGround() {
    SDL_Surface* groundSurface = IMG_Load("ground.png");
    if (groundSurface == nullptr) {
        std::cerr << "Failed to load ground.png!" << std::endl;
        std::cerr << "SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        IMG_Quit();
        return ;
    }
    groundTexture = SDL_CreateTextureFromSurface(gRenderer, groundSurface);
    SDL_FreeSurface(groundSurface);

    if (groundTexture == nullptr) {
        std::cerr << "Failed to create texture from ground.png! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        IMG_Quit(); // Đóng SDL_image nếu SDL_CreateTextureFromSurface lỗi.
        return ;
    }
}

void LoadSpike() {
    SDL_Surface* spikeSurface = IMG_Load("spike.png");
    if (!spikeSurface) {
        std::cerr << "Failed to load spike.png! IMG_Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return;
    }
    spikeTexture = SDL_CreateTextureFromSurface(gRenderer, spikeSurface);
    SDL_FreeSurface(spikeSurface);
    if (!spikeTexture) {
        std::cerr << "Failed to create texture from spike.png! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return;
    }
}

void LoadSpike2() {
    SDL_Surface* spike2SheetSurface = IMG_Load("spike2_sheet.png");
    if (!spike2SheetSurface) {
        std::cerr << "Failed to load spike2_sheet.png! IMG_Error: " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(spikeTexture);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return;
    }
    spike2SheetTexture = SDL_CreateTextureFromSurface(gRenderer, spike2SheetSurface);
    SDL_FreeSurface(spike2SheetSurface);
    if (!spike2SheetTexture) {
        std::cerr << "Failed to create texture from spike2_sheet.png! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyTexture(spikeTexture);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return;
    }
}

bool main_OnGround() {
    return characterPosY == GROUND;
}

void main_HandleEvent(SDL_Event& e, Mix_Chunk *gJump) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_SPACE && !isJumping) {
            isJumping = true;
            characterVelocity = JUMP_FORCE;
            //std::cout << "Nhảy!" << std::endl;
        }
    }
}

/*
void main_Move() {
    characterVelocity += GRAVITY;
    characterPosY += characterVelocity;
}
*/

void main_Render(SDL_Rect* currentClip, SDL_Renderer *gRenderer, LTexture& gCharacterTexture) {
    //gCharacterTexture.Render(characterPosX, characterPosY, gRenderer, currentClip);
    SDL_Rect renderSpace = {characterPosX, characterPosY, characterWidth, characterHeigh};

    SDL_RenderCopy(gRenderer, gCharacterTexture.mTexture, currentClip, &renderSpace);
}

struct GameObject {
    SDL_Rect rect;
    SDL_Color color;
    float x_velocity;
    float y_velocity;
    bool isLethal;
    int currentFrame;
    int type;
};

struct Bullet {
    SDL_Rect rect;
    float x_velocity;
    float y_velocity;
    float startX;
};

void resetGame(GameObject& player, std::vector<GameObject>& obstacles, std::vector<GameObject>& spikes,bool& running, bool& isJumping, float& obstacleSpeed, int& currentGroundLevel) {
    //score = 0;
    obstacleSpeed = 5.0f; // Đặt lại tốc độ
    obstacles.clear();
    spikes.clear();
    running = true;
    isJumping = false;

    int PLAYER_SIZE = 57;
// Tạo nền ban đầu
    currentGroundLevel = GROUND_LEVEL_LOW;

// Tạo nền ban đầu
    GameObject initialPlatform;
    initialPlatform.rect = {0, GROUND_LEVEL_LOW, 1500, 10};
    initialPlatform.color = COLOR_RED;
    initialPlatform.x_velocity = -obstacleSpeed;
    initialPlatform.y_velocity = 0;
    obstacles.push_back(initialPlatform);

//Đặt lại vị trí người chơi trên nền ban đầu
    player.rect = {characterPosX, characterPosY, PLAYER_SIZE, PLAYER_SIZE};
    player.y_velocity = 0;

    characterPosX = SCREEN_WIDTH - 800;
    characterPosY = currentGroundLevel - 100;
    characterVelocity = 0;
    //Clear CHUONG NGAI VAT
    spikes.clear();

}

bool checkAABBCollision(const SDL_Rect& a, const SDL_Rect& b) {
// Kiểm tra xem có khoảng trống giữa các hình chữ nhật trên mỗi trục không
    if (a.x + a.w < b.x || b.x + b.w < a.x || a.y + a.h < b.y || b.y + b.h < a.y) {
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    GameObject player;
    player.color = COLOR_RED;
    player.x_velocity = 0;
    player.y_velocity = 0;

    vector<GameObject> obstacles;
    vector<GameObject> spikes;
    vector<Bullet> bullets;
    //LoadGround();

    int frame_Labtube = 0;
    PlayButton.SetInteract(29, 23, 120, 40);
    HelpButton.SetInteract(29, 23, 120, 40);
    ExitButton.SetInteract(29, 23, 120, 40);
    CloseButton.SetInteract(15, 15, 30, 30);
    SettingButton.SetInteract(15, 15, 30, 30);
    MenuButton.SetInteract(15, 15, 30, 30);

    //frame
    int currentFrame = 0;
    int frameDelay = 200;
    Uint32 lastFrameTime = SDL_GetTicks();

    if (!Init()) {
        printf("Failed to initialize!\n");
    } else {
        if (!LoadMedia()) {
            printf("Failed to load media!\n");
        } else {
            LoadRunGround();
            LoadSpike();
            LoadSpike2();

            bool Play_Again = false;
            bool Quit_Menu = false;

            Mix_PlayMusic(gMenuMusic, IS_REPEATITIVE);
            while (!Quit_Menu) {
                SDL_Event e_mouse;
                while (SDL_PollEvent(&e_mouse) != 0) {
                    if (e_mouse.type == SDL_QUIT) {
                        Quit_Menu = true;
                    }

                    bool Quit_Game = false;
                    HandlePlayButton(&e_mouse, PlayButton, Quit_Menu, Play_Again, gClick);

                    HandleHelpButton(&e_mouse, gBackButton,
                                     HelpButton, BackButton,
                                     gInstructionTexture, gBackButtonTexture,
                                     gRenderer, Quit_Game, gClick);

                    //HandleExitButton(&e_mouse, ExitButton, Quit_Menu, gClick);
                    HandleCloseButton(&e_mouse, CloseButton, Quit_Menu, gClick);
                    HandleSettingButton(&e_mouse, SettingButton, gClick);
                    HandleMenuButton(&e_mouse, MenuButton, gClick);
                    if (Quit_Game == true) {
                        return 0;
                    }
                }

                gMenuTexture.Render(0, 0, gRenderer);

                gTransTexture.Render(0, 0, gRenderer);
                //labtube

                //SDL_RenderCopy(gRenderer, gLabtubeTexture.mTexture, currentClip_Labtube, &renderSpace);
                //ControlLabtubeFrame(frame_Labtube);
                //SDL_Delay(2000);
                //


                // Cập nhật frame
                Uint32 currentTime = SDL_GetTicks();
                SDL_Rect renderSpace = {0, 0, 800, 500};
                if (currentTime - lastFrameTime >= frameDelay) {
                    currentFrame = (currentFrame + 1) % 3; // Chuyển frame, quay lại 0 khi tới 3
                    lastFrameTime = currentTime;
                }
                SDL_RenderCopy(gRenderer, gLabtubeTexture.mTexture, &gLabtube[currentFrame], &renderSpace);


                //end labtube
                gVineTexture.Render(0, 0, gRenderer);
                gNameTexture.Render(0, 0, gRenderer);

                //Button
                SDL_Rect* currentClip_Close = &gCloseButton[CloseButton.currentSprite];
                CloseButton.Render(currentClip_Close, gRenderer, gCloseButtonTexture);

                SDL_Rect* currentClip_Setting = &gSettingButton[SettingButton.currentSprite];
                SettingButton.Render(currentClip_Setting, gRenderer, gSettingButtonTexture);

                SDL_Rect* currentClip_Menu = &gMenuButton[MenuButton.currentSprite];
                MenuButton.Render(currentClip_Menu, gRenderer, gMenuButtonTexture);


                //
                SDL_Rect* currentClip_Play = &gPlayButton[PlayButton.currentSprite];
                PlayButton.Render(currentClip_Play, gRenderer, gPlayButtonTexture);

                SDL_Rect* currentClip_Help = &gHelpButton[HelpButton.currentSprite];
                HelpButton.Render(currentClip_Help, gRenderer, gHelpButtonTexture);

                //SDL_Rect* currentClip_Exit = &gExitButton[ExitButton.currentSprite];
                //ExitButton.Render(currentClip_Exit, gRenderer, gExitButtonTexture);

                SDL_RenderPresent(gRenderer);
            }

            while (Play_Again) {
                srand(time(NULL));
                int time = 0;
                int score = 0;
                int acceleration = 0;
                int frame_Character = 0;
                int frame_Enemy = 0;
                string highscore = GetHighScoreFromFile("high_score.txt");

                GameObject obstacle;
                obstacles.erase(obstacles.begin(), obstacles. end());
                resetGame(player, obstacles, spikes,running, isJumping, obstacleSpeed, currentGroundLevel);

                float lastPlatformX = 0;
                if (!obstacles.empty()) {
                    lastPlatformX = obstacles.back().rect.x + obstacles.back().rect.w;
                }

                SDL_Event e;
                //Enemy enemy1(ON_GROUND_ENEMY);
                //Enemy enemy2(ON_GROUND_ENEMY);
                //Enemy enemy3(IN_AIR_ENEMY);

                Mix_PlayMusic(gMusic, IS_REPEATITIVE);
                //GenerateEnemy(enemy1, enemy2, enemy3, gEnemyClips, gRenderer);


                int OffsetSpeed_Ground = BASE_OFFSET_SPEED;
                std::vector <double> OffsetSpeed_Bkgr(BACKGROUND_LAYER, BASE_OFFSET_SPEED);

                bool Quit = false;
                bool Game_State = true;
                while (!Quit) {

                    if (Game_State) { // LUU Y BIEN RUNNING
                        UpdateGameTimeAndScore(time, acceleration, score);
                        while (SDL_PollEvent(&e) != 0) {
                            if (e.type == SDL_QUIT) {
                                running = false;
                                Quit = true;
                                Play_Again = false;
                            }
                            HandlePauseButton(&e, gRenderer, gContinueButton,
                                              PauseButton, ContinueButton,
                                              gContinueButtonTexture, Game_State, gClick);

                            if (e.type == SDL_KEYDOWN) {
                                if (e.key.keysym.sym == SDLK_SPACE && !isJumping) {
                                    isJumping = true;
                                    Mix_PlayChannel(MIX_CHANNEL, gJump, NOT_REPEATITIVE);
                                    characterVelocity = JUMP_FORCE;
                                    player.y_velocity = JUMP_FORCE;
                                    //std::cout << "Nhảy!" << std::endl;
                                }
                                if (e.key.keysym.sym == SDLK_p) {
                                    Bullet bullet;
                                    bullet.rect = {player.rect.x + player.rect.w, player.rect.y + (player.rect.h - BULLET_SIZE) / 2, BULLET_SIZE, BULLET_SIZE};
                                    bullet.x_velocity = BULLET_SPEED;
                                    bullet.y_velocity = 0;
                                    bullets.push_back(bullet);
                                    std::cout << "Bắn đạn!" << std::endl;
                                    //SDL_Delay(100);
                                }
                            }

                        }


                        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                        SDL_RenderClear(gRenderer);

                        //RenderScrollingBackground(OffsetSpeed_Bkgr, gBackgroundTexture, gRenderer);
                        //RenderScrollingGround(OffsetSpeed_Ground, acceleration, gGroundTexture, gRenderer);


                        int obstacleWidth = 3000 + (rand() % 500);

                        // Luân phiên giữa nền cao và thấp
                        if (currentGroundLevel == GROUND_LEVEL_LOW) {
                            currentGroundLevel = GROUND_LEVEL_HIGH;
                        } else {
                            currentGroundLevel = GROUND_LEVEL_LOW;
                        }

                        //Tính toán vị trí X cho nền mới, đảm bảo không quá xa nền trước đó
                        int newPlatformX = lastPlatformX + (rand() % MAX_PLATFORM_GAP) + 200; //50 là khoảng cách tối thiểu
                        obstacle.rect = {newPlatformX, currentGroundLevel, obstacleWidth, 100};
                        obstacle.color = (rand() % 2 == 0) ? COLOR_RED : COLOR_RED;
                        obstacle.x_velocity = -obstacleSpeed;
                        obstacle.y_velocity = 0;
                        obstacles.push_back(obstacle);
                        lastPlatformX = obstacle.rect.x + obstacle.rect.w;
                        // }

                        // Thêm spikes (ngẫu nhiên spike hoặc spike2)
                        if ((float)rand() / RAND_MAX < SPIKE_PROBABILITY) {
                            int numSpikes = 1 + (rand() % MAX_SPIKES_PER_PLATFORM);
                            int spikeSpacing = obstacleWidth / (numSpikes + 1);
                            for (int i = 1; i <= numSpikes; ++i) {
                                GameObject spike;
                                spike.rect = {newPlatformX + i * spikeSpacing - SPIKE_SIZE / 2, currentGroundLevel - SPIKE_SIZE, SPIKE_SIZE, SPIKE_SIZE};
                                spike.x_velocity = -obstacleSpeed;
                                spike.y_velocity = 0;
                                spike.isLethal = true;
                                spike.currentFrame = 0;
                                spike.type = (rand() % 2); // 0: spike tĩnh, 1: spike2 hoạt hình
                                spikes.push_back(spike);
                            }
                        }

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

                        // Cập nhật spikes và hoạt hình cho spike2
                        Uint32 currentTime = SDL_GetTicks();
                        if (currentTime - animationTimer > ANIMATION_SPEED) {
                            for (auto& spike : spikes) {
                                if (spike.type == 1) { // Chỉ cập nhật frame cho spike2
                                    spike.currentFrame = (spike.currentFrame + 1) % SPIKE2_FRAME_COUNT;
                                }
                            }
                            animationTimer = currentTime;
                        }
                        for (auto it = spikes.begin(); it != spikes.end();) {
                            it->rect.x += it->x_velocity;
                            if (it->rect.x + it->rect.w < 0) {
                                it = spikes.erase(it);
                            } else {
                                ++it;
                            }
                        }
                        // Cập nhật đạn
                        for (auto it = bullets.begin(); it != bullets.end();) {
                            it->rect.x += it->x_velocity;
                            bool hitSpike2 = false;
                            for (auto spikeIt = spikes.begin(); spikeIt != spikes.end();) {
                                if (spikeIt->type == 1 && checkAABBCollision(it->rect, spikeIt->rect)) { // Chỉ ảnh hưởng spike2
                                    spikeIt = spikes.erase(spikeIt);
                                    hitSpike2 = true;
                                    break;
                                } else {
                                    ++spikeIt;
                                }
                            }
                            // Kiểm tra khoảng cách (từ startX đến vị trí hiện tại)
                            float distanceTraveled = it->rect.x - it->startX;
                            if (hitSpike2 || distanceTraveled > BULLET_RANGE) {
                                it = bullets.erase(it);
                            } else {
                                ++it;
                            }
                        }

                        //Ve nen
                        for (auto& obstacle : obstacles) {
                            SDL_RenderCopy(gRenderer, groundTexture, NULL, &obstacle.rect);

                        }
                        //Ve spike
                        SDL_Rect srcRect = {0, 0, 15, 15}; // Kích thước frame gốc từ sprite sheet (15x15)
                        for (const auto& spike : spikes) {
                            SDL_Rect destRect = spike.rect; // Vị trí và kích thước đích
                            destRect.w = SPIKE_SIZE; // Điều chỉnh kích thước đích
                            destRect.h = SPIKE_SIZE;
                            destRect.x = spike.rect.x + (spike.rect.w - SPIKE_SIZE) / 2; // Căn giữa
                            destRect.y = spike.rect.y + (spike.rect.h - SPIKE_SIZE) / 2;

                            if (spike.type == 0) {
                                // Spike tĩnh
                                SDL_RenderCopy(gRenderer, spikeTexture, nullptr, &destRect);
                            } else {
                                // Spike2 hoạt hình
                                srcRect.x = spike.currentFrame * 15; // Chọn frame từ sprite sheet
                                SDL_RenderCopy(gRenderer, spike2SheetTexture, &srcRect, &destRect);
                            }
                        }

                        for (const auto& bullet : bullets) {
                            SDL_SetRenderDrawColor(gRenderer, COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b, COLOR_BLACK.a);
                            SDL_RenderFillRect(gRenderer, &bullet.rect);
                        }

                        //SDL_RenderPresent(gRenderer);
                        //chuyen dong
                        characterVelocity += GRAVITY;
                        characterPosY += characterVelocity;

                        player.y_velocity += GRAVITY;
                        player.rect.y += player.y_velocity;
                        //
                        SDL_Rect* currentClip_Pause = &gPauseButton[PauseButton.currentSprite];
                        PauseButton.Render(currentClip_Pause, gRenderer, gPauseButtonTexture);

                        DrawPlayerScore(gText1Texture, gScoreTexture, textColor, gRenderer, gFont, score);
                        DrawPlayerHighScore(gText2Texture, gHighScoreTexture, textColor, gRenderer, gFont, highscore);

                        isOnGround = false;
                        collided = true;
                        SDL_Rect characterr = {characterHeigh, characterWidth, characterPosX, characterPosY};

                        cout << player.rect.h << " " << player.rect.w << " " << player.rect.x << " " << player.rect.y << endl;
                        cout << characterHeigh << " " << characterWidth << " " << characterPosX << " " << characterPosY << endl;
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
                                if (characterVelocity >= 0) {
                                    // Nếu cạnh dưới của player chạm vào cạnh trên của nền
                                    if (characterPosY + characterHeigh > obstacle.rect.y/* && player.rect.y + player.rect.h < obstacle.rect.y + obstacle.rect.h*/) {
                                        characterPosY = obstacle.rect.y - characterHeigh;
                                        characterVelocity = 0;
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
                                Quit = true;
                                break;
                            }
                        }
                        /*
                        if (!isOnGround && player.rect.y > SCREEN_HEIGHT) {
                            std::cout << "Rơi khỏi nền! Game Over!" << std::endl;
                            //running = false; // Dừng vòng lặp trò chơi
                        }*/
                        if (!isOnGround && characterPosY > SCREEN_HEIGHT) {
                            std::cout << "Rơi khỏi nền! Game Over!" << std::endl;
                            //running = false; // Dừng vòng lặp trò chơi
                            Mix_PauseMusic();
                            Mix_PlayChannel(MIX_CHANNEL, gLose, NOT_REPEATITIVE);
                            UpdateHighScore("high_score.txt", score, highscore);
                            Quit = true;
                        }

                        // Sinh khung

                        //SDL_SetRenderDrawColor(gRenderer, player.color.r, player.color.g, player.color.b, player.color.a);
                        //SDL_RenderFillRect(gRenderer, &player.rect);

                        //
                        SDL_Rect* currentClip_Character = nullptr;
                        if (true) {
                            currentClip_Character = &gCharacterClips[frame_Character / SLOW_FRAME_CHAR];
                            //main_Render(currentClip_Character, gRenderer, gCharacterTexture); // Sử dụng hàm mới
                            SDL_Rect renderSpace = {characterPosX, characterPosY, characterWidth, characterHeigh};
                            SDL_RenderCopy(gRenderer, gCharacterTexture.mTexture, currentClip_Character, &renderSpace);
                        } else {
                            //currentClip_Character = &gCharacterClips[0];
                            //main_Render(currentClip_Character, gRenderer, gCharacterTexture); // Sử dụng hàm mới
                        }
                        //cout << currentClip_Character->h << " " << currentClip_Character->w;
                        SDL_RenderPresent(gRenderer);

                        ControlCharFrame(frame_Character);
                        //ControlEnemyFrame(frame_Enemy);

                    }
                }

                DrawEndGameSelection(gLoseTexture, &e, gRenderer, Play_Again);
                if (Play_Again) {
                    bool End_Game = false;
                    while (!End_Game) {
                        while (SDL_PollEvent(&e) != 0) {
                            if (e.type == SDL_QUIT) {
                                Play_Again = false;
                            }

                            if (e.type == SDL_KEYDOWN) {
                                switch (e.key.keysym.sym) {
                                case SDLK_SPACE: {
                                    //resetGame(player, obstacles, running, isJumping, obstacleSpeed, currentGroundLevel);
                                    obstacles.erase(obstacles.begin(), obstacles. end());
                                    resetGame(player, obstacles, spikes, running, isJumping, obstacleSpeed, currentGroundLevel);

                                    End_Game = true;


                                    break;
                                }
                                case SDLK_ESCAPE:
                                    End_Game = true;
                                    Play_Again = false;
                                    break;
                                }
                            }
                        }

                        gLoseTexture.Render(0, 0, gRenderer);

                        SDL_RenderPresent(gRenderer);
                    }
                } else {
                    /*
                    enemy1.~Enemy();
                    enemy2.~Enemy();
                    enemy3.~Enemy();
                    */
                }
            }
        }
    }
    //waitUntilKeyPressed();
    Close();

    return 0;
}


bool Init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO) < 0) {
        LogError("Can not initialize SDL.", SDL_ERROR);
        success = false;
    } else {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            std::cout << "Warning: Linear texture filtering not enabled!";
        }

        gWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            LogError("Can not create window", SDL_ERROR);
            success = false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL) {
                LogError("Can not create renderer", SDL_ERROR);
                success = false;
            } else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    LogError("Can not initialize SDL_image", IMG_ERROR);
                    success = false;
                }

                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
                    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
                    success = false;
                }

                if (TTF_Init() == -1) {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool LoadMedia() {
    bool success = true;

    gMusic = Mix_LoadMUS("sound/bkgr_audio.wav");
    if (gMusic == nullptr) {
        LogError("Failed to load background music", MIX_ERROR);
        success = false;
    }

    gMenuMusic = Mix_LoadMUS("sound/menu_audio.wav");
    if (gMenuMusic == nullptr) {
        LogError("Failed to load menu music", MIX_ERROR);
        success = false;
    }

    gClick = Mix_LoadWAV("sound/mouse_click.wav");
    if (gClick == nullptr) {
        LogError("Failed to load mouse click sound", MIX_ERROR);
        success = false;
    }

    gJump = Mix_LoadWAV("sound/jump_sound.wav");
    if (gJump == nullptr) {
        LogError("Failed to load jumping sound", MIX_ERROR);
        success = false;
    }

    gLose = Mix_LoadWAV("sound/lose_sound.wav");
    if (gLose == nullptr) {
        LogError("Failed to load lose sound", MIX_ERROR);
        success = false;
    }

    else {
        gFont = TTF_OpenFont("font/pixel_font.ttf", 28);
        if (gFont == NULL) {
            LogError("Failed to load font", MIX_ERROR);
            success = false;
        } else {
            if (!gText1Texture.LoadFromRenderedText("Your score: ", gFont, textColor, gRenderer)) {
                std::cout << "Failed to render text1 texture" << std::endl;
                success = false;
            }

            if (!gText2Texture.LoadFromRenderedText("High score: ", gFont, textColor, gRenderer)) {
                std::cout << "Failed to render text2 texture" << std::endl;
                success = false;
            }

            if (!gMenuTexture.LoadFromFile("images/bgg.png", gRenderer)) {
                std::cout << "Failed to load menu image" << std::endl;
                success = false;
            }

            if (!gInstructionTexture.LoadFromFile("imgs/background/instruction.png", gRenderer)) {
                std::cout << "Failed to load instruction image" << std::endl;
                success = false;
            }

            if (!gPlayButtonTexture.LoadFromFile("images/play.png", gRenderer)) {
                std::cout << "Failed to load play_button image" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gPlayButton[i].x = 175 * i;
                    gPlayButton[i].y = 0;
                    gPlayButton[i].w = 175;
                    gPlayButton[i].h = 90;
                }
            }

            if (!gHelpButtonTexture.LoadFromFile("images/help.png", gRenderer)) {
                std::cout << "Failed to load help_button image" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gHelpButton[i].x = 175 * i;
                    gHelpButton[i].y = 0;
                    gHelpButton[i].w = 175;
                    gHelpButton[i].h = 90;
                }
            }

            if (!gBackButtonTexture.LoadFromFile("imgs/button/big_button/back_button.png", gRenderer)) {
                std::cout << "Failed to load back_button image" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gBackButton[i].x = 100 * i;
                    gBackButton[i].y = 0;
                    gBackButton[i].w = 100;
                    gBackButton[i].h = 78;
                }
            }

            if (!gExitButtonTexture.LoadFromFile("images/quit.png", gRenderer)) {
                std::cout << "Failed to load quit image" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gExitButton[i].x = 175 * i;
                    gExitButton[i].y = 0;
                    gExitButton[i].w = 175;
                    gExitButton[i].h = 90;
                }
            }

            //Start edit
            if (!gNameTexture.LoadFromFile("images/name.png", gRenderer)) {
                std::cout << "Failed to load name.png" << std::endl;
                success = false;
            }

            if (!gVineTexture.LoadFromFile("images/leo.png", gRenderer)) {
                std::cout << "Failed to load leo.png" << std::endl;
                success = false;
            }

            if (!gTransTexture.LoadFromFile("images/trans.png", gRenderer)) {
                std::cout << "Failed to load trans.png" << std::endl;
                success = false;
            }

            if (!gMenuButtonTexture.LoadFromFile("images/menu.png", gRenderer)) {
                std::cout << "Failed to load menu.png" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gMenuButton[i].x = 60 * i;
                    gMenuButton[i].y = 0;
                    gMenuButton[i].w = 60;
                    gMenuButton[i].h = 60;
                }
            }

            if (!gCloseButtonTexture.LoadFromFile("images/close.png", gRenderer)) {
                std::cout << "Failed to load close.png" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gCloseButton[i].x = 60 * i;
                    gCloseButton[i].y = 0;
                    gCloseButton[i].w = 60;
                    gCloseButton[i].h = 60;
                }
            }

            if (!gSettingButtonTexture.LoadFromFile("images/setting.png", gRenderer)) {
                std::cout << "Failed to load close.png" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gSettingButton[i].x = 60 * i;
                    gSettingButton[i].y = 0;
                    gSettingButton[i].w = 60;
                    gSettingButton[i].h = 60;
                }
            }

            if (!gSettingButtonTexture.LoadFromFile("images/setting.png", gRenderer)) {
                std::cout << "Failed to load setting.png" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gSettingButton[i].x = 60 * i;
                    gSettingButton[i].y = 0;
                    gSettingButton[i].w = 60;
                    gSettingButton[i].h = 60;
                }
            }

            if (!gSoundOnButtonTexture.LoadFromFile("images/sound_on.png", gRenderer)) {
                std::cout << "Failed to load sound_on.png" << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gSoundOnButton[i].x = 60 * i;
                    gSoundOnButton[i].y = 0;
                    gSoundOnButton[i].w = 60;
                    gSoundOnButton[i].h = 60;
                }
            }

            if (!gLabtubeTexture.LoadFromFile("images/labtube.png", gRenderer)) {
                std::cout << "Failed to load labtube.png " << std::endl;
                success = false;
            } else {
                for (int i = 0; i < 3; ++i) { //3: number of labtube frame
                    gLabtube[i].x = 800 * i;
                    gLabtube[i].y = 0;
                    gLabtube[i].w = 800;
                    gLabtube[i].h = 500;
                }
            }


            //End

            if (!gPauseButtonTexture.LoadFromFile("imgs/button/big_button/pause_button.png", gRenderer)) {
                std::cout << "Failed to load pause_button image " << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gPauseButton[i].x = 22 * i;
                    gPauseButton[i].y = 0;
                    gPauseButton[i].w = 22;
                    gPauseButton[i].h = 34;
                }
            }

            if (!gContinueButtonTexture.LoadFromFile("imgs/button/big_button/continue_button.png", gRenderer)) {
                std::cout << "Failed to load continue_button image " << std::endl;
                success = false;
            } else {
                for (int i = 0; i < BUTTON_TOTAL; ++i) {
                    gContinueButton[i].x = 22 * i;
                    gContinueButton[i].y = 0;
                    gContinueButton[i].w = 22;
                    gContinueButton[i].h = 34;
                }
            }

            for (int i = 0; i < BACKGROUND_LAYER; ++i) {
                if (!gBackgroundTexture[i].LoadFromFile(LAYER[i].c_str(), gRenderer)) {
                    std::cout << "Failed to load background image" << std::endl;
                    success = false;
                }
            }

            if (!gGroundTexture.LoadFromFile("imgs/background/ground.png", gRenderer)) {
                std::cout << "Failed to load ground image" << std::endl;
                success = false;
            }

            if (!gCharacterTexture.LoadFromFile("imgs/character/char.png", gRenderer)) {
                std::cout << "Failed to load character_run image." << std::endl;
                success = false;
            } else {
                gCharacterClips[0].x = 57 * 0;
                gCharacterClips[0].y = 0;
                gCharacterClips[0].w = 57;
                gCharacterClips[0].h = 57;

                gCharacterClips[1].x = 57 * 1;
                gCharacterClips[1].y = 0;
                gCharacterClips[1].w = 57;
                gCharacterClips[1].h = 57;

                gCharacterClips[2].x = 57 * 2;
                gCharacterClips[2].y = 0;
                gCharacterClips[2].w = 57;
                gCharacterClips[2].h = 57;

                gCharacterClips[3].x = 57 * 3;
                gCharacterClips[3].y = 0;
                gCharacterClips[3].w = 57;
                gCharacterClips[3].h = 57;

                gCharacterClips[4].x = 57 * 4;
                gCharacterClips[4].y = 0;
                gCharacterClips[4].w = 57;
                gCharacterClips[4].h = 57;

                gCharacterClips[5].x = 57 * 5;
                gCharacterClips[5].y = 0;
                gCharacterClips[5].w = 57;
                gCharacterClips[5].h = 57;
            }

            if (!gLoseTexture.LoadFromFile("imgs/background/lose.png", gRenderer)) {
                std::cout << "Failed to load lose image." << std::endl;
                success = false;
            }
        }
    }
    return success;
}

void Close() {

    gMenuTexture.Free();
    gInstructionTexture.Free();
    gCharacterTexture.Free();
    gGroundTexture.Free();
    gPlayButtonTexture.Free();
    gHelpButtonTexture.Free();
    gExitButtonTexture.Free();
    gBackButtonTexture.Free();
    gPauseButtonTexture.Free();
    gContinueButtonTexture.Free();
    gLoseTexture.Free();
    gText1Texture.Free();
    gScoreTexture.Free();
    gText2Texture.Free();
    gHighScoreTexture.Free();

    for (int i = 0; i < BACKGROUND_LAYER; ++i) {
        gBackgroundTexture[i].Free();
    }

    Mix_FreeMusic(gMusic);
    Mix_FreeMusic(gMenuMusic);
    Mix_FreeChunk(gClick);
    Mix_FreeChunk(gLose);
    Mix_FreeChunk(gJump);
    gMusic = nullptr;
    gMenuMusic = nullptr;
    gClick = nullptr;
    gLose = nullptr;
    gJump = nullptr;

    SDL_DestroyTexture(spikeTexture);
    SDL_DestroyTexture(spike2SheetTexture);

    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;

    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}

