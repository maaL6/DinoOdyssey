#ifndef GAME_UTILS_H_
#define GAME_UTILS_H_

#include "Game_Base.h"
#include "Button.h"

bool Init();
bool LoadMedia();
void Close();

/*
std::string GetHighScoreFromFile(const std::string& path);

void UpdateHighScore(const std::string& path, const int& score, const std::string& old_high_score);

int UpdateGameTimeAndScore(int& time, int& score);
*/

int GetHighScoreFromFile(const std::string& path);
void UpdateHighScore(const std::string& path, int score, int old_high_score);
int UpdateGameTimeAndScore(int& time, int& score);

void RenderScrollingBackground(std::vector <double>& offsetSpeed,
    LTexture(&gBackgroundTexture)[BACKGROUND_LAYER],
    SDL_Renderer* gRenderer);

void HandlePlayButton(SDL_Event* e,
    Button& PlayButton,
    bool& QuitMenu,
    bool& Play,
    Mix_Chunk* gClick);

bool HandleHelpButton(SDL_Event* e,
    Button& HelpButton,
    SDL_Renderer* gRenderer,
    bool& Quit_game,
    Mix_Chunk* gClick);

void HandleNextPageButton(SDL_Event* e,
                      Button& NextPageButton,
                      int& pageNumber,
                      Mix_Chunk* gClick);

void HandleBackButton(SDL_Event* e,
                      Button& NextPageButton,
                      int& pageNumber,
                      Mix_Chunk* gClick);

void HandleExitHelpButton(SDL_Event* e,
                      Button& NextPageButton,
                      bool& Quit_Menu,
                      bool& Quit_Instruction,
                      bool& Play_Again,
                      Mix_Chunk* gClick);

void HandleExitButton(SDL_Event* e,
    Button& ExitButton,
    bool& Quit,
    Mix_Chunk* gClick);

void HandleCloseButton(SDL_Event* e,
    Button& CloseButton,
    bool& Quit,
    Mix_Chunk* gClick);

bool HandleSettingButton(SDL_Event* e,
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

void DrawPlayerScore(LTexture gTextTexture,
    LTexture gScoreTexture,
    SDL_Color textColor,
    SDL_Renderer* gRenderer,
    TTF_Font* gFont,
    const int& score);
void DrawBullet(LTexture gTextTexture,
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
    int highscore);

bool isMouseInRect(SDL_Event *e, SDL_Rect rect);

#endif // !GAME_UTILS_H_
