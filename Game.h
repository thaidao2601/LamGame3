#ifndef GAME_H
#define GAME_H

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<vector>

#include"Constants.h"
#include"Wall.h"
#include"PlayerTank.h"
#include"EnemyTank.h"
#include"Explosion.h"

class Game
{
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *brickTexture=nullptr;
    SDL_Texture *stoneTexture=nullptr;
    SDL_Texture *yellowTankTexture=nullptr;
    SDL_Texture *greenTankTexture=nullptr;
    SDL_Texture *redTankTexture=nullptr;
    bool running;
    bool isPaused;
    SDL_Rect pauseButton;
    SDL_Rect continueButton;
    SDL_Rect exitGameButton;
    std::vector<Wall>walls;
    PlayerTank player1;
    PlayerTank player2;
    bool twoPlayerMode=false;
    std::vector<EnemyTank>enemies;
    int enemyCount;
    int player1Lives;
    int player2Lives;
    bool gameOver;
    bool victory;
    SDL_Rect exitButton;
    bool returnToMenu;
    SDL_Texture *explosionTexture=nullptr;
    std::vector<Explosion>explosions;
    Mix_Music *backgroundMusic;
    SDL_Texture *bulletTexture=nullptr;

    Game(bool twoPlayer);
    ~Game();
    void Initialize();
    void generateWalls();
    void spawnEnemies();
    PlayerTank spawnPlayer1();
    PlayerTank spawnPlayer2();
    void update();
    void handleEvents();
    void renderGameStatus();
    void renderGameResult();
    void renderPauseMenu();
    void renderText(const char *text,SDL_Rect rect,SDL_Color color);
    void render();
    void run();
};

#endif
