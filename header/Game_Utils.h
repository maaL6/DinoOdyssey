#ifndef GAME_UTILS_H_
#define GAME_UTILS_H_

#include "Game_Base.h"
#include "Button.h"

bool Init();
bool LoadMedia();
void Close();

std::string GetHighScoreFromFile(std::string path);

void UpdateHighScore(std::string path,
    const int& score,
    const std::string& old_high_score);

int UpdateGameTimeAndScore(int& time, int& speed, int& score);

void RenderScrollingBackground(std::vector <double>& offsetSpeed,
    LTexture(&gBackgroundTexture)[BACKGROUND_LAYER],
    SDL_Renderer* gRenderer);

void RenderScrollingGround(int& speed,
    const int acceleration,
    LTexture gGroundTexture,
    SDL_Renderer* gRenderer);

void HandlePlayButton(SDL_Event* e,
    Button& PlayButton,
    bool& QuitMenu,
    bool& Play,
    Mix_Chunk* gClick);

void HandleHelpButton(SDL_Event* e,
    SDL_Rect(&gBackButton)[BUTTON_TOTAL],
    Button& HelpButton,
    Button& BackButton,
    LTexture gInstructionTexture,
    LTexture gBackButtonTexture,
    SDL_Renderer* gRenderer,
    bool& Quit_game,
    Mix_Chunk* gClick);

void HandleExitButton(SDL_Event* e,
    Button& ExitButton,
    bool& Quit,
    Mix_Chunk* gClick);

void HandleCloseButton(SDL_Event* e,
    Button& CloseButton,
    bool& Quit,
    Mix_Chunk* gClick);

void HandleSettingButton(SDL_Event* e,
    Button& gSettingButton,
    Mix_Chunk* gClick);

void HandleMenuButton(SDL_Event* e,
    Button& gMenuButton,
    Mix_Chunk* gClick);


/*void HandleContinueButton(Button ContinueButton,
    LTexture gContinueButtonTexture,
    SDL_Event* e,
    SDL_Renderer* gRenderer,
    SDL_Rect(&gContinueButton)[BUTTON_TOTAL],
    bool& Game_State, Mix_Chunk* gClick);*/

void HandleContinueButton(Button& ContinueButton,  // Tham chiếu để thay đổi trực tiếp ContinueButton
                          LTexture gContinueButtonTexture,
                          SDL_Event* e,
                          SDL_Renderer* gRenderer,
                          SDL_Rect(&gContinueButton)[BUTTON_TOTAL],
                          bool& Game_State,
                          Mix_Chunk* gClick);

void HandlePauseButton(SDL_Event* e,
                       SDL_Renderer* gRenderer,
                       SDL_Rect (&gContinueButton)[BUTTON_TOTAL],  // Tham chiếu đến mảng
                       Button& PauseButton,
                       Button& ContinueButton,  // Tham chiếu để thay đổi trực tiếp ContinueButton
                       LTexture gContinueButtonTexture,
                       bool &Game_State,
                       Mix_Chunk *gClick);

void HandleSoundButton(SDL_Event* e,
                       int& currentX,
                       Button& SoundOnButton,
                       Button& SoundOffButton,
                       Mix_Chunk *gClick);

bool HandleAgainButton(SDL_Event* e,
                       Button& AgainButton,
                       Mix_Chunk* gClick);

bool HandleQuitButton(SDL_Event* e, Button& QuitButton, Mix_Chunk* gClick);

void ControlCharFrame(int& frame);

void ControlLabtubeFrame(int &frame);

void DrawPlayerScore(LTexture gTextTexture,
    LTexture gScoreTexture,
    SDL_Color textColor,
    SDL_Renderer* gRenderer,
    TTF_Font* gFont,
    const int& score);

void DrawPlayerHighScore(LTexture gTextTexture,
    LTexture gHighScoreTexture,
    SDL_Color textColor,
    SDL_Renderer* gRenderer,
    TTF_Font* gFont,
    const std::string& HighScore);

void DrawEndGameSelection(LTexture gLoseTexture,
    SDL_Event* e,
    SDL_Renderer* gRenderer,
    bool& Play_Again);

#endif // !GAME_UTILS_H_
