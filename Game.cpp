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

void Game::generateWalls()
{
    for(int i=1;i<MAP_HEIGHT-1;i++)
    {
        for(int j=1;j<MAP_WIDTH-1;j++)
        {
            if(Map[i][j]==1)
            {
                Wall w=Wall(j*TILE_SIZE,i*TILE_SIZE);
                walls.push_back(w);
            }
        }
    }
}

void Game::spawnEnemies()
{
    enemies.clear();
    for (int i=0;i<EnemiesNum;++i)
    {
        int ex,ey;
        bool validPosition=false;
        while(!validPosition)
        {
            ex=(rand()%(MAP_WIDTH-2)+1)*TILE_SIZE;
            ey=(rand()%(5)+1)*TILE_SIZE;
            validPosition=true;
            for(const auto&wall:walls)
            {
                if (wall.active&&wall.x==ex&&wall.y==ey)
                {
                    validPosition=false;
                    break;
                }
            }
        }
        enemies.push_back(EnemyTank(ex,ey));
    }
}

PlayerTank Game::spawnPlayer1()
{
    return PlayerTank(((MAP_WIDTH-1)/2+1)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE);
}

PlayerTank Game::spawnPlayer2()
{
    return PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE);
}

void Game::update()
{
    if(!isPaused&&!gameOver)
    {
        player1.updateBullets();
        if(twoPlayerMode)player2.updateBullets();

        for(auto&enemy:enemies)
        {
            enemy.move(walls);
            enemy.updateBullets();
            if (rand()%100<2)
            {
                enemy.shoot();
            }
        }

        for(auto&enemy:enemies)
        {
            for(auto&bullet:enemy.bullets)
            {
                for(auto&wall:walls)
                {
                    if(wall.active&&SDL_HasIntersection(&bullet.rect,&wall.rect))
                    {
                        wall.active=false;
                        bullet.active=false;
                        break;
                    }
                }
            }
        }

        for(auto&bullet:player1.bullets)
        {
            for(auto&wall:walls)
            {
                if(wall.active&&SDL_HasIntersection(&bullet.rect,&wall.rect))
                {
                    wall.active=false;
                    bullet.active=false;
                    break;
                }
            }
        }

        for(auto&bullet:player1.bullets)
        {
            for(auto&enemy:enemies)
            {
                if(enemy.active&&SDL_HasIntersection(&bullet.rect,&enemy.rect))
                {
                    explosions.push_back(Explosion(enemy.rect.x,enemy.rect.y));
                    enemy.active=false;
                    bullet.active=false;
                    break;
                }
            }
        }

        if(twoPlayerMode)
        {
            for(auto&bullet:player2.bullets)
            {
                for(auto&wall:walls)
                {
                    if(wall.active&&SDL_HasIntersection(&bullet.rect,&wall.rect))
                    {
                        wall.active=false;
                        bullet.active=false;
                        break;
                    }
                }
            }

            for(auto&bullet:player2.bullets)
            {
                for(auto&enemy:enemies)
                {
                    if(enemy.active&&SDL_HasIntersection(&bullet.rect,&enemy.rect))
                    {
                        explosions.push_back(Explosion(enemy.rect.x,enemy.rect.y));
                        enemy.active=false;
                        bullet.active=false;
                        break;
                    }
                }
            }
        }

        if(twoPlayerMode)
        {
            for (auto&enemy:enemies)
            {
                for (auto&bullet:enemy.bullets)
                {
                    if(SDL_HasIntersection(&bullet.rect,&player2.rect))
                    {
                        explosions.push_back(Explosion(player2.rect.x,player2.rect.y));
                        player2.active=false;
                        bullet.active=false;
                        break;
                    }
                }
            }
        }

        enemies.erase(remove_if(enemies.begin(),enemies.end(),
                                [](EnemyTank& e)
        {
            return !e.active;
        }),enemies.end());

        for(auto&enemy:enemies)
        {
            for(auto&bullet:enemy.bullets)
            {
                if(SDL_HasIntersection(&bullet.rect,&player1.rect))
                {
                    explosions.push_back(Explosion(player1.rect.x,player1.rect.y));
                    player1.active=false;
                    bullet.active=false;
                    break;
                }
            }
        }

        if(!player1.active)
        {
            if(player1Lives> 0)player1Lives--;
            if(player1Lives>0)
            {
                player1=spawnPlayer1();
            }
        }

        if(twoPlayerMode&&!player2.active)
        {
            if(player2Lives>0)player2Lives--;
            if(player2Lives>0)
            {
                player2=spawnPlayer2();
            }
        }

        enemyCount=enemies.size();
        if(enemyCount==0)
        {
            gameOver=true;
            victory=true;
        }
        if((player1Lives==0&&(!twoPlayerMode||player2Lives==0)))
        {
            gameOver=true;
            victory=false;
        }
    }
}

void Game::handleEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type==SDL_QUIT)
        {
            running=false;
        }
        else if(event.type==SDL_MOUSEBUTTONDOWN)
        {
            int mouseX,mouseY;
            SDL_GetMouseState(&mouseX,&mouseY);
            if (!isPaused&&mouseX>=pauseButton.x&&mouseX<=pauseButton.x+pauseButton.w&&
                           mouseY>=pauseButton.y&&mouseY<=pauseButton.y+pauseButton.h)
            {
                isPaused=true;
                Mix_PauseMusic();
            }

            if(isPaused)
            {
                if(mouseX>=continueButton.x&&mouseX<=continueButton.x+continueButton.w&&
                   mouseY>=continueButton.y&&mouseY<=continueButton.y+continueButton.h)
                {
                    isPaused=false;
                    Mix_ResumeMusic();
                }

                if(mouseX>=exitGameButton.x&&mouseX<=exitGameButton.x+exitGameButton.w&&
                   mouseY>=exitGameButton.y&&mouseY<=exitGameButton.y+exitGameButton.h)
                {
                    running=false;
                }
            }
            if(gameOver)
            {
                if(mouseX>=restartButton.x&&mouseX<=restartButton.x+restartButton.w&&
                        mouseY>=restartButton.y&&mouseY<=restartButton.y+restartButton.h)
                {
                    returnToMenu=true;
                    running=false;
                }

                if(mouseX>=exitButton.x&&mouseX<=exitButton.x+exitButton.w&&
                   mouseY>=exitButton.y&&mouseY<=exitButton.y+exitButton.h)
                {
                    running=false;
                }
            }
        }
        else if(event.type==SDL_KEYDOWN)
        {
            if(event.key.keysym.sym==SDLK_ESCAPE)
            {
                isPaused=!isPaused;
                if(isPaused)Mix_PauseMusic();
                else Mix_ResumeMusic();
            }
        }
    }

    if(!isPaused)
    {
        const Uint8 *keystate=SDL_GetKeyboardState(NULL);

        //Player 1 controls
        if(keystate[SDL_SCANCODE_UP])player1.move(0,-1,walls);
        if(keystate[SDL_SCANCODE_DOWN])player1.move(0,1,walls);
        if(keystate[SDL_SCANCODE_LEFT])player1.move(-1,0,walls);
        if(keystate[SDL_SCANCODE_RIGHT])player1.move(1,0,walls);
        if(keystate[SDL_SCANCODE_SPACE])player1.shoot();

        //Player 2 controls(if two-player mode)
        if(twoPlayerMode&&player2.active)
        {
            if(keystate[SDL_SCANCODE_W])player2.move(0,-1,walls);
            if(keystate[SDL_SCANCODE_S])player2.move(0,1,walls);
            if(keystate[SDL_SCANCODE_A])player2.move(-1,0,walls);
            if(keystate[SDL_SCANCODE_D])player2.move(1,0,walls);
            if(keystate[SDL_SCANCODE_K])player2.shoot();
        }
    }
}

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
    restartButton={(SCREEN_WIDTH-buttonWidth)/2-buttonWidth/2-10,startY,buttonWidth,buttonHeight};
    exitButton={(SCREEN_WIDTH-buttonWidth)/2+buttonWidth/2+10,startY,buttonWidth,buttonHeight};
    SDL_SetRenderDrawColor(renderer,100,100,100,255);
    SDL_RenderFillRect(renderer,&restartButton);
    SDL_RenderFillRect(renderer,&exitButton);
    SDL_Color buttonTextColor={255,255,255,255};
    renderText("Chơi lại",restartButton,buttonTextColor);
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
