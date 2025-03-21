#ifndef ELEMENTS_H_
#define ELEMENTS_H_

#include "Button.h"
#include "Game_Base.h"
#include "Game_Utils.h"
#include "LTexture.h"

const std::string LAYER[BACKGROUND_LAYER] = {
	"images/elements/background/1.png",
	"images/elements/background/2.png",
	"images/elements/background/3.png",
	"images/elements/background/4.png",
	"images/elements/background/5.png",
	"images/elements/background/6.png",
	"images/elements/background/7.png",
	"images/elements/background/8.png",
	"images/elements/background/9.png",
};

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font* gFont = nullptr;
Mix_Music* gMusic = nullptr;
Mix_Music* gMenuMusic = nullptr;
Mix_Chunk* gClick = nullptr;
Mix_Chunk* gJump = nullptr;
Mix_Chunk* gLose = nullptr;
Mix_Chunk* gShoot = nullptr;
Mix_Chunk* gBuff = nullptr;

SDL_Rect gPlayButton[BUTTON_TOTAL];
SDL_Rect gHelpButton[BUTTON_TOTAL];
SDL_Rect gExitButton[BUTTON_TOTAL];
SDL_Rect gBackButton[BUTTON_TOTAL];
SDL_Rect gPauseButton[BUTTON_TOTAL];
//
SDL_Rect gAgainButton[BUTTON_TOTAL];
SDL_Rect gMenuButton[BUTTON_TOTAL];
SDL_Rect gCloseButton[BUTTON_TOTAL];
SDL_Rect gSettingButton[BUTTON_TOTAL];
SDL_Rect gSoundOnButton[BUTTON_TOTAL];
SDL_Rect gSoundOffButton[BUTTON_TOTAL];
//SDL_Rect gVolumeLine[BUTTON_TOTAL];
SDL_Rect gLabtube[3]; // Labtube có 3 frames
SDL_Rect gHeart[4]; // Character có 4 mạng
SDL_Rect gOkButton[BUTTON_TOTAL];
//
SDL_Rect gContinueButton[BUTTON_TOTAL];
//SDL_Rect gPlayAgainButton[BUTTON_TOTAL];
SDL_Rect gCharacterClips[RUNNING_FRAMES];
//SDL_Rect gEnemyClips[FLYING_FRAMES];
SDL_Rect gBackPageButton[BUTTON_TOTAL];
SDL_Rect gNextPageButton[BUTTON_TOTAL];
SDL_Rect gExitHelpButton[BUTTON_TOTAL];

LTexture gMenuTexture;
//LTexture gInstructionTexture;
LTexture gBackgroundTexture[BACKGROUND_LAYER];
LTexture gCharacterTexture[4];
//LTexture gGroundTexture;
//LTexture gPlayButtonTexture;
//LTexture gHelpButtonTexture;
//LTexture gExitButtonTexture;
LTexture gBackButtonTexture;
LTexture gPauseButtonTexture;
LTexture gContinueButtonTexture;
LTexture gLoseTexture;
LTexture gText1Texture;
LTexture gScoreTexture;
LTexture gText2Texture;
LTexture gHighScoreTexture;
LTexture gRemainBulletTexture;

//
LTexture gNextPageButtonTexture;
LTexture gBackPageButtonTexture;
LTexture gAgainButtonTexture;
LTexture gPlayButtonTexture;
LTexture gHelpButtonTexture;
LTexture gQuitButtonTexture;
LTexture gMenuButtonTexture;
LTexture gCloseButtonTexture;
LTexture gSettingButtonTexture;
LTexture gSoundOnButtonTexture;
LTexture gSoundOffButtonTexture;

LTexture gExitHelpButtonTexture;
//LTexture gVolumeLineTexture;
LTexture gOkButtonTexture;
LTexture gTransTexture;
LTexture gVineTexture;
LTexture gLabtubeTexture;
LTexture gNameTexture;
LTexture gHeartTexture;
//LTexture gEndgameTexture;
LTexture gGlowTexture;
//LTexture gSoundOnTexture;
//LTexture gSoundOffTexture;
LTexture gBulletTexture;
LTexture gSkinSelectorTexture[5];
LTexture gHelpTexture[4];
LTexture gBackgroundCoatingTexture;


Button PlayButton(PLAY_BUTTON_POSX, PLAY_BUTTON_POSY);
Button HelpButton(HELP_BUTTON_POSX, HELP_BUTTON_POSY);
Button ExitButton(EXIT_BUTTON_POSX, EXIT_BUTTON_POSY);
Button BackButton(BACK_BUTTON_POSX, BACK_BUTTON_POSY);
Button PauseButton(PAUSE_BUTTON_POSX, PAUSE_BUTTON_POSY);
Button ContinueButton(CONTINUE_BUTTON_POSX, CONTINUE_BUTTON_POSY);
Button CloseButton(CLOSE_BUTTON_POSX, CLOSE_BUTTON_POSY);
Button SettingButton(SETTING_BUTTON_POSX, SETTING_BUTTON_POSY);
Button MenuButton(MENU_BUTTON_POSX, MENU_BUTTON_POSY);
//Button QuitButton(QUIT_BUTTON_POSX, QUIT_BUTTON_POSY);
Button AgainButton(AGAIN_BUTTON_POSX, AGAIN_BUTTON_POSY);
Button SoundOnButton(SOUND_BUTTON_POSX, SOUND_BUTTON_POSY);
Button SoundOffButton(SOUND_BUTTON_POSX, SOUND_BUTTON_POSY);
Button OkButton(OK_BUTTON_POSX, OK_BUTTON_POSY);
Button BackPageButton(BACKPAGE_BUTTON_POSX, BACKPAGE_BUTTON_POSY);
Button NextPageButton(NEXTPAGE_BUTTON_POSX, NEXTPAGE_BUTTON_POSY);
Button ExitHelpButton(EXITHELP_BUTTON_POSX, EXITHELP_BUTTON_POSY);
//

// Constants
const SDL_Color textColor = {255, 255, 255, 150};
const SDL_Color COLOR_BLACK = {255, 255, 255, 255};
const int PLAYER_SIZE = 100;
const int PLAYER_COLLISION_SIZE = 50;
const int GROUND_LEVEL_HIGH = SCREEN_HEIGHT - PLAYER_SIZE - 100;
const int GROUND_LEVEL_LOW = SCREEN_HEIGHT - PLAYER_SIZE;
const float JUMP_FORCE = -14.0f;
const float GRAVITY = 0.5f;
const int PLATFORM_HEIGHT = 10;
const float SPIKE_PROBABILITY = 0.9f;
const int MAX_SPIKES_PER_PLATFORM = 7;
const int MAX_PLATFORM_GAP = 10;
const int BULLET_SIZE = 50;
const float BULLET_SPEED = 5.0f;
const int BULLET_RANGE = 200;
const float POWERUP_PROBABILITY = 0.9f;
const int POWERUP_SIZE = 50;
const int addBullets = 3;
const int RENDER_OFFSET_Y = -23;
const int ROBOT_SIZE = 70;
const int POISON_SIZE = 70;
const int ROBOT_COLLISION_SIZE = 50;
const int POSION_COLLISION_SIZE = 45;
const Uint32 SCORE_INTERVAL = 500;


// Global Variables
int counttloop = 0;
int REMAINING_HEARTS = 3;
int bulletCount = 0;
int currentGroundLevel = GROUND_LEVEL_LOW;
bool isOnGround = false;
bool collided = true;
bool running = true;
bool isJumping = false;
float beginningObstacleSpeed = 5;
float obstacleSpeed = 5.0f;

SDL_Texture* groundTexture = nullptr;
SDL_Texture* spikeTexture = nullptr;
SDL_Texture* spike2SheetTexture = nullptr;
//SDL_Texture* bulletTexture = nullptr;
SDL_Texture* powerUpTexture = nullptr;
SDL_Texture* robotTexture = nullptr;
SDL_Texture* gameStateTexture = nullptr;
//SDL_Texture* glowTexture = nullptr;
//SDL_Texture* loseTexture = nullptr;
SDL_Texture* sliderTexture = nullptr;
SDL_Texture* knobTexture = nullptr;

// Structures
struct GameObject {
    SDL_Rect rect;          // Render
    SDL_Rect collisionRect; // CollisionCheck
    SDL_Color color;
    float x_velocity;
    float y_velocity;
    bool isLethal;
    int currentFrame;
    int type;
};

struct Bullet {
    SDL_Rect rect;          // Render
    SDL_Rect collisionRect; // CollisionCheck
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

#endif
