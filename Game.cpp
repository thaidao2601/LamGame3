#include"Game.h"
#include<iostream>
#include<algorithm>
#include<string>

Game::Game(bool twoPlayer):player1(((MAP_WIDTH-1)/2+1)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE),
                           player2(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE)
{
    running=true;
    isPaused=false;
    twoPlayerMode=twoPlayer;
    running=true;
    enemyCount=EnemiesNum;
    player1Lives=playerLives;
    player2Lives=twoPlayerMode?playerLives:0;
    gameOver=false;
    victory=false;
    returnToMenu=false;
    Initialize();
    pauseButton={SCREEN_WIDTH-TILE_SIZE-30,0,30,30};
    generateWalls();
    spawnEnemies();

}

Game::~Game()
{
    if(brickTexture)
    {
        SDL_DestroyTexture(brickTexture);
    }
    if(stoneTexture)
    {
        SDL_DestroyTexture(stoneTexture);
    }
    if(yellowTankTexture)
    {
        SDL_DestroyTexture(yellowTankTexture);
    }
    if(greenTankTexture)
    {
        SDL_DestroyTexture(greenTankTexture);
    }
    if(redTankTexture)
    {
        SDL_DestroyTexture(redTankTexture);
    }
    if(explosionTexture)
    {
        SDL_DestroyTexture(explosionTexture);
    }
    if(backgroundMusic)
    {
        Mix_FreeMusic(backgroundMusic);
    }
    if(bulletTexture)
    {
        SDL_DestroyTexture(bulletTexture);
    }
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::run()
{
    if(backgroundMusic)
    {
        if(Mix_PlayMusic(backgroundMusic,-1)==-1)
        {
            std::cerr<<"Failed to play background music! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
        }
    }
    while(running)
    {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}
