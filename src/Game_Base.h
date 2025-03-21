#ifndef GAME_BASE_
#define GAME_BASE_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

#define SDL_ERROR 1
#define IMG_ERROR 2
#define MIX_ERROR 3
#define TTF_ERROR 4

#define MIX_CHANNEL -1

#define TIME_MAX 1000
#define GROUND 430
#define MAX_HEIGHT 250

#define BASE_OFFSET_SPEED 0

#define SPEED_INCREASEMENT 2
#define SCORE_INCREASEMENT 1
#define TIME_INCREASEMENT 1
#define FRAME_INCREASEMENT 1

#define GROUND_SPEED 5
#define ENEMY_SPEED 5
#define MAX_ENEMY_WIDTH 100


#define IN_AIR_ENEMY 1
#define ON_GROUND_ENEMY 0

#define SLOW_FRAME_CHAR 4
#define SLOW_FRAME_ENEMY 4

#define IS_REPEATITIVE -1
#define NOT_REPEATITIVE 0

#define SMALL_BUTTON 1
#define COMMON_BUTTON 2

const std::string WINDOW_TITLE = "Dino Odyssey";

//float OBSTACLE_SPEED = 5.0f;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 500;

const int COMMON_BUTTON_WIDTH = 175;
const int COMMON_BUTTON_HEIGHT = 90;
const int SMALL_BUTTON_WIDTH = 22;
const int SMALL_BUTTON_HEIGHT = 34;

const int PLAY_BUTTON_POSX = 420;
const int PLAY_BUTTON_POSY= 230;
const int HELP_BUTTON_POSX = 420;
const int HELP_BUTTON_POSY = 300;
const int EXIT_BUTTON_POSX = 420;
const int EXIT_BUTTON_POSY = 300;
const int CLOSE_BUTTON_POSX = 740;
const int CLOSE_BUTTON_POSY = 0;
const int SETTING_BUTTON_POSX = 740;
const int SETTING_BUTTON_POSY = 45;
const int MENU_BUTTON_POSX = 740;
const int MENU_BUTTON_POSY = 90;
const int BACK_BUTTON_POSX = 31;
const int BACK_BUTTON_POSY = 29;
const int PAUSE_BUTTON_POSX = 744;
const int PAUSE_BUTTON_POSY = 0;
const int CONTINUE_BUTTON_POSX = 744;
const int CONTINUE_BUTTON_POSY = 0;

const int BULLET_POSX = 20;
const int BULLET_POSY = 15;
const int BULLET_COUNT_POSX = 100;
const int BULLET_COUNT_POSY = 15;
const int TEXT_1_POSX = 470;
const int TEXT_1_POSY = 15;
const int TEXT_2_POSX = 570;
const int TEXT_2_POSY = 15;
const int SCORE_POSX = 552;
const int SCORE_POSY = 15;
const int HIGH_SCORE_POSX = 630;
const int HIGH_SCORE_POSY = 15;
const int AGAIN_BUTTON_POSX = 200;
const int AGAIN_BUTTON_POSY = 300;
const int REMAINING_HEARTS_POSX = 20;
const int REMAINING_HEARTS_POSY = 425;
const int SOUND_BUTTON_POSX = 0;
const int SOUND_BUTTON_POSY = 435;
const int OK_BUTTON_POSX = 310;
const int OK_BUTTON_POSY = 370;
const int SLIDER_START_X = 60;
const int SLIDER_END_X = 60 + 170;
const int SLIDER_Y = 465;
const int KNOB_SIZE = 40;
const int NUMBER_OF_INSTRUCTION_PAGES = 4;
const int BACKPAGE_BUTTON_POSX = 693;
const int BACKPAGE_BUTTON_POSY = 440;
const int NEXTPAGE_BUTTON_POSX = 740;
const int NEXTPAGE_BUTTON_POSY = 440;
const int EXITHELP_BUTTON_POSX = 2;
const int EXITHELP_BUTTON_POSY = 3;


const double LAYER_1_SPEED = 0.0;
const double LAYER_2_SPEED = 0.25;
const double LAYER_3_SPEED = 0.5;
const double LAYER_4_SPEED = 0.75;
const double LAYER_5_SPEED = 1.0;
const double LAYER_6_SPEED = 1.25;
const double LAYER_7_SPEED = 1.5;
const double LAYER_8_SPEED = 1.75;
const double LAYER_9_SPEED = 2.0;

const int RUNNING_FRAMES = 4;
const int BACKGROUND_LAYER = 9;

enum ButtonSprite
{
	BUTTON_MOUSE_OUT = 0,
	BUTTON_MOUSE_OVER = 1,
	BUTTON_TOTAL = 2
};

void LogError(std::string msg, int error_code = SDL_ERROR);

#endif // !GAME_LIB_
