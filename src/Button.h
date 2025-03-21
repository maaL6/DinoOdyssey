#ifndef BUTTON_H_
#define BUTTON_H_

#include "Game_Base.h"
#include "LTexture.h"

class Button
{
public:
	ButtonSprite currentSprite;

	Button();

	Button(int x, int y);

	void SetInteract(int x, int y, int w, int h);

	void SetPosition(int x, int y);

	int GetX();

	int GetY();

	bool IsInside(SDL_Event *e, int size);

	bool IsInside2(SDL_Event *e, int size);

	void Render(SDL_Rect* currentClip, SDL_Renderer* gRenderer, LTexture gButtonTexture);

private:
	SDL_Point position;
	SDL_Rect interact;
};

#endif // !BUTTON_H_
