#include"Game.h"
#include<string>
#include<iostream>

void Game::renderGameStatus()
{
    TTF_Font *font=TTF_OpenFont("media/consola.ttf",20);
    if (!font)
    {
        std::cerr<<"Failed to load font!"<<std::endl;
        return;
    }
    SDL_Color textColor={255,255,255,255};

    std::string enemyText="Địch còn lại: "+std::to_string(enemyCount);
    SDL_Surface* enemySurface=TTF_RenderUTF8_Solid(font,enemyText.c_str(),textColor);
    SDL_Texture* enemyTexture=SDL_CreateTextureFromSurface(renderer,enemySurface);
    SDL_Rect enemyRect={10,10,enemySurface->w,enemySurface->h};
    SDL_RenderCopy(renderer,enemyTexture,NULL,&enemyRect);
    SDL_FreeSurface(enemySurface);
    SDL_DestroyTexture(enemyTexture);

    std::string player1Text="Xe tăng 1: "+std::to_string(player1Lives)+" mạng";
    SDL_Surface* player1Surface=TTF_RenderUTF8_Solid(font,player1Text.c_str(),textColor);
    SDL_Texture* player1Texture=SDL_CreateTextureFromSurface(renderer, player1Surface);
    SDL_Rect player1Rect={10,40,player1Surface->w,player1Surface->h};
    SDL_RenderCopy(renderer,player1Texture,NULL,&player1Rect);
    SDL_FreeSurface(player1Surface);
    SDL_DestroyTexture(player1Texture);

    if(twoPlayerMode)
    {
        std::string player2Text="Xe tăng 2: "+std::to_string(player2Lives)+" mạng";
        SDL_Surface *player2Surface=TTF_RenderUTF8_Solid(font,player2Text.c_str(),textColor);
        SDL_Texture *player2Texture=SDL_CreateTextureFromSurface(renderer,player2Surface);
        SDL_Rect player2Rect={10,70,player2Surface->w,player2Surface->h};
        SDL_RenderCopy(renderer,player2Texture,NULL,&player2Rect);
        SDL_FreeSurface(player2Surface);
        SDL_DestroyTexture(player2Texture);
    }

    TTF_CloseFont(font);
}

void Game::renderGameResult()
{
    TTF_Font *font=TTF_OpenFont("media/consola.ttf",48);
    if (!font)
    {
        std::cerr<<"Failed to load font!"<<std::endl;
        return;
    }

    SDL_Color textColor={255,255,255,255};
    std::string resultText=victory?"Chiến Thắng!":"Thua Cuộc!";

    SDL_Surface *resultSurface=TTF_RenderUTF8_Solid(font,resultText.c_str(),textColor);
    SDL_Texture *resultTexture=SDL_CreateTextureFromSurface(renderer,resultSurface);
    SDL_Rect resultRect={(SCREEN_WIDTH-resultSurface->w)/2,(SCREEN_HEIGHT-resultSurface->h)/2,resultSurface->w,resultSurface->h};
    SDL_SetRenderDrawColor(renderer,0,0,0,200);
    SDL_Rect overlay={0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer,&overlay);
    SDL_RenderCopy(renderer,resultTexture,NULL,&resultRect);
    SDL_FreeSurface(resultSurface);
    SDL_DestroyTexture(resultTexture);

    int buttonWidth=200;
    int buttonHeight=50;
    int startY=SCREEN_HEIGHT/2+50;

    exitButton={(SCREEN_WIDTH-buttonWidth)/2,startY,buttonWidth,buttonHeight};
    SDL_SetRenderDrawColor(renderer,100,100,100,255);
    SDL_RenderFillRect(renderer,&exitButton);
    SDL_Color buttonTextColor={255,255,255,255};
    renderText("Thoát",exitButton,buttonTextColor);

    TTF_CloseFont(font);
}

void Game::renderPauseMenu()
{
    SDL_SetRenderDrawColor(renderer,0,0,0,128);
    SDL_Rect overlay={0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer,&overlay);

    int buttonWidth=200;
    int buttonHeight=50;
    int startY=SCREEN_HEIGHT/2-buttonHeight-20;

    continueButton={(SCREEN_WIDTH-buttonWidth)/2,startY,buttonWidth,buttonHeight };
    exitGameButton={(SCREEN_WIDTH-buttonWidth)/2,startY+buttonHeight+20,buttonWidth,buttonHeight};

    SDL_SetRenderDrawColor(renderer,100,100,100,255);
    SDL_RenderFillRect(renderer,&continueButton);
    SDL_RenderFillRect(renderer,&exitGameButton);

    SDL_Color textColor={255,255,255,255};
    renderText("Tiếp tục",continueButton,textColor);
    renderText("Thoát",exitGameButton,textColor);
}

void Game::renderText(const char *text,SDL_Rect rect,SDL_Color color)
{
    TTF_Font *font=TTF_OpenFont("media/consola.ttf",24);
    if(!font)
    {
        std::cerr<<"Failed to load font! TTF_Error: "<<TTF_GetError()<<std::endl;
        return;
    }

    SDL_Surface *textSurface=TTF_RenderUTF8_Solid(font,text,color);
    if(!textSurface)
    {
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture *textTexture=SDL_CreateTextureFromSurface(renderer,textSurface);
    SDL_Rect textRect={rect.x+(rect.w-textSurface->w)/2,rect.y+(rect.h-textSurface->h)/2,textSurface->w,textSurface->h};
    SDL_RenderCopy(renderer,textTexture,NULL,&textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    //Draw the boundary using stone texture
    if(stoneTexture)
    {
        //Top boundary
        SDL_Rect topBoundary={0,0,SCREEN_WIDTH,TILE_SIZE};
        SDL_RenderCopy(renderer,stoneTexture,NULL,&topBoundary);

        //Bottom boundary
        SDL_Rect bottomBoundary={0,SCREEN_HEIGHT-TILE_SIZE,SCREEN_WIDTH,TILE_SIZE};
        SDL_RenderCopy(renderer,stoneTexture,NULL,&bottomBoundary);

        //Left boundary
        SDL_Rect leftBoundary={0,0,TILE_SIZE,SCREEN_HEIGHT};
        SDL_RenderCopy(renderer,stoneTexture,NULL,&leftBoundary);

        //Right boundary
        SDL_Rect rightBoundary={SCREEN_WIDTH-TILE_SIZE,0,TILE_SIZE,SCREEN_HEIGHT};
        SDL_RenderCopy(renderer,stoneTexture,NULL,&rightBoundary);
    }
    else
    {
        //Fallback to drawing colored rectangles if texture loading failed
        SDL_SetRenderDrawColor(renderer,128,128,128,255);

        //Top boundary
        SDL_Rect topBoundary={0,0,SCREEN_WIDTH,TILE_SIZE};
        SDL_RenderFillRect(renderer,&topBoundary);

        //Bottom boundary
        SDL_Rect bottomBoundary={0,SCREEN_HEIGHT-TILE_SIZE,SCREEN_WIDTH,TILE_SIZE};
        SDL_RenderFillRect(renderer,&bottomBoundary);

        //Left boundary
        SDL_Rect leftBoundary={0,0,TILE_SIZE,SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer,&leftBoundary);

        //Right boundary
        SDL_Rect rightBoundary={SCREEN_WIDTH-TILE_SIZE,0,TILE_SIZE,SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer,&rightBoundary);
    }

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderFillRect(renderer,&pauseButton);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderDrawLine(renderer,pauseButton.x+10,pauseButton.y+10,pauseButton.x+10,pauseButton.y+20);
    SDL_RenderDrawLine(renderer,pauseButton.x+20,pauseButton.y+10,pauseButton.x+20,pauseButton.y+20);

    //Draw walls with brick texture instead of solid color
    for(int i=0;i<int(walls.size());++i)
    {
        if(walls[i].active)
        {
            if(brickTexture)
            {
                SDL_RenderCopy(renderer,brickTexture,NULL,&walls[i].rect);
            }
            else
            {
                walls[i].render(renderer);
            }
        }
    }

    if(player1.active)
    {
        if(yellowTankTexture)
        {
            double angle=0;
            if(player1.dirX==0&&player1.dirY==-1)angle=0;
            else if(player1.dirX==0&&player1.dirY==1)angle=180;
            else if(player1.dirX==-1&&player1.dirY==0)angle=270;
            else if(player1.dirX==1&&player1.dirY==0)angle=90;
            SDL_RenderCopyEx(renderer,yellowTankTexture,NULL,&player1.rect,angle,NULL,SDL_FLIP_NONE);
            for (auto&bullet:player1.bullets)
            {
                bullet.render(renderer,bulletTexture);
            }
        }
        else
        {
            player1.render(renderer,1,bulletTexture);
        }
    }
    if(twoPlayerMode&&player2.active)
    {
        if(greenTankTexture)
        {
            double angle=0;
            if(player2.dirX==0&&player2.dirY==-1)angle=0;
            else if(player2.dirX==0&&player2.dirY==1)angle=180;
            else if(player2.dirX==-1&&player2.dirY==0)angle=270;
            else if(player2.dirX==1&&player2.dirY==0)angle=90;
            SDL_RenderCopyEx(renderer,greenTankTexture,NULL,&player2.rect,angle,NULL,SDL_FLIP_NONE);
            for (auto&bullet:player2.bullets)
            {
                bullet.render(renderer,bulletTexture);
            }
        }
        else
        {
            player2.render(renderer,2,bulletTexture);
        }
    }

    for (auto&enemy:enemies)
    {
        if(redTankTexture)
        {
            double angle=0;
            if(enemy.dirX==0&&enemy.dirY==-1)angle=0;
            else if(enemy.dirX==0&&enemy.dirY==1)angle=180;
            else if(enemy.dirX==-1&&enemy.dirY==0)angle=270;
            else if(enemy.dirX==1&&enemy.dirY==0)angle=90;
            SDL_RenderCopyEx(renderer,redTankTexture,NULL,&enemy.rect,angle,NULL,SDL_FLIP_NONE);
            for(auto&bullet:enemy.bullets)
            {
                bullet.render(renderer,bulletTexture);
            }
        }
        else
        {
            enemy.render(renderer,bulletTexture);
        }
    }

    if(explosionTexture)
    {
        for(auto i=explosions.begin();i!=explosions.end();)
        {
            if(i->isExpired())
            {
                i=explosions.erase(i);
            }
            else
            {
                SDL_RenderCopy(renderer,explosionTexture,NULL,&i->rect);
                ++i;
            }
        }
    }

    renderGameStatus();

    if(gameOver)
    {
        renderGameResult();
    }

    if(isPaused)
    {
        renderPauseMenu();
    }

    SDL_RenderPresent(renderer);
}

