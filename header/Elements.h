#ifndef ELEMENTS_H_
#define ELEMENTS_H_

#include "Button.h"
#include "Character.h"
#include "Enemy.h"
#include "Game_Base.h"
#include "Game_Utils.h"
#include "LTexture.h"

const std::string LAYER[BACKGROUND_LAYER] = {
	"imgs/background/layer01.png",
	"imgs/background/layer02.png",
	"imgs/background/layer03.png",
	"imgs/background/layer04.png",
	"imgs/background/layer05.png",
	"imgs/background/layer06.png",
	"imgs/background/layer07.png",
	"imgs/background/layer08.png",
	"imgs/background/layer09.png",
};

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Color textColor = { 0, 0, 0 };
TTF_Font* gFont = nullptr;
Mix_Music* gMusic = nullptr;
Mix_Music* gMenuMusic = nullptr;
Mix_Chunk* gClick = nullptr;
Mix_Chunk* gJump = nullptr;
Mix_Chunk* gLose = nullptr;

SDL_Rect gPlayButton[BUTTON_TOTAL];
SDL_Rect gHelpButton[BUTTON_TOTAL];
SDL_Rect gExitButton[BUTTON_TOTAL];
SDL_Rect gBackButton[BUTTON_TOTAL];
SDL_Rect gPauseButton[BUTTON_TOTAL];
//
SDL_Rect gMenuButton[BUTTON_TOTAL];
SDL_Rect gCloseButton[BUTTON_TOTAL];
SDL_Rect gSettingButton[BUTTON_TOTAL];
SDL_Rect gSoundOnButton[BUTTON_TOTAL];
SDL_Rect gSoundOffButton[BUTTON_TOTAL];
SDL_Rect gVolumeLine[BUTTON_TOTAL];
SDL_Rect gLabtube[3];
//
SDL_Rect gContinueButton[BUTTON_TOTAL];
SDL_Rect gPlayAgainButton[BUTTON_TOTAL];
SDL_Rect gCharacterClips[RUNNING_FRAMES];
SDL_Rect gEnemyClips[FLYING_FRAMES];

LTexture gMenuTexture;
LTexture gInstructionTexture;
LTexture gBackgroundTexture[BACKGROUND_LAYER];
LTexture gCharacterTexture;
LTexture gGroundTexture;
//LTexture gPlayButtonTexture;
//LTexture gHelpButtonTexture;
LTexture gExitButtonTexture;
LTexture gBackButtonTexture;
LTexture gPauseButtonTexture;
LTexture gContinueButtonTexture;
LTexture gLoseTexture;
LTexture gText1Texture;
LTexture gScoreTexture;
LTexture gText2Texture;
LTexture gHighScoreTexture;

//
LTexture gPlayButtonTexture;
LTexture gHelpButtonTexture;
LTexture gQuitButtonTexture;
LTexture gMenuButtonTexture;
LTexture gCloseButtonTexture;
LTexture gSettingButtonTexture;
LTexture gSoundOnButtonTexture;
LTexture gSoundOffButtonTexture;
LTexture gVolumeLineTexture;
LTexture gTransTexture;
LTexture gVineTexture;
LTexture gLabtubeTexture;
LTexture gNameTexture;
//
Button PlayButton(PLAY_BUTTON_POSX, PLAY_BUTTON_POSY);
Button HelpButton(HELP_BUTTON_POSX, HELP_BUTTON_POSY);
Button ExitButton(EXIT_BUTTON_POSX, EXIT_BUTTON_POSY);
Button BackButton(BACK_BUTTON_POSX, BACK_BUTTON_POSY);
Button PauseButton(PAUSE_BUTTON_POSX, PAUSE_BUTTON_POSY);
Button ContinueButton(CONTINUE_BUTTON_POSX, CONTINUE_BUTTON_POSY);
//
Button CloseButton(CLOSE_BUTTON_POSX, CLOSE_BUTTON_POSY);
Button SettingButton(SETTING_BUTTON_POSX, SETTING_BUTTON_POSY);
Button MenuButton(MENU_BUTTON_POSX, MENU_BUTTON_POSY);
//

#endif
