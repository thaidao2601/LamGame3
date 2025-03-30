#include"Game.h"
#include<algorithm>

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
            Mix_HaltMusic();
        }
        if((player1Lives==0&&(!twoPlayerMode||player2Lives==0)))
        {
            gameOver=true;
            victory=false;
            Mix_HaltMusic();
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
