#include"Game.h"
#include<iostream>

void Game::Initialize()
{
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        std::cerr<<"SDL could not initialize! SDL_Error: "<<SDL_GetError()<<std::endl;
        running=false;
    }
    if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0)
    {
        std::cerr<<"SDL_mixer could not initialize! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
        running=false;
    }
    backgroundMusic=Mix_LoadMUS("media/backgroundmusic.mp3");
    if(!backgroundMusic)
    {
        std::cerr<<"Failed to load background music! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
    }
    window=SDL_CreateWindow("Battle City",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if(!window)
    {
        std::cerr<<"Window could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
        running=false;
    }
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        std::cerr<<"Renderer could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
        running=false;
    }
    brickTexture=IMG_LoadTexture(renderer,"media/brick.jpg");
    if(!brickTexture)
    {
        std::cerr<<"Failed to load brick texture! IMG_Error: "<<IMG_GetError()<<std::endl;
    }
    stoneTexture=IMG_LoadTexture(renderer,"media/stone.jpg");
    if(!stoneTexture)
    {
        std::cerr<<"Failed to load stone texture! IMG_Error: "<<IMG_GetError()<<std::endl;
    }
    yellowTankTexture=IMG_LoadTexture(renderer,"media/yellow_tank.png");
    if(!yellowTankTexture)
    {
        std::cerr<<"Failed to load yellow tank texture! IMG_Error: "<<IMG_GetError()<<std::endl;
    }
    greenTankTexture=IMG_LoadTexture(renderer,"media/green_tank.png");
    if(!greenTankTexture)
    {
        std::cerr<<"Failed to load green tank texture! IMG_Error: "<<IMG_GetError()<<std::endl;
    }
    redTankTexture=IMG_LoadTexture(renderer,"media/red_tank.png");
    if(!redTankTexture)
    {
        std::cerr<<"Failed to load red tank texture! IMG_Error: "<<IMG_GetError()<<std::endl;
    }
    explosionTexture=IMG_LoadTexture(renderer,"media/explosion.jpg");
    if(!explosionTexture)
    {
        std::cerr<<"Failed to load explosion texture! IMG_Error: "<<IMG_GetError()<<std::endl;
    }
    bulletTexture=IMG_LoadTexture(renderer,"media/bullet.png");
    if(!bulletTexture)
    {
        std::cerr<<"Failed to load bullet texture! IMG_Error: "<<IMG_GetError()<<std::endl;
    }
}
