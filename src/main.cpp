//#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
//#include <cmath>
//#include <cstdio>
#include "Game_Base.h"
#include "Game_Utils.h"
#include "LTexture.h"
#include "Elements.h"

using namespace std;

void resetGame(GameObject& player, vector<GameObject>& obstacles, vector<GameObject>& spikes, vector<GameObject>& powerUps,
               bool& running, bool& isJumping, float& obstacleSpeed, int& currentGroundLevel) {
    obstacleSpeed = beginningObstacleSpeed;
    obstacles.clear();
    spikes.clear();
    powerUps.clear();
    running = true;
    isJumping = false;
    currentGroundLevel = GROUND_LEVEL_LOW;

    GameObject initialPlatform;
    initialPlatform.rect = {0, GROUND_LEVEL_LOW, 1500, 10};
    initialPlatform.collisionRect = {0, GROUND_LEVEL_LOW, 1500, 10};
    //initialPlatform.color = COLOR_RED;
    initialPlatform.x_velocity = -obstacleSpeed;
    initialPlatform.y_velocity = 0;
    obstacles.push_back(initialPlatform);

    player.rect = {150 + RENDER_OFFSET_Y, currentGroundLevel - 100 + RENDER_OFFSET_Y, PLAYER_SIZE, PLAYER_SIZE};
    player.collisionRect = {158, currentGroundLevel - 100, PLAYER_COLLISION_SIZE - 15, PLAYER_COLLISION_SIZE};
    player.x_velocity = 0;
    player.y_velocity = 0;
    REMAINING_HEARTS = 3;
    bulletCount = 0;
}

// Main Function
int main(int argc, char* argv[]) {
    GameObject player;
    //player.color = COLOR_RED;
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
    OkButton.SetInteract(29, 23, 120, 40);
    CloseButton.SetInteract(15, 15, 30, 30);
    AgainButton.SetInteract(30, 25, 120, 40);
    SettingButton.SetInteract(15, 15, 30, 30);
    MenuButton.SetInteract(15, 15, 30, 30);
    PauseButton.SetInteract(22, 18, 15, 25);
    ContinueButton.SetInteract(20, 19, 23, 22);
    NextPageButton.SetInteract(20, 19, 23, 22);
    BackPageButton.SetInteract(20, 19, 23, 22);
    ExitHelpButton.SetInteract(15, 15, 30, 30);
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
    bool Quit_Game = false;
    bool Play_Again = false;
    bool Quit_skinSelector = true;
    bool Quit_Menu = false;
    bool Quit_Instruction = true;
    Mix_PlayMusic(gMenuMusic, IS_REPEATITIVE);

    int lockedSkin = 0;

    int currentX = (SLIDER_START_X + SLIDER_END_X) / 2;
    bool isDragging = false;
    //Game Loop
    while(!Quit_Game){
    // Menu Loop
    while (!Quit_Menu) {
        int volume = ((currentX - SLIDER_START_X) * MIX_MAX_VOLUME) / (SLIDER_END_X - SLIDER_START_X);
        Mix_VolumeMusic(volume);
        Mix_Volume(MIX_CHANNEL, volume);

        SDL_Event e_mouse;
        while (SDL_PollEvent(&e_mouse) != 0) {
            if (e_mouse.type == SDL_QUIT) Quit_Game = true;
            bool Quit_Game = false;
            HandlePlayButton(&e_mouse, PlayButton, Quit_Menu, Play_Again, gClick);
            HandleCloseButton(&e_mouse, CloseButton, Quit_Game, gClick);
            if(HandleHelpButton(&e_mouse, HelpButton, gRenderer, Quit_Game, gClick)) {
                //cout << "TRUE" << endl;
                Quit_Menu = true;
                Quit_Instruction = false;
                Play_Again = true;
            }
            if (HandleSettingButton(&e_mouse, SettingButton, gClick)) {
                Quit_Menu = true;
                Quit_skinSelector = false;
                Play_Again = true;
            }
            HandleMenuButton(&e_mouse, MenuButton, gClick);
            HandleSoundButton(&e_mouse, currentX, SoundOnButton, SoundOffButton, gClick);
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

        gMenuTexture.Render(0, 0, gRenderer);
        gTransTexture.Render(0, 0, gRenderer);

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastFrameTime >= frameDelay) {
            currentFrame = (currentFrame + 1) % 3;
            lastFrameTime = currentTime;
        }
        gLabtubeTexture.Render(0, 0, gRenderer, &gLabtube[currentFrame]);
        gVineTexture.Render(0, 0, gRenderer);
        gNameTexture.Render(0, 0, gRenderer);
        if (volume) {
            SDL_Rect* currentClip_SoundOn = &gSoundOnButton[SoundOnButton.currentSprite];
            SoundOnButton.Render(currentClip_SoundOn, gRenderer, gSoundOnButtonTexture);
        } else {
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

        SDL_Rect sliderRect = {SLIDER_START_X, SLIDER_Y - 5, SLIDER_END_X - SLIDER_START_X, 13};
        SDL_RenderCopy(gRenderer, sliderTexture, NULL, &sliderRect);

        SDL_Rect knobRect = {currentX - KNOB_SIZE / 2, SLIDER_Y - KNOB_SIZE / 2 + 2, KNOB_SIZE, KNOB_SIZE};
        SDL_RenderCopy(gRenderer, knobTexture, NULL, &knobRect);
        //SDL_Rect test = {0,0,600,600};
        //gExitButtonTexture.Render(0,0,gRenderer,&test);
        SDL_RenderPresent(gRenderer);
    }

    //Instruction
    int pageNumber = 0;
    while(!Quit_Instruction) {
        SDL_Event e_mouse;
        while (SDL_PollEvent(&e_mouse) != 0) {
            HandleNextPageButton(&e_mouse, NextPageButton, pageNumber, gClick);
            HandleBackButton(&e_mouse, BackPageButton, pageNumber, gClick);
            HandleExitHelpButton(&e_mouse, ExitHelpButton, Quit_Menu, Quit_Instruction, Play_Again, gClick);
        }

        gHelpTexture[pageNumber].Render(0, 0, gRenderer);

        SDL_Rect* currentClip_NextPage = &gNextPageButton[NextPageButton.currentSprite];
        NextPageButton.Render(currentClip_NextPage, gRenderer, gNextPageButtonTexture);

        SDL_Rect* currentClip_BackPage = &gBackPageButton[BackPageButton.currentSprite];
        BackPageButton.Render(currentClip_BackPage, gRenderer, gBackPageButtonTexture);

        SDL_Rect* currentClip_ExitHelp = &gExitHelpButton[ExitHelpButton.currentSprite];
        ExitHelpButton.Render(currentClip_ExitHelp, gRenderer, gExitHelpButtonTexture);

        SDL_RenderPresent(gRenderer);
    }

    // Skin Selector
    SDL_Rect skin1 = {60, 0, 125, 360};
    SDL_Rect skin2 = {244, 0, 125, 360};
    SDL_Rect skin3 = {429, 0, 125, 360};
    SDL_Rect skin4 = {616, 0, 125, 360};
    int skinNumber = 0;
    while (!Quit_skinSelector) {
        SDL_Event e_mouse;
        while (SDL_PollEvent(&e_mouse) != 0) {
            if (isMouseInRect(&e_mouse, skin1)) {
                skinNumber = 1;
                if (e_mouse.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, gClick, 0);
                    lockedSkin = 1;
                }
            } else if (isMouseInRect(&e_mouse, skin2)) {
                skinNumber = 2;
                if (e_mouse.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, gClick, 0);
                    lockedSkin = 2;
                }
            } else if (isMouseInRect(&e_mouse, skin3)) {
                skinNumber = 3;
                if (e_mouse.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, gClick, 0);
                    lockedSkin = 3;
                }
            } else if (isMouseInRect(&e_mouse, skin4)) {
                skinNumber = 4;
                if (e_mouse.type == SDL_MOUSEBUTTONDOWN) {
                    Mix_PlayChannel(-1, gClick, 0);
                    lockedSkin = 4;
                }
            } else {
                skinNumber = 0;
            }
            HandlePlayButton(&e_mouse, OkButton, Quit_skinSelector, Play_Again, gClick);
            HandleExitHelpButton(&e_mouse, ExitHelpButton, Quit_Menu, Quit_skinSelector, Play_Again, gClick);
        }

        if (!lockedSkin)
            gSkinSelectorTexture[skinNumber].Render(0, 0, gRenderer);
        else
            gSkinSelectorTexture[lockedSkin].Render(0, 0, gRenderer);
        SDL_Rect* currentClip_ExitHelp = &gExitHelpButton[ExitHelpButton.currentSprite];
        ExitHelpButton.Render(currentClip_ExitHelp, gRenderer, gExitHelpButtonTexture);
        SDL_Rect* currentClip_Ok = &gOkButton[OkButton.currentSprite];
        OkButton.Render(currentClip_Ok, gRenderer, gOkButtonTexture);
        SDL_RenderPresent(gRenderer);
    }

    // Game Loop
    while (Play_Again) {
        srand(time(NULL));
        int frame_Character = 0;
        int score = 0;
        int time = 0;
        int highscore = GetHighScoreFromFile("high_score.txt"); // Lấy highscore dạng int

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
                            Mix_PlayChannel(MIX_CHANNEL, gShoot, NOT_REPEATITIVE);
                            Bullet bullet;
                            bullet.rect = {player.rect.x + 20, player.rect.y + (player.rect.h - BULLET_SIZE) / 2 - 10, BULLET_SIZE, BULLET_SIZE};
                            bullet.collisionRect = {player.rect.x + 20, player.rect.y + (player.rect.h - BULLET_SIZE) / 2 - 10, BULLET_SIZE, BULLET_SIZE};
                            bullet.x_velocity = BULLET_SPEED;
                            bullet.y_velocity = 0;
                            bullet.startX = bullet.rect.x;
                            bullets.push_back(bullet);
                            bulletCount--;
                            //cout << "Shot bullet" << bulletCount << " bullets." << endl;
                        }
                    }
                }
            }

            if (Game_State) {
                UpdateGameTimeAndScore(time, score);
                //cout << score << endl;
                int obstacleWidth = 3000 + (rand() % 500);
                currentGroundLevel = (currentGroundLevel == GROUND_LEVEL_LOW) ? GROUND_LEVEL_HIGH : GROUND_LEVEL_LOW;
                int newPlatformX = lastPlatformX + (rand() % MAX_PLATFORM_GAP) + 200;

                GameObject obstacle;
                obstacle.rect = {newPlatformX, currentGroundLevel, obstacleWidth, 10};
                obstacle.collisionRect = {newPlatformX, currentGroundLevel, obstacleWidth, 5};
                //obstacle.color = COLOR_RED;
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
                        if (spike.type == 1) {
                            int randomRobotHeight = rand() % 100;
                            spike.rect = {newPlatformX + i * spikeSpacing - ROBOT_SIZE / 2 - 10, currentGroundLevel - ROBOT_SIZE - randomRobotHeight, ROBOT_SIZE, ROBOT_SIZE};
                            spike.collisionRect = {newPlatformX + i * spikeSpacing - ROBOT_SIZE / 2, currentGroundLevel - ROBOT_SIZE + 20 - randomRobotHeight, ROBOT_COLLISION_SIZE, ROBOT_COLLISION_SIZE};
                        } else {
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
                    powerUp.rect = {newPlatformX + (rand() % obstacleWidth) - POWERUP_SIZE / 2, currentGroundLevel - POWERUP_SIZE + 14, POWERUP_SIZE, POWERUP_SIZE};
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
                        //cout << "Hit spike" << endl;
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
                            Mix_PlayChannel(MIX_CHANNEL, gBuff, NOT_REPEATITIVE);
                            bulletCount += MAX_BULLETS;
                            //cout << "Picked up power-up" << bulletCount << " bullets" << endl;
                            it = powerUps.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }

                if (!isOnGround && player.collisionRect.y > SCREEN_HEIGHT) {
                    //cout << "Fell off platform" << endl;
                    REMAINING_HEARTS = 0;
                    Mix_PauseMusic();
                    Mix_PlayChannel(MIX_CHANNEL, gLose, NOT_REPEATITIVE);
                    UpdateHighScore("high_score.txt", score, highscore);
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
                    UpdateHighScore("high_score.txt", score, highscore);
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
                //gBackgroundCoatingTexture.Render(0, 0, gRenderer);
            } else {
                for (int i = 0; i < BACKGROUND_LAYER; ++i) {
                    gBackgroundTexture[i].Render(OffsetSpeed_Bkgr[i], 0, gRenderer);
                    gBackgroundTexture[i].Render(OffsetSpeed_Bkgr[i] + gBackgroundTexture[i].GetWidth(), 0, gRenderer);
                }
                //gBackgroundCoatingTexture.Render(0, 0, gRenderer);
            }
            gBackgroundCoatingTexture.Render(0, 0, gRenderer);
            for (auto& obstacle : obstacles) {
                SDL_RenderCopy(gRenderer, groundTexture, NULL, &obstacle.rect);
            }

            for (const auto& spike : spikes) {
                if (spike.type == 0) {
                    SDL_RenderCopy(gRenderer, spikeTexture, nullptr, &spike.rect);
                } else {
                    SDL_RenderCopy(gRenderer, robotTexture, nullptr, &spike.rect);
                }
                //check collision area
                //SDL_RenderDrawRect(gRenderer, &spike.collisionRect);
            }

            for (const auto& bullet : bullets) {
                gBulletTexture.Render(bullet.rect.x, bullet.rect.y, gRenderer);
                //check collision area
                //SDL_RenderDrawRect(gRenderer, &bullet.collisionRect);
            }

            for (const auto& powerUp : powerUps) {
                SDL_RenderCopy(gRenderer, powerUpTexture, nullptr, &powerUp.rect);
                //check collision area
                //SDL_RenderDrawRect(gRenderer, &powerUp.collisionRect);
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
            SDL_RenderCopy(gRenderer, gCharacterTexture[lockedSkin].mTexture, currentClip_Character, &player.rect);
            //SDL_RenderDrawRect(gRenderer, &player.collisionRect);

            DrawBullet(gBulletTexture, gRemainBulletTexture, textColor, gRenderer, gFont, bulletCount);
            DrawPlayerScore(gText1Texture, gScoreTexture, textColor,gRenderer, gFont, score);
            DrawPlayerHighScore(gText2Texture, gHighScoreTexture, textColor, gRenderer, gFont, highscore);

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
                ExitButton.Render(currentClip_Exit, gRenderer, gQuitButtonTexture);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    }
    Close();
    return 0;
}

// Initialization and Cleanup
bool Init() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        LogError("Can not initialize SDL.", SDL_ERROR);
        success = false;
    } else {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "Warning: Linear texture filtering not enabled!" << endl;
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

    gShoot = Mix_LoadWAV("sound/shooting_sound.mp3");
    if (!gShoot) {
        LogError("Failed to load shooting sound", MIX_ERROR);
        success = false;
    }

    gBuff = Mix_LoadWAV("sound/buff_sound.mp3");
    if (!gBuff) {
        LogError("Failed to load buff pickup sound", MIX_ERROR);
        success = false;
    }

    gFont = TTF_OpenFont("font/pixel_font.ttf", 28);
    if (!gFont) {
        LogError("Failed to load font", MIX_ERROR);
        success = false;
    } else {
        if (!gMenuTexture.LoadFromFile("images/menu/bgg.png", gRenderer)) success = false;
        if (!gPlayButtonTexture.LoadFromFile("images/button/play.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gPlayButton[i].x = 175 * i;
                gPlayButton[i].y = 0;
                gPlayButton[i].w = 175;
                gPlayButton[i].h = 90;
            }
        }
        if (!gHelpButtonTexture.LoadFromFile("images/button/help.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gHelpButton[i].x = 175 * i;
                gHelpButton[i].y = 0;
                gHelpButton[i].w = 175;
                gHelpButton[i].h = 90;
            }
        }
        if (!gQuitButtonTexture.LoadFromFile("images/button/quit.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gExitButton[i].x = 175 * i;
                gExitButton[i].y = 0;
                gExitButton[i].w = 175;
                gExitButton[i].h = 90;
            }
        }
        if (!gAgainButtonTexture.LoadFromFile("images/button/again.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gAgainButton[i].x = 175 * i;
                gAgainButton[i].y = 0;
                gAgainButton[i].w = 175;
                gAgainButton[i].h = 90;
            }
        }
        if (!gOkButtonTexture.LoadFromFile("images/button/ok_button.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gOkButton[i].x = 175 * i;
                gOkButton[i].y = 0;
                gOkButton[i].w = 175;
                gOkButton[i].h = 90;
            }
        }
        if (!gNameTexture.LoadFromFile("images/menu/name.png", gRenderer)) success = false;
        if (!gVineTexture.LoadFromFile("images/menu/leo.png", gRenderer)) success = false;
        if (!gTransTexture.LoadFromFile("images/menu/trans.png", gRenderer)) success = false;
        if (!gMenuButtonTexture.LoadFromFile("images/button/menu.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gMenuButton[i].x = 60 * i;
                gMenuButton[i].y = 0;
                gMenuButton[i].w = 60;
                gMenuButton[i].h = 60;
            }
        }
        if (!gCloseButtonTexture.LoadFromFile("images/button/close.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gCloseButton[i].x = 60 * i;
                gCloseButton[i].y = 0;
                gCloseButton[i].w = 60;
                gCloseButton[i].h = 60;
            }
        }
        if (!gExitHelpButtonTexture.LoadFromFile("images/button/close.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gExitHelpButton[i].x = 60 * i;
                gExitHelpButton[i].y = 0;
                gExitHelpButton[i].w = 60;
                gExitHelpButton[i].h = 60;
            }
        }
        if (!gSettingButtonTexture.LoadFromFile("images/button/setting.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gSettingButton[i].x = 60 * i;
                gSettingButton[i].y = 0;
                gSettingButton[i].w = 60;
                gSettingButton[i].h = 60;
            }
        }
        if (!gSoundOnButtonTexture.LoadFromFile("images/button/sound_on.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gSoundOnButton[i].x = 60 * i;
                gSoundOnButton[i].y = 0;
                gSoundOnButton[i].w = 60;
                gSoundOnButton[i].h = 60;
            }
        }
        if (!gLabtubeTexture.LoadFromFile("images/menu/labtube.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < 3; ++i) {
                gLabtube[i].x = 800 * i;
                gLabtube[i].y = 0;
                gLabtube[i].w = 800;
                gLabtube[i].h = 500;
            }
        }
        if (!gHeartTexture.LoadFromFile("images/elements/heart.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < 4; ++i) {
                gHeart[i].x = 140 * i;
                gHeart[i].y = 0;
                gHeart[i].w = 140;
                gHeart[i].h = 60;
            }
        }
        if (!gPauseButtonTexture.LoadFromFile("images/button/pause.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gPauseButton[i].x = 60 * i;
                gPauseButton[i].y = 0;
                gPauseButton[i].w = 60;
                gPauseButton[i].h = 60;
            }
        }
        if (!gContinueButtonTexture.LoadFromFile("images/button/continue.png", gRenderer)) success = false;
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

        if (!gCharacterTexture[0].LoadFromFile("images/elements/dino_skin/default_dino.png", gRenderer) ||
                !gCharacterTexture[1].LoadFromFile("images/elements/dino_skin/default_dino.png", gRenderer) ||
                !gCharacterTexture[2].LoadFromFile("images/elements/dino_skin/pink_dino.png", gRenderer) ||
                !gCharacterTexture[3].LoadFromFile("images/elements/dino_skin/blue_dino.png", gRenderer) ||
                !gCharacterTexture[4].LoadFromFile("images/elements/dino_skin/arcane_dino.png", gRenderer))
            success = false;
        else {
            for (int i = 0; i < 4; ++i) {
                gCharacterClips[i].x = 258 * i;
                gCharacterClips[i].y = 0;
                gCharacterClips[i].w = 258;
                gCharacterClips[i].h = 281;
            }
        }

        if (!gBackPageButtonTexture.LoadFromFile("images/back.png", gRenderer)) success = false;
        else {
            gBackPageButton[0].x = 60;
            gBackPageButton[0].y = 0;
            gBackPageButton[0].w = 60;
            gBackPageButton[0].h = 60;

            gBackPageButton[1].x = 0;
            gBackPageButton[1].y = 0;
            gBackPageButton[1].w = 60;
            gBackPageButton[1].h = 60;
        }
        if (!gNextPageButtonTexture.LoadFromFile("images/button/continue.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gNextPageButton[i].x = 60 * i;
                gNextPageButton[i].y = 0;
                gNextPageButton[i].w = 60;
                gNextPageButton[i].h = 60;
            }
        }

        SDL_Surface* groundSurface = IMG_Load("images/elements/ground.png");
        if (!groundSurface) success = false;
        else {
            groundTexture = SDL_CreateTextureFromSurface(gRenderer, groundSurface);
            SDL_FreeSurface(groundSurface);
            if (!groundTexture) success = false;
        }

        SDL_Surface* spikeSurface = IMG_Load("images/elements/poison.png");
        if (!spikeSurface) success = false;
        else {
            spikeTexture = SDL_CreateTextureFromSurface(gRenderer, spikeSurface);
            SDL_FreeSurface(spikeSurface);
            if (!spikeTexture) success = false;
        }

        SDL_Surface* robotSurface = IMG_Load("images/elements/robot.png");
        if (!robotSurface) success = false;
        else {
            robotTexture = SDL_CreateTextureFromSurface(gRenderer, robotSurface);
            SDL_FreeSurface(robotSurface);
            if (!robotTexture) success = false;
        }

        if (!gBulletTexture.LoadFromFile("images/elements/bullet_skin/green_dino_bullet.png", gRenderer)) success = false;
        /*
        if (!gBulletTexture.LoadFromFile("images/elements/bullet_skin/bullet_green.png", gRenderer)) success = false;
        if (!gBulletTexture.LoadFromFile("images/elements/bullet_skin/bullet_green.png", gRenderer)) success = false;
        if (!gBulletTexture.LoadFromFile("images/elements/bullet_skin/bullet_green.png", gRenderer)) success = false;
        */

        SDL_Surface* powerUpSurface = IMG_Load("images/elements/buff.png");
        if (!powerUpSurface) success = false;
        else {
            powerUpTexture = SDL_CreateTextureFromSurface(gRenderer, powerUpSurface);
            SDL_FreeSurface(powerUpSurface);
            if (!powerUpTexture) success = false;
        }

        if (!gLoseTexture.LoadFromFile("images/elements/endgame.png", gRenderer)) success = false;

        SDL_Surface* sliderSurface = IMG_Load("images/button/slider.png");
        if (!sliderSurface) success = false;
        else {
            sliderTexture = SDL_CreateTextureFromSurface(gRenderer, sliderSurface);
            SDL_FreeSurface(sliderSurface);
            if (!sliderTexture) success = false;
        }

        SDL_Surface* knobSurface = IMG_Load("images/button/knob.png");
        if (!knobSurface) success = false;
        else {
            knobTexture = SDL_CreateTextureFromSurface(gRenderer, knobSurface);
            SDL_FreeSurface(knobSurface);
            if (!knobTexture) success = false;
        }

        if (!gSoundOffButtonTexture.LoadFromFile("images/button/sound_off.png", gRenderer)) success = false;
        else {
            for (int i = 0; i < BUTTON_TOTAL; ++i) {
                gSoundOffButton[i].x = 60 * i;
                gSoundOffButton[i].y = 0;
                gSoundOffButton[i].w = 60;
                gSoundOffButton[i].h = 60;
            }
        }

        if (!gSkinSelectorTexture[0].LoadFromFile("images/skin_selector/all_closed.png", gRenderer)) success = false;
        if (!gSkinSelectorTexture[1].LoadFromFile("images/skin_selector/1_open.png", gRenderer)) success = false;
        if (!gSkinSelectorTexture[2].LoadFromFile("images/skin_selector/2_open.png", gRenderer)) success = false;
        if (!gSkinSelectorTexture[3].LoadFromFile("images/skin_selector/3_open.png", gRenderer)) success = false;
        if (!gSkinSelectorTexture[4].LoadFromFile("images/skin_selector/4_open.png", gRenderer)) success = false;

        if (!gBackgroundCoatingTexture.LoadFromFile("images/elements/background/trans.png", gRenderer)) success = false;


        if (!gHelpTexture[0].LoadFromFile("images/menu/help/1.png", gRenderer) ||
                !gHelpTexture[1].LoadFromFile("images/menu/help/2.png", gRenderer) ||
                !gHelpTexture[2].LoadFromFile("images/menu/help/3.png", gRenderer) ||
                !gHelpTexture[3].LoadFromFile("images/menu/help/4.png", gRenderer))
            success = false;

        return success;
    }
}
void Close() {
    gMenuTexture.Free();
    for (int i = 0; i < 5; i++) gCharacterTexture[i].Free();
    gPlayButtonTexture.Free();
    gHelpButtonTexture.Free();
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
    Mix_FreeChunk(gShoot);
    Mix_FreeChunk(gBuff);
    gMusic = nullptr;
    gMenuMusic = nullptr;
    gClick = nullptr;
    gLose = nullptr;
    gJump = nullptr;
    gShoot = nullptr;
    gBuff = nullptr;

    SDL_DestroyTexture(spikeTexture);
    SDL_DestroyTexture(robotTexture);
    SDL_DestroyTexture(groundTexture);
    SDL_DestroyTexture(powerUpTexture);
    SDL_DestroyTexture(gameStateTexture);
    SDL_DestroyTexture(sliderTexture);
    SDL_DestroyTexture(knobTexture);
    gameStateTexture = nullptr;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = nullptr;
    gWindow = nullptr;

    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}
