//#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
//#include <SDL2/SDL_main.h>
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
const int PLAYER_SIZE = 100;
const int PLAYER_COLLISION_SIZE = 50;
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
const int POWERUP_SIZE = 120;
const int MAX_BULLETS = 5;
const int RENDER_OFFSET_Y = -23;
const int ROBOT_SIZE = 70;
const int POISON_SIZE = 70;
const int ROBOT_COLLISION_SIZE = 50;
const int POSION_COLLISION_SIZE = 45;
const Uint32 SCORE_INTERVAL = 500;


const int SLIDER_START_X = 60;
const int SLIDER_END_X = 60 + 170;
const int SLIDER_Y = 465;
const int KNOB_SIZE = 40;


// Global Variables
int skinNumber = 1;
int counttloop = 0;
int REMAINING_HEARTS = 3;
int bulletCount = 0;
int currentGroundLevel = GROUND_LEVEL_LOW;
bool isOnGround = false;
bool collided = true;
bool running = true;
bool isJumping = false;
float obstacleSpeed = 5.0f;
Uint32 obstacleTimer = SDL_GetTicks();
Uint32 animationTimer = SDL_GetTicks();

SDL_Texture* groundTexture = nullptr;
SDL_Texture* spikeTexture = nullptr;
SDL_Texture* spike2SheetTexture = nullptr;
SDL_Texture* bulletTexture = nullptr;
SDL_Texture* powerUpTexture = nullptr;
SDL_Texture* robotTexture = nullptr;
SDL_Texture* gameStateTexture = nullptr;
SDL_Texture* glowTexture = nullptr;
SDL_Texture* loseTexture = nullptr;
SDL_Texture* sliderTexture = nullptr;
SDL_Texture* knobTexture = nullptr;

// Structures
struct GameObject {
    SDL_Rect rect;          // Dùng để render
    SDL_Rect collisionRect; // Dùng để check va chạm
    SDL_Color color;
    float x_velocity;
    float y_velocity;
    bool isLethal;
    int currentFrame;
    int type;
};

struct Bullet {
    SDL_Rect rect;          // Dùng để render
    SDL_Rect collisionRect; // Dùng để check va chạm
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

    player.rect = {200 + RENDER_OFFSET_Y, currentGroundLevel - 100 + RENDER_OFFSET_Y, PLAYER_SIZE, PLAYER_SIZE};
    player.collisionRect = {200, currentGroundLevel - 100, PLAYER_COLLISION_SIZE, PLAYER_COLLISION_SIZE};
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

    gMusic = Mix_LoadMUS("sound/loop.mp3");
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
        if (!gText1Texture.LoadFromRenderedText("Your score: ", gFont, textColor, gRenderer)) success = false;
        if (!gText2Texture.LoadFromRenderedText("High score: ", gFont, textColor, gRenderer)) success = false;
        if (!gMenuTexture.LoadFromFile("images/bgg.png", gRenderer)) success = false;
        if (!gInstructionTexture.LoadFromFile("imgs/background/instruction.png", gRenderer)) success = false;
        if (!gPlayButtonTexture.LoadFromFile("images/play.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gPlayButton[i].x = 175 * i;
                gPlayButton[i].y = 0;
                gPlayButton[i].w = 175;
                gPlayButton[i].h = 90;
            }
        }
        if (!gHelpButtonTexture.LoadFromFile("images/help.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gHelpButton[i].x = 175 * i;
                gHelpButton[i].y = 0;
                gHelpButton[i].w = 175;
                gHelpButton[i].h = 90;
            }
        }
        if (!gBackButtonTexture.LoadFromFile("imgs/button/big_button/back_button.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gBackButton[i].x = 100 * i;
                gBackButton[i].y = 0;
                gBackButton[i].w = 100;
                gBackButton[i].h = 78;
            }
        }
        if (!gExitButtonTexture.LoadFromFile("images/quit.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gExitButton[i].x = 175 * i;
                gExitButton[i].y = 0;
                gExitButton[i].w = 175;
                gExitButton[i].h = 90;
            }
        }
        if (!gAgainButtonTexture.LoadFromFile("images/again.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gAgainButton[i].x = 175 * i;
                gAgainButton[i].y = 0;
                gAgainButton[i].w = 175;
                gAgainButton[i].h = 90;
            }
        }
        if (!gNameTexture.LoadFromFile("images/name.png", gRenderer)) success = false;
        if (!gVineTexture.LoadFromFile("images/leo.png", gRenderer)) success = false;
        if (!gTransTexture.LoadFromFile("images/trans.png", gRenderer)) success = false;
        if (!gMenuButtonTexture.LoadFromFile("images/menu.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gMenuButton[i].x = 60 * i;
                gMenuButton[i].y = 0;
                gMenuButton[i].w = 60;
                gMenuButton[i].h = 60;
            }
        }
        if (!gCloseButtonTexture.LoadFromFile("images/close.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gCloseButton[i].x = 60 * i;
                gCloseButton[i].y = 0;
                gCloseButton[i].w = 60;
                gCloseButton[i].h = 60;
            }
        }
        if (!gSettingButtonTexture.LoadFromFile("images/setting.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gSettingButton[i].x = 60 * i;
                gSettingButton[i].y = 0;
                gSettingButton[i].w = 60;
                gSettingButton[i].h = 60;
            }
        }
        if (!gSoundOnButtonTexture.LoadFromFile("images/sound_on.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gSoundOnButton[i].x = 60 * i;
                gSoundOnButton[i].y = 0;
                gSoundOnButton[i].w = 60;
                gSoundOnButton[i].h = 60;
            }
        }
        if (!gLabtubeTexture.LoadFromFile("images/labtube.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < 3; ++i) {
                gLabtube[i].x = 800 * i;
                gLabtube[i].y = 0;
                gLabtube[i].w = 800;
                gLabtube[i].h = 500;
            }
        }
        if (!gHeartTexture.LoadFromFile("images/heart.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < 4; ++i) {
                gHeart[i].x = 140 * i;
                gHeart[i].y = 0;
                gHeart[i].w = 140;
                gHeart[i].h = 60;
            }
        }
        if (!gPauseButtonTexture.LoadFromFile("images/pause.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gPauseButton[i].x = 60 * i;
                gPauseButton[i].y = 0;
                gPauseButton[i].w = 60;
                gPauseButton[i].h = 60;
            }
        }
        if (!gContinueButtonTexture.LoadFromFile("images/continue.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gContinueButton[i].x = 60 * i;
                gContinueButton[i].y = 0;
                gContinueButton[i].w = 60;
                gContinueButton[i].h = 60;
            }
        }
        for (int i = 0; i < BACKGROUND_LAYER; ++i) {
            if (!gBackgroundTexture[i].LoadFromFile(LAYER[i].c_str(), gRenderer)) success = false;
        }
        if (!gGroundTexture.LoadFromFile("imgs/background/ground.png", gRenderer)) success = false;

        if (!gCharacterTexture[0].LoadFromFile("images/default_dino.png", gRenderer)
                ||  !gCharacterTexture[1].LoadFromFile("images/arcane_dino.png", gRenderer)
                ||  !gCharacterTexture[2].LoadFromFile("images/blue_dino.png", gRenderer)
                ||  !gCharacterTexture[3].LoadFromFile("images/purple_dino.png", gRenderer)
                ||  !gCharacterTexture[4].LoadFromFile("images/grey_dino.png", gRenderer))
            success = false;
        else {
            for (int i = 0; i < 4; ++i) {
                gCharacterClips[i].x = 258 * i;
                gCharacterClips[i].y = 0;
                gCharacterClips[i].w = 258;
                gCharacterClips[i].h = 281;
            }
        }

        SDL_Surface* groundSurface = IMG_Load("ground.png");
        if (!groundSurface) success = false;
        else {
            groundTexture = SDL_CreateTextureFromSurface(gRenderer, groundSurface);
            SDL_FreeSurface(groundSurface);
            if (!groundTexture) success = false;
        }

        SDL_Surface* spikeSurface = IMG_Load("spike.png");
        if (!spikeSurface) success = false;
        else {
            spikeTexture = SDL_CreateTextureFromSurface(gRenderer, spikeSurface);
            SDL_FreeSurface(spikeSurface);
            if (!spikeTexture) success = false;
        }

        SDL_Surface* spike2SheetSurface = IMG_Load("spike2_sheet.png");
        if (!spike2SheetSurface) success = false;
        else {
            spike2SheetTexture = SDL_CreateTextureFromSurface(gRenderer, spike2SheetSurface);
            SDL_FreeSurface(spike2SheetSurface);
            if (!spike2SheetTexture) success = false;
        }

        SDL_Surface* robotSurface = IMG_Load("robot2.png");
        if (!robotSurface) success = false;
        else {
            robotTexture = SDL_CreateTextureFromSurface(gRenderer, robotSurface);
            SDL_FreeSurface(robotSurface);
            if (!robotTexture) success = false;
        }

        SDL_Surface* bulletSurface = IMG_Load("images/bullet.png");
        if (!bulletSurface) success = false;
        else {
            bulletTexture = SDL_CreateTextureFromSurface(gRenderer, bulletSurface);
            SDL_FreeSurface(bulletSurface);
            if (!bulletTexture) success = false;
        }

        SDL_Surface* powerUpSurface = IMG_Load("images/buff.png");
        if (!powerUpSurface) success = false;
        else {
            powerUpTexture = SDL_CreateTextureFromSurface(gRenderer, powerUpSurface);
            SDL_FreeSurface(powerUpSurface);
            if (!powerUpTexture) success = false;
        }

        SDL_Surface* glowSurface = IMG_Load("images/glow.png");
        if (!glowSurface) success = false;
        else {
            glowTexture = SDL_CreateTextureFromSurface(gRenderer, glowSurface);
            SDL_FreeSurface(glowSurface);
            if (!glowTexture) success = false;
        }

        if (!gGlowTexture.LoadFromFile("images/glow.png", gRenderer)) success = false;

        SDL_Surface* loseSurface = IMG_Load("images/GAME OVER.png");
        if (!loseSurface) success = false;
        else {
            loseTexture = SDL_CreateTextureFromSurface(gRenderer, loseSurface);
            SDL_FreeSurface(loseSurface);
            if (!loseTexture) success = false;
        }

        if (!gLoseTexture.LoadFromFile("images/GAME OVER.png", gRenderer)) success = false;
    }
    SDL_Surface* sliderSurface = IMG_Load("images/slider.png");
    if (!sliderSurface) {
        LogError("Failed to load slider image", IMG_ERROR);
        success = false;
    } else {
        sliderTexture = SDL_CreateTextureFromSurface(gRenderer, sliderSurface);
        SDL_FreeSurface(sliderSurface);
        if (!sliderTexture) {
            LogError("Failed to create slider texture", SDL_ERROR);
            success = false;
        }
    }

    SDL_Surface* knobSurface = IMG_Load("images/knob.png");
    if (!knobSurface) {
        LogError("Failed to load knob image", IMG_ERROR);
        success = false;
    } else {
        knobTexture = SDL_CreateTextureFromSurface(gRenderer, knobSurface);
        SDL_FreeSurface(knobSurface);
        if (!knobTexture) {
            LogError("Failed to create knob texture", SDL_ERROR);
            success = false;
        }
    }

    if (!gSoundOnButtonTexture.LoadFromFile("images/sound_on.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gSoundOnButton[i].x = 60 * i;
                gSoundOnButton[i].y = 0;
                gSoundOnButton[i].w = 60;
                gSoundOnButton[i].h = 60;
            }
        }

    if (!gSoundOffButtonTexture.LoadFromFile("images/sound_off.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gSoundOffButton[i].x = 60 * i;
                gSoundOffButton[i].y = 0;
                gSoundOffButton[i].w = 60;
                gSoundOffButton[i].h = 60;
            }
        }

    return success;
}

void Close() {
    gMenuTexture.Free();
    gInstructionTexture.Free();
    for(int i = 0 ; i < 5 ; i++)
        gCharacterTexture[i].Free();
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

    vector<double> layer_speed;
    layer_speed.push_back(LAYER_1_SPEED);
    layer_speed.push_back(LAYER_2_SPEED);
    layer_speed.push_back(LAYER_3_SPEED);
    layer_speed.push_back(LAYER_4_SPEED);
    layer_speed.push_back(LAYER_5_SPEED);
    layer_speed.push_back(LAYER_6_SPEED);
    layer_speed.push_back(LAYER_7_SPEED);
    layer_speed.push_back(LAYER_8_SPEED);
    layer_speed.push_back(LAYER_9_SPEED);

    PlayButton.SetInteract(29, 23, 120, 40);
    HelpButton.SetInteract(29, 23, 120, 40);
    ExitButton.SetInteract(29, 23, 120, 40);
    CloseButton.SetInteract(15, 15, 30, 30);
    AgainButton.SetInteract(30, 25, 120, 40);
    SettingButton.SetInteract(15, 15, 30, 30);
    MenuButton.SetInteract(15, 15, 30, 30);
    PauseButton.SetInteract(22, 18, 15, 25);
    ContinueButton.SetInteract(20, 19, 23, 22);
    SoundOnButton.SetInteract(15, 18, 29, 24);
    SoundOffButton.SetInteract(15, 18, 29, 24);

    int currentFrame = 0;
    int frameDelay = 200;
    int frameDelay_character = 75;
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
    /*
    int channel = Mix_PlayChannel(-1, gJump, -1);
    if (channel == -1) {
        std::cerr << "Failed to play sound: " << Mix_GetError() << std::endl;
        return 1;
    }
    */
    int currentX = (SLIDER_START_X + SLIDER_END_X) / 2;
    bool isDragging = false;
    bool quit = false;

    // Menu Loop
    while (!Quit_Menu) {

        int volume = ((currentX - SLIDER_START_X) * MIX_MAX_VOLUME) / (SLIDER_END_X - SLIDER_START_X);
        Mix_VolumeMusic(volume);
        Mix_Volume(MIX_CHANNEL, volume);

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
            HandleSoundButton(&e_mouse, currentX, SoundOnButton, SoundOffButton, gClick);
        cout << currentX << endl;
            if (Quit_Game) return 0;

            else if (e_mouse.type == SDL_MOUSEBUTTONDOWN) {
                if (e_mouse.button.button == SDL_BUTTON_LEFT) {
                    SDL_Point mousePos = { e_mouse.button.x, e_mouse.button.y };
                    if (mousePos.y >= SLIDER_Y - 10 &&
                            mousePos.y <= SLIDER_Y + 10 &&
                            mousePos.x >= SLIDER_START_X &&
                            mousePos.x <= SLIDER_END_X) {
                        currentX = mousePos.x;
                        currentX = SDL_clamp(currentX, SLIDER_START_X, SLIDER_END_X);
                        isDragging = true;
                    }
                }
            } else if (e_mouse.type == SDL_MOUSEBUTTONUP) {
                if (e_mouse.button.button == SDL_BUTTON_LEFT) {
                    isDragging = false;
                }
            } else if (e_mouse.type == SDL_MOUSEMOTION) {
                if (isDragging) {
                    currentX = SDL_clamp(e_mouse.motion.x, SLIDER_START_X, SLIDER_END_X);
                }
            }
        }
        //Chỉnh âm

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
        if(volume){
            SDL_Rect* currentClip_SoundOn = &gSoundOnButton[SoundOnButton.currentSprite];
            SoundOnButton.Render(currentClip_SoundOn, gRenderer, gSoundOnButtonTexture);
        }else{
            SDL_Rect* currentClip_SoundOff = &gSoundOffButton[SoundOffButton.currentSprite];
            SoundOffButton.Render(currentClip_SoundOff, gRenderer, gSoundOffButtonTexture);
        }
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

        SDL_Rect sliderRect = {
            SLIDER_START_X,
            SLIDER_Y - 5,
            SLIDER_END_X - SLIDER_START_X,
            13
        };
        SDL_RenderCopy(gRenderer, sliderTexture, NULL, &sliderRect);

        // knob
        SDL_Rect knobRect = {
            currentX - KNOB_SIZE / 2,
            SLIDER_Y - KNOB_SIZE / 2 + 2,
            KNOB_SIZE,
            KNOB_SIZE
        };
        SDL_RenderCopy(gRenderer, knobTexture, NULL, &knobRect);

        SDL_RenderPresent(gRenderer);
    }
    // Game Loop
    while (Play_Again) {
        srand(time(NULL));
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
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    running = false;
                    Quit = true;
                    Play_Again = false;
                }
                HandlePauseButton(&e, gRenderer, gContinueButton, PauseButton, ContinueButton,
                                  gContinueButtonTexture, Game_State, gClick);
                if (Game_State) {
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
            }

            if (Game_State) {
                int obstacleWidth = 3000 + (rand() % 500);
                currentGroundLevel = (currentGroundLevel == GROUND_LEVEL_LOW) ? GROUND_LEVEL_HIGH : GROUND_LEVEL_LOW;
                int newPlatformX = lastPlatformX + (rand() % MAX_PLATFORM_GAP) + 200;

                GameObject obstacle;
                obstacle.rect = {newPlatformX, currentGroundLevel, obstacleWidth, 10};
                obstacle.collisionRect = {newPlatformX, currentGroundLevel, obstacleWidth, 5};
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
                        spike.type = (rand() % 2);
                        if (spike.type == 1) { // robot
                            int randomRobotHeight = rand() % 100;
                            spike.rect = {newPlatformX + i * spikeSpacing - ROBOT_SIZE / 2 - 10, currentGroundLevel - ROBOT_SIZE - randomRobotHeight, ROBOT_SIZE, ROBOT_SIZE};
                            spike.collisionRect = {newPlatformX + i * spikeSpacing - ROBOT_SIZE / 2, currentGroundLevel - ROBOT_SIZE + 20 - randomRobotHeight, ROBOT_COLLISION_SIZE, ROBOT_COLLISION_SIZE};
                        } else { // poison
                            spike.rect = {newPlatformX + i * spikeSpacing - POISON_SIZE / 2, currentGroundLevel - POISON_SIZE + 25, POISON_SIZE, POISON_SIZE};
                            spike.collisionRect = {newPlatformX + i * spikeSpacing - POISON_SIZE / 2 + 15, currentGroundLevel - POISON_SIZE + 35, POSION_COLLISION_SIZE, POSION_COLLISION_SIZE};
                        }
                        spike.x_velocity = -obstacleSpeed;
                        spike.y_velocity = 0;
                        spike.isLethal = true;
                        spike.currentFrame = 0;
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
                            player.collisionRect.y = obstacle.collisionRect.y - player.collisionRect.h;
                            player.rect.y = player.collisionRect.y + RENDER_OFFSET_Y;
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
                        if (checkAABBCollision(player.collisionRect, it->collisionRect)) {
                            bulletCount += MAX_BULLETS;
                            cout << "Nhặt power-up! Bạn có " << bulletCount << " viên đạn." << endl;
                            it = powerUps.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }

                if (!isOnGround && player.collisionRect.y > SCREEN_HEIGHT) {
                    cout << "Rơi khỏi nền! Game Over!" << endl;
                    REMAINING_HEARTS = 0;
                    Mix_PauseMusic();
                    Mix_PlayChannel(MIX_CHANNEL, gLose, NOT_REPEATITIVE);
                    Quit = true;

                    gHeartTexture.Render(REMAINING_HEARTS_POSX, REMAINING_HEARTS_POSY, gRenderer, &gHeart[REMAINING_HEARTS]);
                    SDL_RenderPresent(gRenderer);

                    if (gameStateTexture) SDL_DestroyTexture(gameStateTexture);
                    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                               0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
                    SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, tempSurface->pixels, tempSurface->pitch);
                    gameStateTexture = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
                    SDL_FreeSurface(tempSurface);
                }

                if (!REMAINING_HEARTS) {
                    gHeartTexture.Render(REMAINING_HEARTS_POSX, REMAINING_HEARTS_POSY, gRenderer, &gHeart[REMAINING_HEARTS]);
                    SDL_RenderPresent(gRenderer);
                    if (gameStateTexture) SDL_DestroyTexture(gameStateTexture);
                    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                               0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
                    SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, tempSurface->pixels, tempSurface->pitch);
                    gameStateTexture = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
                    SDL_FreeSurface(tempSurface);
                    Quit = true;
                }
                ControlCharFrame(frame_Character);

                player.y_velocity += GRAVITY;
                player.collisionRect.y += player.y_velocity;
                player.rect.y = player.collisionRect.y + RENDER_OFFSET_Y;
            }

            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);

            if (Game_State) {
                for (int i = 0; i < BACKGROUND_LAYER; ++i) {
                    OffsetSpeed_Bkgr[i] -= layer_speed[i];
                    if (OffsetSpeed_Bkgr[i] < -gBackgroundTexture[i].GetWidth()) {
                        OffsetSpeed_Bkgr[i] = 0;
                    }
                    gBackgroundTexture[i].Render(OffsetSpeed_Bkgr[i], 0, gRenderer);
                    gBackgroundTexture[i].Render(OffsetSpeed_Bkgr[i] + gBackgroundTexture[i].GetWidth(), 0, gRenderer);
                }
            }

            for (int i = 0; i < BACKGROUND_LAYER; ++i) {
                gBackgroundTexture[i].Render(OffsetSpeed_Bkgr[i], 0, gRenderer);
                gBackgroundTexture[i].Render(OffsetSpeed_Bkgr[i] + gBackgroundTexture[i].GetWidth(), 0, gRenderer);
            }

            for (auto& obstacle : obstacles) {
                SDL_RenderCopy(gRenderer, groundTexture, NULL, &obstacle.rect);
            }

            SDL_Rect srcRect = {0, 0, 15, 15};
            for (const auto& spike : spikes) {
                if (spike.type == 0) {
                    SDL_RenderCopy(gRenderer, spikeTexture, nullptr, &spike.rect);
                } else {
                    SDL_RenderCopy(gRenderer, robotTexture, nullptr, &spike.rect);
                }
            }

            for (const auto& bullet : bullets) {
                SDL_RenderCopy(gRenderer, bulletTexture, nullptr, &bullet.rect);
            }

            for (const auto& powerUp : powerUps) {
                SDL_RenderCopy(gRenderer, powerUpTexture, nullptr, &powerUp.rect);
            }

            if (Game_State) {
                SDL_Rect* currentClip_Pause = &gPauseButton[PauseButton.currentSprite];
                PauseButton.Render(currentClip_Pause, gRenderer, gPauseButtonTexture);
            } else {
                SDL_Rect* currentClip_Continue = &gContinueButton[ContinueButton.currentSprite];
                ContinueButton.Render(currentClip_Continue, gRenderer, gContinueButtonTexture);
            }

            gHeartTexture.Render(REMAINING_HEARTS_POSX, REMAINING_HEARTS_POSY, gRenderer, &gHeart[REMAINING_HEARTS]);

            Uint32 currentTime = SDL_GetTicks();
            if (Game_State && currentTime - lastFrameTime >= frameDelay_character) {
                currentFrame = (currentFrame + 1) % 4;
                lastFrameTime = currentTime;
            }

            SDL_Rect* currentClip_Character = &gCharacterClips[currentFrame];
            SDL_RenderCopy(gRenderer, gCharacterTexture[skinNumber].mTexture, currentClip_Character, &player.rect);

            SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
            SDL_RenderPresent(gRenderer);
        }

        if (Play_Again) {
            REMAINING_HEARTS = 3;
            bool End_Game = false;
            SDL_Event e;

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
                    if (HandleQuitButton(&e, ExitButton, gClick)) {
                        Close();
                        return 0;
                    }
                }

                SDL_RenderCopy(gRenderer, gameStateTexture, NULL, NULL);
                gLoseTexture.Render(0, 0, gRenderer);
                SDL_Rect* currentClip_Again = &gAgainButton[AgainButton.currentSprite];
                AgainButton.Render(currentClip_Again, gRenderer, gAgainButtonTexture);
                SDL_Rect* currentClip_Exit = &gExitButton[ExitButton.currentSprite];
                ExitButton.Render(currentClip_Exit, gRenderer, gExitButtonTexture);
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    Close();
    return 0;
}
