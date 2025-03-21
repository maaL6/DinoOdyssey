#include "Game_Utils.h"

int GetHighScoreFromFile(const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");
    int highscore = 0;

    if (file) {
        char buffer[16] = {0};
        if (fgets(buffer, sizeof(buffer), file) != nullptr) {
            try {
                highscore = std::stoi(std::string(buffer));
            } catch (const std::exception& e) {
                highscore = 0;
            }
        }
        fclose(file);
    } else {
        file = fopen(path.c_str(), "w");
        if (file) {
            fprintf(file, "0");
            fclose(file);
        }
    }
    return highscore;
}

void UpdateHighScore(const std::string& path, int score, int old_high_score) {
    if (score > old_high_score) {
        FILE* file = fopen(path.c_str(), "w");
        if (file) {
            fprintf(file, "%d", score);
            fclose(file);
        }
    }
}

int UpdateGameTimeAndScore(int& time, int& score) {
    if (time > TIME_MAX) {
        time = 0;
    }
    if (time % 7 == 0) {
        score += SCORE_INCREASEMENT;
    }
    time += TIME_INCREASEMENT;
    return time;
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

bool HandleHelpButton(SDL_Event* e,
                      Button& HelpButton,
                      SDL_Renderer *gRenderer,
                      bool &Quit_game,
                      Mix_Chunk *gClick) {
    bool clicked = false;
    if (HelpButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            HelpButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            HelpButton.currentSprite = BUTTON_MOUSE_OVER;
            Mix_PlayChannel(-1, gClick, 0);
            clicked = true;
            break;
        }
    } else {
        HelpButton.currentSprite = BUTTON_MOUSE_OUT;
    }
    return clicked;
}

void HandleNextPageButton(SDL_Event* e,
                      Button& NextPageButton,
                      int& pageNumber,
                      Mix_Chunk* gClick) {
    if (NextPageButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            NextPageButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(pageNumber < 3)
                pageNumber++;
            std::cout<<pageNumber;
            Mix_PlayChannel(MIX_CHANNEL, gClick, 0);
            NextPageButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        }
    } else {
        NextPageButton.currentSprite = BUTTON_MOUSE_OUT;
    }
}

void HandleExitHelpButton(SDL_Event* e,
                      Button& NextPageButton,
                      bool& Quit_Menu,
                      bool& Quit_Instruction,
                      bool& Play_Again,
                      Mix_Chunk* gClick) {
    if (NextPageButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            NextPageButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            Quit_Menu = false;
            Quit_Instruction = true;
            Play_Again = false;
            Mix_PlayChannel(MIX_CHANNEL, gClick, 0);
            NextPageButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        }
    } else {
        NextPageButton.currentSprite = BUTTON_MOUSE_OUT;
    }
}

void HandleBackButton(SDL_Event* e,
                      Button& NextPageButton,
                      int& pageNumber,
                      Mix_Chunk* gClick) {
    if (NextPageButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            NextPageButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(pageNumber > 0)
                pageNumber--;
            std::cout<<pageNumber;
            Mix_PlayChannel(MIX_CHANNEL, gClick, 0);
            NextPageButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        }
    } else {
        NextPageButton.currentSprite = BUTTON_MOUSE_OUT;
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

bool HandleSettingButton(SDL_Event* e,
                         Button& SettingButton,
                         Mix_Chunk* gClick) {
    bool clicked = false;
    if (SettingButton.IsInside2(e, COMMON_BUTTON)) {
        switch (e->type) {
        case SDL_MOUSEMOTION:
            SettingButton.currentSprite = BUTTON_MOUSE_OVER;
            break;
        case SDL_MOUSEBUTTONDOWN:
            SettingButton.currentSprite = BUTTON_MOUSE_OVER;
            Mix_PlayChannel(-1, gClick, 0); // Phát âm thanh khi nhấn
            clicked = true; // Báo rằng nút đã được nhấn
            break;
        }
    } else {
        SettingButton.currentSprite = BUTTON_MOUSE_OUT;
    }
    return clicked;
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
                Game_State = false; // Tạm dừng game
                break;
            }
        } else {
            PauseButton.currentSprite = BUTTON_MOUSE_OUT;
        }
    }
    // Xử lý nút Continue khi game đang tạm dừng
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
                //Game_State = false; // Tạm dừng game
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
                //Game_State = false; // Tạm dừng game
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
            Mix_PlayChannel(-1, gClick, 0); // Phát âm thanh khi nhấn
            clicked = true; // Báo rằng nút đã được nhấn
            break;
        }
    } else {
        AgainButton.currentSprite = BUTTON_MOUSE_OUT;
    }
    return clicked; // Trả về true nếu nút được nhấn xuống
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
            Mix_PlayChannel(-1, gClick, 0); // Phát âm thanh khi nhấn
            clicked = true; // Báo rằng nút đã được nhấn
            break;
        }
    } else {
        QuitButton.currentSprite = BUTTON_MOUSE_OUT;
    }
    return clicked; // Trả về true nếu nút được nhấn xuống
}

void DrawPlayerScore(LTexture gTextTexture,
                     LTexture gScoreTexture,
                     SDL_Color textColor,
                     SDL_Renderer *gRenderer,
                     TTF_Font *gFont,
                     const int& score) {
    //gTextTexture.Render(TEXT_1_POSX, TEXT_1_POSY, gRenderer);
    if (gScoreTexture.LoadFromRenderedText(std::to_string(score), gFont, textColor, gRenderer)) {
        gScoreTexture.Render(SCORE_POSX, SCORE_POSY, gRenderer);
    }
}

void DrawBullet(LTexture gTextTexture,
                     LTexture gScoreTexture,
                     SDL_Color textColor,
                     SDL_Renderer *gRenderer,
                     TTF_Font *gFont,
                     const int& score) {
    gTextTexture.Render(BULLET_POSX, BULLET_POSY, gRenderer);
    if (gScoreTexture.LoadFromRenderedText(std::to_string(score), gFont, textColor, gRenderer)) {
        gScoreTexture.Render(BULLET_COUNT_POSX, BULLET_COUNT_POSY, gRenderer);
    }
}

void DrawPlayerHighScore(LTexture gTextTexture,
                         LTexture gHighScoreTexture,
                         SDL_Color textColor,
                         SDL_Renderer* gRenderer,
                         TTF_Font* gFont,
                         int highscore) {
    //gTextTexture.Render(TEXT_2_POSX, TEXT_2_POSY, gRenderer);
    std::string HighScore = std::to_string(highscore);
    HighScore = "/" + HighScore;
    if (gHighScoreTexture.LoadFromRenderedText(HighScore, gFont, textColor, gRenderer)) {
        gHighScoreTexture.Render(HIGH_SCORE_POSX, HIGH_SCORE_POSY, gRenderer);
    }
}

bool isMouseInRect(SDL_Event *e, SDL_Rect rect) {

    int mouseX = e->motion.x;
    int mouseY = e->motion.y;

    if (mouseX >= rect.x &&
        mouseX <= rect.x + rect.w &&
        mouseY >= rect.y &&
        mouseY <= rect.y + rect.h) {
        return 1;
    }
    return 0;
}
