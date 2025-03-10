#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "Game_Base.h"
#include "Game_Utils.h"
#include "LTexture.h"
#include "Elements.h"

using namespace std;

// Constants
const SDL_Color COLOR_RED = {255, 0, 0, 255};
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const int PLAYER_SIZE = 57;
const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100;
const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE;
const float OBSTACLE_SPEED = 5.0f;
const float JUMP_FORCE = -14.0f;
const float GRAVITY = 0.5f;
const Uint32 OBSTACLE_SPAWN_INTERVAL = 1500;
const int PLATFORM_HEIGHT = 10;
const float SPIKE_PROBABILITY = 0.9f;
const int MAX_SPIKES_PER_PLATFORM = 7;
const int MAX_PLATFORM_GAP = 10;
const int SPIKE2_FRAME_COUNT = 5;
const int ANIMATION_SPEED = 10;
const int BULLET_SIZE = 50;
const float BULLET_SPEED = 5.0f;
const int BULLET_RANGE = 200;
const float POWERUP_PROBABILITY = 0.9f;
const int POWERUP_SIZE = 40;
const int MAX_BULLETS = 5;

// Global Variables
int REMAINING_HEARTS = 3;
int SPIKE_SIZE = 70;
int COLLISION_SPIKE_SIZE = 50;
int bulletCount = 0;
int currentGroundLevel = GROUND_LEVEL_LOW;
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
SDL_Texture* bulletTexture = nullptr;
SDL_Texture* powerUpTexture = nullptr;
SDL_Texture* robotTexture = nullptr;
SDL_Texture* gameStateTexture = nullptr; 

// Structures
struct GameObject {
    SDL_Rect rect;          
    SDL_Rect collisionRect;
    SDL_Color color;
    float x_velocity;
    float y_velocity;
    bool isLethal;
    int currentFrame;
    int type;
};

struct Bullet {
    SDL_Rect rect;          
    SDL_Rect collisionRect; 
    float x_velocity;
    float y_velocity;
    float startX;
};

// Utility Functions
void waitUntilKeyPressed() {
    SDL_Event e;
    while (true) {
        if (SDL_PollEvent(&e) != 0 && (e.type == SDL_KEYDOWN || e.type == SDL_QUIT))
            return;
        SDL_Delay(100);
    }
}

bool checkAABBCollision(const SDL_Rect& a, const SDL_Rect& b) {
    if (a.x + a.w < b.x || b.x + b.w < a.x || a.y + a.h < b.y || b.y + b.h < a.y) {
        return false;
    }
    return true;
}

// Game Logic Functions
void resetGame(GameObject& player, vector<GameObject>& obstacles, vector<GameObject>& spikes, vector<GameObject>& powerUps,
               bool& running, bool& isJumping, float& obstacleSpeed, int& currentGroundLevel) {
    obstacleSpeed = 5.0f;
    obstacles.clear();
    spikes.clear();
    powerUps.clear();
    running = true;
    isJumping = false;
    currentGroundLevel = GROUND_LEVEL_LOW;

    GameObject initialPlatform;
    initialPlatform.rect = {0, GROUND_LEVEL_LOW, 1500, 10};
    initialPlatform.collisionRect = {0, GROUND_LEVEL_LOW, 1500, 10};
    initialPlatform.color = COLOR_RED;
    initialPlatform.x_velocity = -obstacleSpeed;
    initialPlatform.y_velocity = 0;
    obstacles.push_back(initialPlatform);

    player.rect = {200, currentGroundLevel - 100, PLAYER_SIZE, PLAYER_SIZE};
    player.collisionRect = {200, currentGroundLevel - 100, PLAYER_SIZE, PLAYER_SIZE};
    player.x_velocity = 0;
    player.y_velocity = 0;
    REMAINING_HEARTS = 3;
    bulletCount = 0;
}



// Initialization and Cleanup
bool Init() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        LogError("Can not initialize SDL.", SDL_ERROR);
        success = false;
    } else {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "Warning: Linear texture filtering not enabled!";
        }
        gWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!gWindow) {
            LogError("Can not create window", SDL_ERROR);
            success = false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (!gRenderer) {
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
    if (!gMusic) {
        LogError("Failed to load background music", MIX_ERROR);
        success = false;
    }

    gMenuMusic = Mix_LoadMUS("sound/menu_audio.wav");
    if (!gMenuMusic) {
        LogError("Failed to load menu music", MIX_ERROR);
        success = false;
    }

    gClick = Mix_LoadWAV("sound/mouse_click.wav");
    if (!gClick) {
        LogError("Failed to load mouse click sound", MIX_ERROR);
        success = false;
    }

    gJump = Mix_LoadWAV("sound/jump_sound.wav");
    if (!gJump) {
        LogError("Failed to load jumping sound", MIX_ERROR);
        success = false;
    }

    gLose = Mix_LoadWAV("sound/lose_sound.wav");
    if (!gLose) {
        LogError("Failed to load lose sound", MIX_ERROR);
        success = false;
    }

    gFont = TTF_OpenFont("font/pixel_font.ttf", 28);
    if (!gFont) {
        LogError("Failed to load font", MIX_ERROR);
        success = false;
    } else {
        if (!gText1Texture.LoadFromRenderedText("Your score: ", gFont, textColor, gRenderer)) {
            cout << "Failed to render text1 texture" << endl;
            success = false;
        }
        if (!gText2Texture.LoadFromRenderedText("High score: ", gFont, textColor, gRenderer)) {
            cout << "Failed to render text2 texture" << endl;
            success = false;
        }
        if (!gMenuTexture.LoadFromFile("images/bgg.png", gRenderer)) {
            cout << "Failed to load menu image" << endl;
            success = false;
        }
        if (!gInstructionTexture.LoadFromFile("imgs/background/instruction.png", gRenderer)) {
            cout << "Failed to load instruction image" << endl;
            success = false;
        }
        if (!gPlayButtonTexture.LoadFromFile("images/play.png", gRenderer)) {
            cout << "Failed to load play_button image" << endl;
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
            cout << "Failed to load help_button image" << endl;
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
            cout << "Failed to load back_button image" << endl;
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
            cout << "Failed to load quit image" << endl;
            success = false;
        } else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gExitButton[i].x = 175 * i;
                gExitButton[i].y = 0;
                gExitButton[i].w = 175;
                gExitButton[i].h = 90;
            }
        }
        if (!gAgainButtonTexture.LoadFromFile("images/again.png", gRenderer)) {
            cout << "Failed to load again image" << endl; 
            success = false;
        } else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gAgainButton[i].x = 175 * i;
                gAgainButton[i].y = 0;
                gAgainButton[i].w = 175;
                gAgainButton[i].h = 90;
            }
        }
        if (!gNameTexture.LoadFromFile("images/name.png", gRenderer)) {
            cout << "Failed to load name.png" << endl;
            success = false;
        }
        if (!gVineTexture.LoadFromFile("images/leo.png", gRenderer)) {
            cout << "Failed to load leo.png" << endl;
            success = false;
        }
        if (!gTransTexture.LoadFromFile("images/trans.png", gRenderer)) {
            cout << "Failed to load trans.png" << endl;
            success = false;
        }
        if (!gMenuButtonTexture.LoadFromFile("images/menu.png", gRenderer)) {
            cout << "Failed to load menu.png" << endl;
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
            cout << "Failed to load close.png" << endl;
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
            cout << "Failed to load setting.png" << endl;
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
            cout << "Failed to load sound_on.png" << endl;
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
            cout << "Failed to load labtube.png " << endl;
            success = false;
        } else {
            for (int i = 0; i < 3; ++i) {
                gLabtube[i].x = 800 * i;
                gLabtube[i].y = 0;
                gLabtube[i].w = 800;
                gLabtube[i].h = 500;
            }
        }
        if (!gHeartTexture.LoadFromFile("images/heart.png", gRenderer)) {
            cout << "Failed to load heart.png " << endl;
            success = false;
        } else {
            for (int i = 0; i < 4; ++i) {
                gHeart[i].x = 140 * i;
                gHeart[i].y = 0;
                gHeart[i].w = 140;
                gHeart[i].h = 60;
            }
        }
        if (!gPauseButtonTexture.LoadFromFile("imgs/button/big_button/pause_button.png", gRenderer)) {
            cout << "Failed to load pause_button image " << endl;
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
            cout << "Failed to load continue_button image " << endl;
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
                cout << "Failed to load background image" << endl;
                success = false;
            }
        }
        if (!gGroundTexture.LoadFromFile("imgs/background/ground.png", gRenderer)) {
            cout << "Failed to load ground image" << endl;
            success = false;
        }
        if (!gCharacterTexture.LoadFromFile("imgs/character/char.png", gRenderer)) {
            cout << "Failed to load character_run image." << endl;
            success = false;
        } else {
            for (int i = 0; i < 6; ++i) {
                gCharacterClips[i].x = 57 * i;
                gCharacterClips[i].y = 0;
                gCharacterClips[i].w = 57;
                gCharacterClips[i].h = 57;
            }
        }
        if (!gLoseTexture.LoadFromFile("imgs/background/lose.png", gRenderer)) {
            cout << "Failed to load lose image." << endl;
            success = false;
        }

        SDL_Surface* groundSurface = IMG_Load("ground.png");
        if (!groundSurface) {
            cerr << "Failed to load ground.png! SDL_image Error: " << IMG_GetError() << endl;
            success = false;
        } else {
            groundTexture = SDL_CreateTextureFromSurface(gRenderer, groundSurface);
            SDL_FreeSurface(groundSurface);
            if (!groundTexture) {
                cerr << "Failed to create texture from ground.png! SDL_Error: " << SDL_GetError() << endl;
                success = false;
            }
        }

        SDL_Surface* spikeSurface = IMG_Load("spike.png");
        if (!spikeSurface) {
            cerr << "Failed to load spike.png! IMG_Error: " << IMG_GetError() << endl;
            success = false;
        } else {
            spikeTexture = SDL_CreateTextureFromSurface(gRenderer, spikeSurface);
            SDL_FreeSurface(spikeSurface);
            if (!spikeTexture) {
                cerr << "Failed to create texture from spike.png! SDL_Error: " << SDL_GetError() << endl;
                success = false;
            }
        }

        SDL_Surface* spike2SheetSurface = IMG_Load("spike2_sheet.png");
        if (!spike2SheetSurface) {
            cerr << "Failed to load spike2_sheet.png! IMG_Error: " << IMG_GetError() << endl;
            success = false;
        } else {
            spike2SheetTexture = SDL_CreateTextureFromSurface(gRenderer, spike2SheetSurface);
            SDL_FreeSurface(spike2SheetSurface);
            if (!spike2SheetTexture) {
                cerr << "Failed to create texture from spike2_sheet.png! SDL_Error: " << SDL_GetError() << endl;
                success = false;
            }
        }

        SDL_Surface* robotSurface = IMG_Load("robot2.png");
        if (!robotSurface) {
            cerr << "Failed to load robot.png! IMG_Error: " << IMG_GetError() << endl;
            success = false;
        } else {
            robotTexture = SDL_CreateTextureFromSurface(gRenderer, robotSurface);
            SDL_FreeSurface(robotSurface);
            if (!robotTexture) {
                cerr << "Failed to create texture from robot.png! SDL_Error: " << SDL_GetError() << endl;
                success = false;
            }
        }

        SDL_Surface* bulletSurface = IMG_Load("images/bullet.png");
        if (!bulletSurface) {
            cerr << "Failed to load bullet.png! IMG_Error: " << IMG_GetError() << endl;
            success = false;
        } else {
            bulletTexture = SDL_CreateTextureFromSurface(gRenderer, bulletSurface);
            SDL_FreeSurface(bulletSurface);
            if (!bulletTexture) {
                cerr << "Failed to create texture from bullet.png! SDL_Error: " << SDL_GetError() << endl;
                success = false;
            }
        }

        SDL_Surface* powerUpSurface = IMG_Load("images/buff.png");
        if (!powerUpSurface) {
            cerr << "Failed to load buff.png! IMG_Error: " << IMG_GetError() << endl;
            success = false;
        } else {
            powerUpTexture = SDL_CreateTextureFromSurface(gRenderer, powerUpSurface);
            SDL_FreeSurface(powerUpSurface);
            if (!powerUpTexture) {
                cerr << "Failed to create texture from buff.png! SDL_Error: " << SDL_GetError() << endl;
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
    gNameTexture.Free();
    gVineTexture.Free();
    gTransTexture.Free();
    gMenuButtonTexture.Free();
    gCloseButtonTexture.Free();
    gSettingButtonTexture.Free();
    gSoundOnButtonTexture.Free();
    gLabtubeTexture.Free();
    gHeartTexture.Free();

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
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(powerUpTexture);
    SDL_DestroyTexture(groundTexture);
    SDL_DestroyTexture(robotTexture);
    SDL_DestroyTexture(gameStateTexture); 
    gameStateTexture = nullptr;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = nullptr;
    gWindow = nullptr;

    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}

// Main Function
int main(int argc, char* argv[]) {
    GameObject player;
    player.color = COLOR_RED;
    player.x_velocity = 0;
    player.y_velocity = 0;

    vector<GameObject> obstacles;
    vector<GameObject> spikes;
    vector<Bullet> bullets;
    vector<GameObject> powerUps;

    PlayButton.SetInteract(29, 23, 120, 40);
    HelpButton.SetInteract(29, 23, 120, 40);
    ExitButton.SetInteract(29, 23, 120, 40);
    CloseButton.SetInteract(15, 15, 30, 30);
    AgainButton.SetInteract(30, 25, 120, 40);
    SettingButton.SetInteract(15, 15, 30, 30);
    MenuButton.SetInteract(15, 15, 30, 30);

    int currentFrame = 0;
    int frameDelay = 200;
    Uint32 lastFrameTime = SDL_GetTicks();

    if (!Init()) {
        printf("Failed to initialize!\n");
        return 1;
    }
    if (!LoadMedia()) {
        printf("Failed to load media!\n");
        return 1;
    }

    bool Play_Again = false;
    bool Quit_Menu = false;
    Mix_PlayMusic(gMenuMusic, IS_REPEATITIVE);

    // Menu Loop
    while (!Quit_Menu) {
        SDL_Event e_mouse;
        while (SDL_PollEvent(&e_mouse) != 0) {
            if (e_mouse.type == SDL_QUIT) Quit_Menu = true;
            bool Quit_Game = false;
            HandlePlayButton(&e_mouse, PlayButton, Quit_Menu, Play_Again, gClick);
            HandleHelpButton(&e_mouse, gBackButton, HelpButton, BackButton, gInstructionTexture,
                             gBackButtonTexture, gRenderer, Quit_Game, gClick);
            HandleCloseButton(&e_mouse, CloseButton, Quit_Menu, gClick);
            HandleSettingButton(&e_mouse, SettingButton, gClick);
            HandleMenuButton(&e_mouse, MenuButton, gClick);
            if (Quit_Game) return 0;
        }

        gMenuTexture.Render(0, 0, gRenderer);
        gTransTexture.Render(0, 0, gRenderer);

        Uint32 currentTime = SDL_GetTicks();
        SDL_Rect renderSpace = {0, 0, 800, 500};
        if (currentTime - lastFrameTime >= frameDelay) {
            currentFrame = (currentFrame + 1) % 3;
            lastFrameTime = currentTime;
        }
        gLabtubeTexture.Render(0, 0, gRenderer, &gLabtube[currentFrame]);
        gVineTexture.Render(0, 0, gRenderer);
        gNameTexture.Render(0, 0, gRenderer);

        SDL_Rect* currentClip_Close = &gCloseButton[CloseButton.currentSprite];
        CloseButton.Render(currentClip_Close, gRenderer, gCloseButtonTexture);
        SDL_Rect* currentClip_Setting = &gSettingButton[SettingButton.currentSprite];
        SettingButton.Render(currentClip_Setting, gRenderer, gSettingButtonTexture);
        SDL_Rect* currentClip_Menu = &gMenuButton[MenuButton.currentSprite];
        MenuButton.Render(currentClip_Menu, gRenderer, gMenuButtonTexture);
        SDL_Rect* currentClip_Play = &gPlayButton[PlayButton.currentSprite];
        PlayButton.Render(currentClip_Play, gRenderer, gPlayButtonTexture);
        SDL_Rect* currentClip_Help = &gHelpButton[HelpButton.currentSprite];
        HelpButton.Render(currentClip_Help, gRenderer, gHelpButtonTexture);

        SDL_RenderPresent(gRenderer);
    }

    // Game Loop
    while (Play_Again) {
        srand(time(NULL));
        int time = 0;
        int score = 0;
        int acceleration = 0;
        int frame_Character = 0;
        string highscore = GetHighScoreFromFile("high_score.txt");

        obstacles.clear();
        resetGame(player, obstacles, spikes, powerUps, running, isJumping, obstacleSpeed, currentGroundLevel);

        float lastPlatformX = obstacles.empty() ? 0 : obstacles.back().rect.x + obstacles.back().rect.w;
        Mix_PlayMusic(gMusic, IS_REPEATITIVE);

        int OffsetSpeed_Ground = BASE_OFFSET_SPEED;
        vector<double> OffsetSpeed_Bkgr(BACKGROUND_LAYER, BASE_OFFSET_SPEED);
        bool Quit = false;
        bool Game_State = true;

        while (!Quit) {
            if (Game_State) {

                SDL_Event e;
                UpdateGameTimeAndScore(time, acceleration, score);
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        running = false;
                        Quit = true;
                        Play_Again = false;
                    }
                    HandlePauseButton(&e, gRenderer, gContinueButton, PauseButton, ContinueButton,
                                      gContinueButtonTexture, Game_State, gClick);
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_SPACE && !isJumping) {
                            isJumping = true;
                            Mix_PlayChannel(MIX_CHANNEL, gJump, NOT_REPEATITIVE);
                            player.y_velocity = JUMP_FORCE;
                        }
                        if (e.key.keysym.sym == SDLK_p && bulletCount > 0) {
                            Bullet bullet;
                            bullet.rect = {player.rect.x + player.rect.w, player.rect.y + (player.rect.h - BULLET_SIZE) / 2, BULLET_SIZE, BULLET_SIZE};
                            bullet.collisionRect = {player.rect.x + player.rect.w, player.rect.y + (player.rect.h - BULLET_SIZE) / 2, BULLET_SIZE, BULLET_SIZE};
                            bullet.x_velocity = BULLET_SPEED;
                            bullet.y_velocity = 0;
                            bullet.startX = bullet.rect.x;
                            bullets.push_back(bullet);
                            bulletCount--;
                            cout << "Bắn đạn! Còn lại: " << bulletCount << " viên." << endl;
                        }
                    }
                }

                int obstacleWidth = 3000 + (rand() % 500);
                currentGroundLevel = (currentGroundLevel == GROUND_LEVEL_LOW) ? GROUND_LEVEL_HIGH : GROUND_LEVEL_LOW;
                int newPlatformX = lastPlatformX + (rand() % MAX_PLATFORM_GAP) + 200;

                GameObject obstacle;
                obstacle.rect = {newPlatformX, currentGroundLevel, obstacleWidth, 100};
                obstacle.collisionRect = {newPlatformX, currentGroundLevel, obstacleWidth, 100};
                obstacle.color = COLOR_RED;
                obstacle.x_velocity = -obstacleSpeed;
                obstacle.y_velocity = 0;
                obstacles.push_back(obstacle);
                lastPlatformX = obstacle.rect.x + obstacle.rect.w;

                if ((float)rand() / RAND_MAX < SPIKE_PROBABILITY) {
                    int numSpikes = 1 + (rand() % MAX_SPIKES_PER_PLATFORM);
                    int spikeSpacing = obstacleWidth / (numSpikes + 1);
                    for (int i = 1; i <= numSpikes; ++i) {
                        GameObject spike;
                        spike.rect = {newPlatformX + i * spikeSpacing - SPIKE_SIZE / 2, currentGroundLevel - SPIKE_SIZE, SPIKE_SIZE, SPIKE_SIZE};
                        spike.collisionRect = {newPlatformX + i * spikeSpacing - SPIKE_SIZE / 2, currentGroundLevel - SPIKE_SIZE, COLLISION_SPIKE_SIZE, COLLISION_SPIKE_SIZE};
                        spike.x_velocity = -obstacleSpeed;
                        spike.y_velocity = 0;
                        spike.isLethal = true;
                        spike.currentFrame = 0;
                        spike.type = (rand() % 2);
                        spikes.push_back(spike);
                    }
                }

                if ((float)rand() / RAND_MAX < POWERUP_PROBABILITY) {
                    GameObject powerUp;
                    powerUp.rect = {newPlatformX + (rand() % obstacleWidth) - POWERUP_SIZE / 2, currentGroundLevel - POWERUP_SIZE, POWERUP_SIZE, POWERUP_SIZE};
                    powerUp.collisionRect = {powerUp.rect.x, powerUp.rect.y, POWERUP_SIZE, POWERUP_SIZE};
                    powerUp.x_velocity = -obstacleSpeed;
                    powerUp.y_velocity = 0;
                    powerUp.isLethal = false;
                    powerUp.currentFrame = 0;
                    powerUp.type = 0;
                    powerUps.push_back(powerUp);
                }

                for (auto it = obstacles.begin(); it != obstacles.end();) {
                    it->rect.x += it->x_velocity;
                    it->collisionRect.x += it->x_velocity;
                    if (it->rect.x + it->rect.w < 0) it = obstacles.erase(it);
                    else ++it;
                }

                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - animationTimer > ANIMATION_SPEED) {
                    for (auto& spike : spikes) {
                        if (spike.type == 1) spike.currentFrame = (spike.currentFrame + 1) % SPIKE2_FRAME_COUNT;
                    }
                    animationTimer = currentTime;
                }
                for (auto it = spikes.begin(); it != spikes.end();) {
                    it->rect.x += it->x_velocity;
                    it->collisionRect.x += it->x_velocity;
                    if (it->rect.x + it->rect.w < 0) it = spikes.erase(it);
                    else ++it;
                }

                for (auto it = bullets.begin(); it != bullets.end();) {
                    it->rect.x += it->x_velocity;
                    it->collisionRect.x += it->x_velocity;
                    bool hitSpike2 = false;
                    for (auto spikeIt = spikes.begin(); spikeIt != spikes.end();) {
                        if (spikeIt->type == 1 && checkAABBCollision(it->collisionRect, spikeIt->collisionRect)) {
                            spikeIt = spikes.erase(spikeIt);
                            hitSpike2 = true;
                            break;
                        } else ++spikeIt;
                    }
                    float distanceTraveled = it->rect.x - it->startX;
                    if (hitSpike2 || distanceTraveled > BULLET_RANGE) it = bullets.erase(it);
                    else ++it;
                }

                isOnGround = false;
                collided = true;
                for (const auto& obstacle : obstacles) {
                    if (checkAABBCollision(player.collisionRect, obstacle.collisionRect)) {
                        collided = false;
                        if (player.y_velocity >= 0) {
                            player.rect.y = obstacle.collisionRect.y - player.rect.h;
                            player.collisionRect.y = obstacle.collisionRect.y - player.collisionRect.h;
                            player.y_velocity = 0;
                            isJumping = false;
                            isOnGround = true;
                        }
                    }
                }

                for (auto it = spikes.begin(); it != spikes.end();) {
                    if (checkAABBCollision(player.collisionRect, it->collisionRect)) {
                        cout << "Chạm gai! Game Over!" << endl;
                        REMAINING_HEARTS--;
                        Mix_PlayChannel(MIX_CHANNEL, gLose, NOT_REPEATITIVE);
                        it = spikes.erase(it);
                        if (REMAINING_HEARTS <= 0) {
                            running = false;
                            break;
                        }
                        SDL_Delay(500);
                    } else ++it;
                }

                for (auto it = powerUps.begin(); it != powerUps.end();) {
                    it->rect.x += it->x_velocity;
                    it->collisionRect.x += it->x_velocity;
                    if (it->collisionRect.x + it->collisionRect.w < 0) {
                        it = powerUps.erase(it);
                    } else {
                        if (checkAABBCollision(player.rect, it->collisionRect)) {
                            bulletCount += MAX_BULLETS;
                            cout << "Nhặt power-up! Bạn có " << bulletCount << " viên đạn." << endl;
                            it = powerUps.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }

                if (!isOnGround && player.rect.y > SCREEN_HEIGHT) {
                    cout << "Rơi khỏi nền! Game Over!" << endl;
                    REMAINING_HEARTS = 0;
                    Mix_PauseMusic();
                    Mix_PlayChannel(MIX_CHANNEL, gLose, NOT_REPEATITIVE);
                    UpdateHighScore("high_score.txt", score, highscore);
                    Quit = true;

                    // 
                    if (gameStateTexture) SDL_DestroyTexture(gameStateTexture);
                    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                                                   0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
                    SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, tempSurface->pixels, tempSurface->pitch);
                    gameStateTexture = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
                    SDL_FreeSurface(tempSurface);
                }

                if (!REMAINING_HEARTS) Quit = true;
                ControlCharFrame(frame_Character);

                player.y_velocity += GRAVITY;
                player.rect.y += player.y_velocity;
                player.collisionRect.y += player.y_velocity;

                //  render
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);
                RenderScrollingBackground(OffsetSpeed_Bkgr, gBackgroundTexture, gRenderer);
                RenderScrollingGround(OffsetSpeed_Ground, acceleration, gGroundTexture, gRenderer);

                for (auto& obstacle : obstacles) {
                    SDL_RenderCopy(gRenderer, groundTexture, NULL, &obstacle.rect);
                }

                SDL_Rect srcRect = {0, 0, 15, 15};
                for (const auto& spike : spikes) {
                    SDL_Rect destRect = spike.rect;
                    destRect.w = SPIKE_SIZE;
                    destRect.h = SPIKE_SIZE;
                    destRect.x = spike.rect.x + (spike.rect.w - SPIKE_SIZE) / 2;
                    destRect.y = spike.rect.y + (spike.rect.h - SPIKE_SIZE) / 2;
                    if (spike.type == 0) {
                        destRect.y += 25;
                        SDL_RenderCopy(gRenderer, spikeTexture, nullptr, &destRect);
                    } else {
                        SDL_RenderCopy(gRenderer, robotTexture, nullptr, &destRect);
                    }
                }

                for (const auto& bullet : bullets) {
                    SDL_RenderCopy(gRenderer, bulletTexture, nullptr, &bullet.rect);
                }

                for (const auto& powerUp : powerUps) {
                    SDL_RenderCopy(gRenderer, powerUpTexture, nullptr, &powerUp.rect);
                }

                SDL_Rect* currentClip_Pause = &gPauseButton[PauseButton.currentSprite];
                PauseButton.Render(currentClip_Pause, gRenderer, gPauseButtonTexture);
                DrawPlayerScore(gText1Texture, gScoreTexture, textColor, gRenderer, gFont, score);
                DrawPlayerHighScore(gText2Texture, gHighScoreTexture, textColor, gRenderer, gFont, highscore);
                gHeartTexture.Render(200, 0, gRenderer, &gHeart[REMAINING_HEARTS]);

                SDL_Rect* currentClip_Character = &gCharacterClips[frame_Character / SLOW_FRAME_CHAR];
                SDL_RenderCopy(gRenderer, gCharacterTexture.mTexture, currentClip_Character, &player.rect);

                SDL_RenderPresent(gRenderer);
            }
        }

        // End Game Loop
        if (Play_Again) {
            REMAINING_HEARTS = 3;
            bool End_Game = false;
            SDL_Event e;


            SDL_SetTextureAlphaMod(gLoseTexture.mTexture, 200);

            while (!End_Game) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        End_Game = true;
                        Play_Again = false;
                    }
                    if (HandleAgainButton(&e, AgainButton, gClick)) { 
                        obstacles.clear();
                        resetGame(player, obstacles, spikes, powerUps, running, isJumping, obstacleSpeed, currentGroundLevel);
                        End_Game = true; 
                    }
                }

                SDL_RenderCopy(gRenderer, gameStateTexture, NULL, NULL); 
                gLoseTexture.Render(0, 0, gRenderer); 
                SDL_Rect* currentClip_Again = &gAgainButton[AgainButton.currentSprite];
                AgainButton.Render(currentClip_Again, gRenderer, gAgainButtonTexture); 
                SDL_RenderPresent(gRenderer);
            }

            SDL_SetTextureAlphaMod(gLoseTexture.mTexture, 255);
        }
    }

    Close();
    return 0;
}
