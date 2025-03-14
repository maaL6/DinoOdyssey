#include "Game_Utils.h"

std::string GetHighScoreFromFile(std::string path) {
    std::fstream HighScoreFile;
    std::string highscore;

    HighScoreFile.open(path, std::ios::in);
    HighScoreFile >> highscore;

    return highscore;
}

void UpdateHighScore(std::string path,
                     const int& score,
                     const std::string& old_high_score) {
    int oldHighScore = 0;
    std::fstream HighScoreFile;
    std::string newHighScore;
    std::stringstream ConvertToInt(old_high_score);

    HighScoreFile.open(path, std::ios::out);

    ConvertToInt >> oldHighScore;
    if (score > oldHighScore) {
        oldHighScore = score;
    }
    newHighScore = std::to_string(oldHighScore);

    HighScoreFile << newHighScore;
}

int UpdateGameTimeAndScore(int& time,
                           int& speed,
                           int& score) {
    if (time == TIME_MAX) {
        speed += SPEED_INCREASEMENT;
    }

    if (time > TIME_MAX) {
        time = 0;
    }
    if (time % 5 == 0) {
        score += SCORE_INCREASEMENT;
    }

    time += TIME_INCREASEMENT;

    return time;
}

void RenderScrollingBackground(std::vector <double>& offsetSpeed,
                               LTexture(&gBackgroundTexture)[BACKGROUND_LAYER],
                               SDL_Renderer* gRenderer) {
    std::vector <double> layer_speed;
    layer_speed.push_back(LAYER_1_SPEED);
    layer_speed.push_back(LAYER_2_SPEED);
    layer_speed.push_back(LAYER_3_SPEED);
    layer_speed.push_back(LAYER_4_SPEED);
    layer_speed.push_back(LAYER_5_SPEED);
    layer_speed.push_back(LAYER_6_SPEED);
    layer_speed.push_back(LAYER_7_SPEED);
    layer_speed.push_back(LAYER_8_SPEED);
    layer_speed.push_back(LAYER_9_SPEED);

    for (int i = 0; i < BACKGROUND_LAYER; ++i) {
        offsetSpeed[i] -= layer_speed[i];
        if (offsetSpeed[i] < -gBackgroundTexture[i].GetWidth()) {
            offsetSpeed[i] = 0;
        }
        gBackgroundTexture[i].Render(offsetSpeed[i], 0, gRenderer);
        gBackgroundTexture[i].Render(offsetSpeed[i] + gBackgroundTexture[i].GetWidth(), 0, gRenderer);
    }
}

void RenderScrollingGround(int& speed,
                           const int acceleration,
                           LTexture gGroundTexture,
                           SDL_Renderer* gRenderer) {
    speed -= GROUND_SPEED + acceleration;
    if (speed < -gGroundTexture.GetWidth()) {
        speed = 0;
    }
    gGroundTexture.Render(speed, 0, gRenderer);
    gGroundTexture.Render(speed + gGroundTexture.GetWidth(), 0, gRenderer);
}

void HandlePlayButton(SDL_Event* e,
                      Button& PlayButton,
                      bool& QuitMenu,
                      bool& Play,
                      Mix_Chunk* gClick) {
    if (e->type == SDL_QUIT) {
        QuitMenu = true;
    }

    if (PlayButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            PlayButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            Play = true;
            QuitMenu = true;
            Mix_PlayChannel(MIX_CHANNEL, gClick, 0);
            PlayButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        }
    } else {
        PlayButton.currentSprite = BUTTON_MOUSE_OUT;
    }
}

void HandleHelpButton(SDL_Event* e,
                      SDL_Rect(&gBackButton)[BUTTON_TOTAL],
                      Button& HelpButton,
                      Button& BackButton,
                      LTexture gInstructionTexture,
                      LTexture gBackButtonTexture,
                      SDL_Renderer *gRenderer,
                      bool &Quit_game,
                      Mix_Chunk *gClick) {
    if (HelpButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            HelpButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            HelpButton.currentSprite = BUTTON_MOUSE_OVER;
            Mix_PlayChannel(MIX_CHANNEL, gClick, NOT_REPEATITIVE);

            bool ReadDone = false;
            while (!ReadDone) {
                do {
                    if (e->type == SDL_QUIT) {
                        ReadDone = true;
                        Quit_game = true;
                        Close();
                    }

                    else if (BackButton.IsInside(e, COMMON_BUTTON)) {
                        switch (e->type) {
                        case SDL_MOUSEMOTION:
                            BackButton.currentSprite = BUTTON_MOUSE_OVER;
                            break;
                        case SDL_MOUSEBUTTONDOWN:
                            BackButton.currentSprite = BUTTON_MOUSE_OVER;
                            Mix_PlayChannel(MIX_CHANNEL, gClick, NOT_REPEATITIVE);
                            ReadDone = true;
                            break;
                        }
                    } else {
                        BackButton.currentSprite = BUTTON_MOUSE_OUT;
                    }

                    gInstructionTexture.Render(0, 0, gRenderer);

                    SDL_Rect* currentClip_Back = &gBackButton[BackButton.currentSprite];
                    BackButton.Render(currentClip_Back, gRenderer, gBackButtonTexture);

                    SDL_RenderPresent(gRenderer);
                } while (SDL_PollEvent(e) != 0 && e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEMOTION);
            }
            break;
        }
    } else {
        HelpButton.currentSprite = BUTTON_MOUSE_OUT;
    }
}

void HandleExitButton(SDL_Event* e,
                      Button& ExitButton,
                      bool& Quit,
                      Mix_Chunk* gClick) {
    if (ExitButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            ExitButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            Quit = true;
            ExitButton.currentSprite = BUTTON_MOUSE_OVER;
            Mix_PlayChannel(MIX_CHANNEL, gClick, NOT_REPEATITIVE);
            break;
        }
    } else {
        ExitButton.currentSprite = BUTTON_MOUSE_OUT;
    }
}

void HandleCloseButton(SDL_Event* e,
                       Button& CloseButton,
                       bool& Quit,
                       Mix_Chunk* gClick) {
    if (CloseButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            CloseButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            Quit = true;
            CloseButton.currentSprite = BUTTON_MOUSE_OVER;
            Mix_PlayChannel(MIX_CHANNEL, gClick, NOT_REPEATITIVE);
            break;
        }
    } else {
        CloseButton.currentSprite = BUTTON_MOUSE_OUT;
    }
}

void HandleSettingButton(SDL_Event* e,
                         Button& SettingButton,
                         Mix_Chunk* gClick) {
    if (SettingButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            SettingButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            SettingButton.currentSprite = BUTTON_MOUSE_OVER;
            //Them lenh
            break;
        }
    } else {
        SettingButton.currentSprite = BUTTON_MOUSE_OUT;
    }
}

void HandleMenuButton(SDL_Event* e,
                      Button& MenuButton,
                      Mix_Chunk* gClick) {
    if (MenuButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            MenuButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            MenuButton.currentSprite = BUTTON_MOUSE_OVER;
            //Them lenh
            break;
        }
    } else {
        MenuButton.currentSprite = BUTTON_MOUSE_OUT;
    }
}

void HandleContinueButton(Button& ContinueButton,
                          LTexture gContinueButtonTexture,
                          SDL_Event* e,
                          SDL_Renderer* gRenderer,
                          SDL_Rect(&gContinueButton)[BUTTON_TOTAL],
                          bool& Game_State,
                          Mix_Chunk* gClick) {
    if (ContinueButton.IsInside2(e, SMALL_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            ContinueButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            ContinueButton.currentSprite = BUTTON_MOUSE_OVER;
            Mix_PlayChannel(MIX_CHANNEL, gClick, NOT_REPEATITIVE);
            Mix_ResumeMusic();
            Game_State = true;
            break;
        }
    } else {
        ContinueButton.currentSprite = BUTTON_MOUSE_OUT;
    }

    SDL_Rect* currentClip_Continue = &gContinueButton[ContinueButton.currentSprite];
    ContinueButton.Render(currentClip_Continue, gRenderer, gContinueButtonTexture);
}

void HandlePauseButton(SDL_Event* e,
                       SDL_Renderer* gRenderer,
                       SDL_Rect (&gContinueButton)[BUTTON_TOTAL],
                       Button& PauseButton,
                       Button& ContinueButton,
                       LTexture gContinueButtonTexture,
                       bool &Game_State,
                       Mix_Chunk *gClick) {
    if (Game_State) {
        if (PauseButton.IsInside2(e, SMALL_BUTTON)) {
            switch (e->type) {
            case SDL_MOUSEMOTION:
                PauseButton.currentSprite = BUTTON_MOUSE_OVER;
                break;
            case SDL_MOUSEBUTTONDOWN:
                PauseButton.currentSprite = BUTTON_MOUSE_OVER;
                Mix_PlayChannel(MIX_CHANNEL, gClick, NOT_REPEATITIVE);
                Mix_PauseMusic();
                Game_State = false; 
                break;
            }
        } else {
            PauseButton.currentSprite = BUTTON_MOUSE_OUT;
        }
    }
    else {
        HandleContinueButton(ContinueButton, gContinueButtonTexture, e, gRenderer, gContinueButton, Game_State, gClick);
    }
}

void HandleSoundButton(SDL_Event* e,
                       int& currentX,
                       Button& SoundOnButton,
                       Button& SoundOffButton,
                       Mix_Chunk *gClick) {
    if (currentX > 60) {
        if (SoundOnButton.IsInside2(e, SMALL_BUTTON)) {
            switch (e->type) {
            case SDL_MOUSEMOTION:
                SoundOnButton.currentSprite = BUTTON_MOUSE_OVER;
                break;
            case SDL_MOUSEBUTTONDOWN:
                SoundOnButton.currentSprite = BUTTON_MOUSE_OVER;
                Mix_PlayChannel(MIX_CHANNEL, gClick, NOT_REPEATITIVE);
                currentX = 60;
                break;
            }
        } else {
            SoundOnButton.currentSprite = BUTTON_MOUSE_OUT;
        }
    } else {
        if (SoundOffButton.IsInside2(e, SMALL_BUTTON)) {
            switch (e->type) {
            case SDL_MOUSEMOTION:
                SoundOffButton.currentSprite = BUTTON_MOUSE_OVER;
                break;
            case SDL_MOUSEBUTTONDOWN:
                SoundOffButton.currentSprite = BUTTON_MOUSE_OVER;
                Mix_PlayChannel(MIX_CHANNEL, gClick, NOT_REPEATITIVE);
                currentX = 150;
                break;
            }
        } else {
            SoundOffButton.currentSprite = BUTTON_MOUSE_OUT;
        }
    }
}


bool HandleAgainButton(SDL_Event* e, Button& AgainButton, Mix_Chunk* gClick) {
    bool clicked = false;
    if (AgainButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            AgainButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            AgainButton.currentSprite = BUTTON_MOUSE_OVER;
            Mix_PlayChannel(-1, gClick, 0); 
            clicked = true; 
            break;
        }
    } else {
        AgainButton.currentSprite = BUTTON_MOUSE_OUT;
    }
    return clicked; 
}

bool HandleQuitButton(SDL_Event* e, Button& QuitButton, Mix_Chunk* gClick) {
    bool clicked = false;
    if (QuitButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            QuitButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            QuitButton.currentSprite = BUTTON_MOUSE_OVER;
            Mix_PlayChannel(-1, gClick, 0); 
            clicked = true; 
            break;
        }
    } else {
        QuitButton.currentSprite = BUTTON_MOUSE_OUT;
    }
    return clicked; 
}

void ControlCharFrame(int &frame) {
    frame += FRAME_INCREASEMENT;
    if (frame / SLOW_FRAME_CHAR >= RUNNING_FRAMES) {
        frame = 0;
    }
    //int RUNNING_FRAMES = 0;
}

void ControlEnemyFrame(int &frame) {
    frame += FRAME_INCREASEMENT;
    if (frame / SLOW_FRAME_ENEMY >= FLYING_FRAMES) {
        frame = 0;
    }
}

void ControlLabtubeFrame(int &frame) {
    frame += FRAME_INCREASEMENT;
    if (frame / 10 >= 3) {
        frame = 0;
    }
}


void DrawPlayerScore(LTexture gTextTexture,
                     LTexture gScoreTexture,
                     SDL_Color textColor,
                     SDL_Renderer *gRenderer,
                     TTF_Font *gFont,
                     const int& score) {
    gTextTexture.Render(TEXT_1_POSX, TEXT_1_POSY, gRenderer);
    if (gScoreTexture.LoadFromRenderedText(std::to_string(score), gFont, textColor, gRenderer)) {
        gScoreTexture.Render(SCORE_POSX, SCORE_POSY, gRenderer);
    }
}

void DrawPlayerHighScore(LTexture gTextTexture,
                         LTexture gHighScoreTexture,
                         SDL_Color textColor,
                         SDL_Renderer* gRenderer,
                         TTF_Font* gFont,
                         const std::string& HighScore) {
    gTextTexture.Render(TEXT_2_POSX, TEXT_2_POSY, gRenderer);
    if (gHighScoreTexture.LoadFromRenderedText(HighScore, gFont, textColor, gRenderer)) {
        gHighScoreTexture.Render(HIGH_SCORE_POSX, HIGH_SCORE_POSY, gRenderer);
    }
}

void DrawEndGameSelection(LTexture gLoseTexture,
                          SDL_Event *e,
                          SDL_Renderer *gRenderer,
                          bool &Play_Again) {
    if (Play_Again) {
        bool End_Game = false;
        while (!End_Game) {
            while (SDL_PollEvent(e) != 0) {
                if (e->type == SDL_QUIT) {
                    Play_Again = false;
                }

                if (e->type == SDL_KEYDOWN) {
                    switch (e->key.keysym.sym) {
                    case SDLK_SPACE:
                        //resetGame(player, obstacles, running, isJumping, obstacleSpeed, currentGroundLevel);
                        End_Game = true;
                        break;
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
    }
}
