#ifndef MENU_H
#define MENU_H

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>

#include"Constants.h"

class Menu
{
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *backgroundTexture;
    bool running;
    bool startGame;
    int playerNum;
    SDL_Rect onePlayerButton;
    SDL_Rect twoPlayerButton;
    SDL_Rect exitButton;
    Mix_Music *menuMusic;

    Menu();
    void renderText(const char *text,SDL_Rect rect,SDL_Color color);
    void render();
    void handleEvents();
    void run();
    ~Menu();
};

#endif
