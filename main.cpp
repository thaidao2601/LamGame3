#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<ctime>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<SDL.h>

using namespace std;

const int SCREEN_WIDTH=720;
const int SCREEN_HEIGHT=720;
const int TILE_SIZE=40;
const int MAP_WIDTH=SCREEN_WIDTH/TILE_SIZE;
const int MAP_HEIGHT=SCREEN_HEIGHT/TILE_SIZE;
const int playerSpeed=4;
const int enemySpeed=4;
const int bulletSpeed=10;
const int playerShootDelay=30;
const int playerLives=2;
const int BULLET_SIZE=10;
const int EnemiesNum=4;
const int EnemymoveDelay=3;
const int EnemyshootDelay=3;
const int ExplosionTime=1000;
const vector<vector<int>>Map=
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,0,0,1,1,0,0,0,1,0,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,1,0,1,0,0,0,1,1,0,0,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

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

    Menu()
    {
        running=true;
        startGame=false;
        playerNum=0;

        //Kiểm tra khởi tạo SDL
        if(SDL_Init(SDL_INIT_VIDEO)<0)
        {
            std::cerr<<"SDL could not initialize! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
            return;
        }

        if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)< 0)
        {
            std::cerr<<"SDL_mixer could not initialize! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
            running=false;
            return;
        }

        //Kiểm tra khởi tạo TTF
        if(TTF_Init()<0)
        {
            std::cerr<<"TTF could not initialize! TTF_Error: "<<TTF_GetError()<<std::endl;
            running=false;
            return;
        }

        //Tạo window và kiểm tra quá trình tạo window
        window=SDL_CreateWindow("Battle City",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
        if(!window)
        {
            std::cerr<<"Window could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
            return;
        }

        //Tạo render và kiểm tra qua trình tạo render
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        if(!renderer)
        {
            std::cerr<<"Renderer could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
            return;
        }

        menuMusic=Mix_LoadMUS("menumusic.mp3");
        if (!menuMusic)
        {
            std::cerr<<"Failed to load menu music! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
        }

        //Tải ảnh nền menu và kiểm tra quá trình tải
        backgroundTexture=IMG_LoadTexture(renderer,"menu_background.png");
        if(!backgroundTexture)
        {
            std::cerr<<"Failed to load menu background! IMG_Error: "<<IMG_GetError()<<std::endl;
        }

        //Thiết lập vị trị các nút trong menu
        int buttonWidth=300;
        int buttonHeight=60;
        int buttonGap=30;
        int startY=(SCREEN_HEIGHT/2)-buttonHeight-(buttonGap/2)-buttonHeight/2;
        onePlayerButton= {(SCREEN_WIDTH-buttonWidth)/2,startY,buttonWidth,buttonHeight};
        twoPlayerButton= {(SCREEN_WIDTH-buttonWidth)/2,startY+buttonHeight+buttonGap,buttonWidth,buttonHeight};
        exitButton= {(SCREEN_WIDTH-buttonWidth)/2,startY+2*(buttonHeight+buttonGap),buttonWidth,buttonHeight};
    }

    void renderText(const char *text,SDL_Rect rect,SDL_Color color)
    {
        //Tải phông và kiểm tra quá trình tải
        TTF_Font *font=TTF_OpenFont("consola.ttf",24);
        if(!font)
        {
            std::cerr<<"Failed to load font! TTF_Error: "<<TTF_GetError()<<std::endl;
            return;
        }

        //Tải bề mặt để chứa chữ, trên nền trong suốt
        SDL_Surface *textSurface=TTF_RenderUTF8_Solid(font,text,color);
        if(!textSurface)
        {
            TTF_CloseFont(font);
            return;
        }

        SDL_Texture *textTexture=SDL_CreateTextureFromSurface(renderer,textSurface);

        // Center text in button
        SDL_Rect textRect= {rect.x+(rect.w-textSurface->w)/2,rect.y+(rect.h-textSurface->h)/2,textSurface->w,textSurface->h};

        SDL_RenderCopy(renderer,textTexture,NULL,&textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        TTF_CloseFont(font);
    }

    void render()
    {
        if(backgroundTexture)
        {
            SDL_RenderCopy(renderer,backgroundTexture,NULL,NULL);
        }
        else
        {
            //Fall back to black background if image couldn't be loaded
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderClear(renderer);
        }
        //Draw title
        SDL_Color titleColor= {255,255,0,255}; //Yellow
        TTF_Font* titleFont=TTF_OpenFont("consola.ttf",48);
        if(!titleFont)
        {
            //Fallback if font file not found
            titleFont=TTF_OpenFont("consola.ttf",48);
        }

        if(titleFont)
        {
            SDL_Surface *titleSurface=TTF_RenderUTF8_Solid(titleFont,"Battle City",titleColor);
            if(titleSurface)
            {
                SDL_Texture *titleTexture=SDL_CreateTextureFromSurface(renderer,titleSurface);
                SDL_Rect titleRect= {(SCREEN_WIDTH-titleSurface->w)/2,100,titleSurface->w,titleSurface->h};
                SDL_RenderCopy(renderer,titleTexture,NULL,&titleRect);
                SDL_FreeSurface(titleSurface);
                SDL_DestroyTexture(titleTexture);
            }
            TTF_CloseFont(titleFont);
        }

        //Draw "One Player" button
        SDL_SetRenderDrawColor(renderer,100,100,255,255);//Blue button
        SDL_RenderFillRect(renderer,&onePlayerButton);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);//White border
        SDL_RenderDrawRect(renderer,&onePlayerButton);

        //Draw "Two Player" button
        SDL_SetRenderDrawColor(renderer,100,255,100,255);//Green button
        SDL_RenderFillRect(renderer,&twoPlayerButton);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);//White border
        SDL_RenderDrawRect(renderer,&twoPlayerButton);

        //Draw "Exit" button
        SDL_SetRenderDrawColor(renderer,255,100,100,255);//Red button
        SDL_RenderFillRect(renderer,&exitButton);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);//White border
        SDL_RenderDrawRect(renderer,&exitButton);

        //Draw button text
        SDL_Color textColor= {255,255,255,255}; //White text
        renderText("Một người chơi",onePlayerButton,textColor);
        renderText("Hai người chơi",twoPlayerButton,textColor);
        renderText("Thoát",exitButton,textColor);

        SDL_RenderPresent(renderer);
    }

    void handleEvents()
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

                // Check if One Player button was clicked
                if(mouseX>=onePlayerButton.x&&mouseX<=(onePlayerButton.x+onePlayerButton.w)&&
                        mouseY>=onePlayerButton.y&&mouseY<=(onePlayerButton.y+onePlayerButton.h))
                {
                    startGame=true;
                    running=false;
                    playerNum=1;
                }
                else if(mouseX>=twoPlayerButton.x&&mouseX<=(twoPlayerButton.x+twoPlayerButton.w)&&
                        mouseY>=twoPlayerButton.y&&mouseY<=(twoPlayerButton.y+twoPlayerButton.h))
                {
                    startGame=true;
                    running=false;
                    playerNum=2;
                }
                //Check if Exit button was clicked
                else if(mouseX>=exitButton.x&&mouseX<=(exitButton.x+exitButton.w)&&
                        mouseY>=exitButton.y&&mouseY<=(exitButton.y+exitButton.h))
                {
                    running=false;
                }
            }
        }
    }

    void run()
    {
        if(menuMusic)
        {
            if(Mix_PlayMusic(menuMusic,0)==-1)
            {
                std::cerr<<"Failed to play menu music! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
            }
        }
        while(running)
        {
            handleEvents();
            render();
            SDL_Delay(16);
        }
    }

    ~Menu()
    {
        if(backgroundTexture)
        {
            SDL_DestroyTexture(backgroundTexture);
        }
        if(menuMusic)
        {
            Mix_FreeMusic(menuMusic);
        }
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
};

class Wall
{
public:
    int x,y;
    SDL_Rect rect;
    bool active;

    Wall(int startX,int startY)
    {
        x=startX;
        y=startY;
        active=true;
        rect= {x,y,TILE_SIZE,TILE_SIZE};
    }
    void render(SDL_Renderer *renderer)
    {
        if(active)
        {
            SDL_SetRenderDrawColor(renderer,150,75,0,225);//Brown color
            SDL_RenderFillRect(renderer,&rect);
        }
    }
};

class Bullet
{
public:
    int x,y;
    int dx,dy;
    SDL_Rect rect;
    bool active;

    Bullet(int startX,int startY,int dirX,int dirY)
    {
        x=startX;
        y=startY;
        dx=dirX;
        dy=dirY;
        active=true;
        rect= {x,y,BULLET_SIZE,BULLET_SIZE}; //Square shape bullet
    }

    void move()
    {
        x+=dx*bulletSpeed;
        y+=dy*bulletSpeed;
        rect.x=x;
        rect.y=y;
        if(x<TILE_SIZE||x>SCREEN_WIDTH-TILE_SIZE||y<TILE_SIZE||y>SCREEN_HEIGHT-TILE_SIZE)
        {
            active=false;
        }
    }

    void render(SDL_Renderer *renderer)
    {
        if (active)
        {
            SDL_SetRenderDrawColor(renderer,255,255,255,255);
            SDL_RenderFillRect(renderer,&rect);
        }
    }

};

class Explosion
{
public:
    int x,y;
    SDL_Rect rect;
    Uint32 startTime;
    bool active;

    Explosion(int posX,int posY):x(posX),y(posY),active(true)
    {
        rect= {x,y,TILE_SIZE,TILE_SIZE};
        startTime=SDL_GetTicks();
    }

    bool isExpired()const
    {
        return SDL_GetTicks()-startTime>ExplosionTime;
    }
};

class PlayerTank
{
public:
    int x,y;
    int dirX,dirY;
    SDL_Rect rect;
    vector<Bullet>bullets;
    bool active;
    int shootDelay;

    PlayerTank(int startX,int startY)
    {
        x=startX;
        y=startY;
        rect= {x,y,TILE_SIZE,TILE_SIZE};
        dirX=0;
        dirY=-1;//Default direction up
        active=true;
        shootDelay=playerShootDelay;
    }

    void move(int dx,int dy,const vector<Wall>&walls)
    {
        int newX=x+dx*playerSpeed;
        int newY=y+dy*playerSpeed;
        this->dirX=dx;
        this->dirY=dy;

        SDL_Rect newRect= {newX,newY,TILE_SIZE,TILE_SIZE};
        for(int i=0; i<int(walls.size()); i++)
        {
            if (walls[i].active&&SDL_HasIntersection(&newRect,&walls[i].rect))
            {
                return;//Prevent movement if colliding with a wall
            }
        }

        if(newX>=TILE_SIZE&&newX<=SCREEN_WIDTH-TILE_SIZE*2&&newY>=TILE_SIZE&&newY<=SCREEN_HEIGHT-TILE_SIZE*2)
        {
            x=newX;
            y=newY;
            rect.x=x;
            rect.y=y;
        }
    }

    void shoot()
    {
        if(shootDelay<=0)
        {
            bullets.push_back(Bullet(x+TILE_SIZE/2-BULLET_SIZE/2,y+TILE_SIZE/2-BULLET_SIZE/2,this->dirX,this->dirY));
            shootDelay=playerShootDelay;
        }
    }

    void updateBullets()
    {
        if(shootDelay>0)
        {
            shootDelay--;
        }

        for(auto&bullet:bullets)
        {
            bullet.move();
        }

        bullets.erase(remove_if(bullets.begin(),bullets.end(),
                                [](Bullet &b)
        {
            return !b.active;
        }), bullets.end());
    }

    void render(SDL_Renderer *renderer,int id)
    {
        if(id==1)SDL_SetRenderDrawColor(renderer,0,250,250,255);
        else SDL_SetRenderDrawColor(renderer,0,17,250,255);
        SDL_RenderFillRect(renderer,&rect);
        for (auto&bullet:bullets)
        {
            bullet.render(renderer);
        }
    }
};

class EnemyTank
{
public:
    int x,y;
    int dirX,dirY;
    int moveDelay,shootDelay;
    int directionTimer;
    SDL_Rect rect;
    bool active;
    vector<Bullet>bullets;

    EnemyTank(int startX,int startY)
    {
        moveDelay=EnemymoveDelay;
        shootDelay=EnemyshootDelay;
        x=startX;
        y=startY;
        rect= {x,y,TILE_SIZE,TILE_SIZE};
        dirX=0;
        dirY=1;
        active=true;
        directionTimer=0;
    }

    void changeDirection()
    {
        int r=rand()%4;
        if(r==0)//Up
        {
            this->dirX=0;
            this->dirY=-1;
        }
        else if(r==1)//Down
        {
            this->dirX=0;
            this->dirY=1;
        }
        else if(r==2)//Left
        {
            this->dirY=0;
            this->dirX=-1;
        }
        else if(r==3)//Right
        {
            this->dirY=0;
            this->dirX=1;
        }
        directionTimer=0;
    }

    void move(const vector<Wall>&walls)
    {
        if(--moveDelay>0)return;
        moveDelay=EnemymoveDelay;
        directionTimer++;

        if(directionTimer>=180/EnemymoveDelay)
        {
            changeDirection();
        }

        int newX=x+this->dirX*enemySpeed;
        int newY=y+this->dirY*enemySpeed;
        SDL_Rect newRect= {newX,newY,TILE_SIZE,TILE_SIZE};

        bool wallCollision =false;
        for(const auto&wall:walls)
        {
            if(wall.active&&SDL_HasIntersection(&newRect,&wall.rect))
            {
                wallCollision=true;
                break;
            }
        }

        bool boundaryCollision=newX<TILE_SIZE||newX>SCREEN_WIDTH-TILE_SIZE*2||
                               newY<TILE_SIZE||newY>SCREEN_HEIGHT-TILE_SIZE*2;

        if(wallCollision||boundaryCollision)
        {
            changeDirection();
            return;
        }

        x=newX;
        y=newY;
        rect.x=x;
        rect.y=y;
    }

    void shoot()
    {
        if(--shootDelay>0)return;
        shootDelay=EnemyshootDelay;
        bullets.push_back(Bullet(x+TILE_SIZE/2-BULLET_SIZE/2,y+TILE_SIZE/2-BULLET_SIZE/2,this->dirX,this->dirY));
    }

    void updateBullets()
    {
        for (auto&bullet:bullets)
        {
            bullet.move();
        }
        bullets.erase(remove_if(bullets.begin(),bullets.end(),
                                [](Bullet &b)
        {
            return !b.active;
        }),bullets.end());
    }

    void render(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderFillRect(renderer,&rect);
        for(auto&bullet:bullets)
        {
            bullet.render(renderer);
        }
    }
};

class Game
{
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    //SDL_Texture *grassTexture=nullptr;
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
    vector<Wall>walls;
    PlayerTank player1=spawnPlayer1();
    PlayerTank player2=spawnPlayer2();
    bool twoPlayerMode=false;
    vector<EnemyTank>enemies;
    int enemyCount;
    int player1Lives;
    int player2Lives;
    bool gameOver;
    bool victory;
    SDL_Rect restartButton;
    SDL_Rect exitButton;
    bool returnToMenu;
    SDL_Texture *explosionTexture=nullptr;
    vector<Explosion>explosions;
    Mix_Music *backgroundMusic;

    void generateWalls()
    {
        for(int i=1; i<MAP_HEIGHT-1; i++)
        {
            for(int j=1; j<MAP_WIDTH-1; j++)
            {
                if(Map[i][j]==1)
                {
                    Wall w=Wall(j*TILE_SIZE,i*TILE_SIZE);
                    walls.push_back(w);
                }
            }
        }
    }

    void spawnEnemies()
    {
        enemies.clear();
        for(int i=0; i<EnemiesNum; ++i)
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
                    if(wall.active&&wall.x==ex&&wall.y==ey)
                    {
                        validPosition=false;
                        break;
                    }
                }
            }
            enemies.push_back(EnemyTank(ex,ey));
        }
    }

    PlayerTank spawnPlayer1()
    {
        return PlayerTank(((MAP_WIDTH-1)/2+1)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE);
    }

    PlayerTank spawnPlayer2()
    {
        return PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE);
    }

    Game(bool twoPlayer)
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
        backgroundMusic = Mix_LoadMUS("backgroundmusic.mp3");
        if(!backgroundMusic)
        {
            std::cerr<<"Failed to load background music! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
        }
        window=SDL_CreateWindow("Battle City",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
        if (!window)
        {
            std::cerr<<"Window could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
        }
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            std::cerr<<"Renderer could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
        }
        //grassTexture=IMG_LoadTexture(renderer,"grass_background2.png");
        //if(!grassTexture)
        //{
        //    std::cerr<<"Failed to load grass background! IMG_Error: "<<IMG_GetError()<<std::endl;
        //}
        brickTexture=IMG_LoadTexture(renderer,"brick.jpg");
        if(!brickTexture)
        {
            std::cerr<<"Failed to load brick texture! IMG_Error: "<<IMG_GetError()<<std::endl;
        }
        stoneTexture=IMG_LoadTexture(renderer,"stone.jpg");
        if(!stoneTexture)
        {
            std::cerr<<"Failed to load stone texture! IMG_Error: "<<IMG_GetError()<<std::endl;
        }
        yellowTankTexture=IMG_LoadTexture(renderer,"yellow_tank.png");
        if(!yellowTankTexture)
        {
            std::cerr<<"Failed to load yellow tank texture! IMG_Error: "<<IMG_GetError()<<std::endl;
        }
        greenTankTexture=IMG_LoadTexture(renderer,"green_tank.png");
        if(!greenTankTexture)
        {
            std::cerr<<"Failed to load green tank texture! IMG_Error: "<<IMG_GetError()<<std::endl;
        }
        redTankTexture=IMG_LoadTexture(renderer,"red_tank.png");
        if(!redTankTexture)
        {
            std::cerr<<"Failed to load red tank texture! IMG_Error: "<<IMG_GetError()<<std::endl;
        }
        explosionTexture=IMG_LoadTexture(renderer,"explosion.jpg");
        if (!explosionTexture)
        {
            std::cerr<<"Failed to load explosion texture! IMG_Error: "<<IMG_GetError()<<std::endl;
        }

        pauseButton= {SCREEN_WIDTH-TILE_SIZE-30,0,30,30};
        generateWalls();
        spawnEnemies();

    }

    void update()
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
                for(auto &bullet:player2.bullets)
                {
                    for(auto &wall:walls)
                    {
                        if(wall.active&&SDL_HasIntersection(&bullet.rect,&wall.rect))
                        {
                            wall.active=false;
                            bullet.active=false;
                            break;
                        }
                    }
                }

                for(auto &bullet:player2.bullets)
                {
                    for(auto &enemy:enemies)
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
                for(auto &enemy:enemies)
                {
                    for(auto &bullet:enemy.bullets)
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
                                    [](EnemyTank &e)
            {
                return !e.active;
            }),enemies.end());

            //if(enemies.empty())
            //{
            //    running=false;
            //}

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
                if(player1Lives>0)player1Lives--;
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

            //if(!player1.active&&(!twoPlayerMode||!player2.active))
            //{
            //    running = false;//Dừng game khi player1 không hoạt động và (không phải chế độ 2 người chơi hoặc player2 không hoạt động)
            //}
        }
    }

    void handleEvents()
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
                if(!isPaused&&mouseX>=pauseButton.x&&mouseX<=pauseButton.x+pauseButton.w&&
                        mouseY>=pauseButton.y&&mouseY<=pauseButton.y+pauseButton.h)
                {
                    isPaused=true;
                }

                if(isPaused)
                {
                    if (mouseX>=continueButton.x&&mouseX<=continueButton.x+continueButton.w&&
                            mouseY>=continueButton.y&&mouseY<=continueButton.y+continueButton.h)
                    {
                        isPaused=false;
                    }

                    if(mouseX>=exitGameButton.x&&mouseX<=exitGameButton.x+exitGameButton.w&&
                            mouseY>=exitGameButton.y&&mouseY<=exitGameButton.y+exitGameButton.h)
                    {
                        running=false;
                    }
                }
                if(gameOver)
                {
                    // Restart button
                    if(mouseX>=restartButton.x&&mouseX<=restartButton.x+restartButton.w&&
                            mouseY>=restartButton.y&&mouseY<=restartButton.y+restartButton.h)
                    {
                        returnToMenu=true;
                        running=false;
                    }

                    // Exit button
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
                }
            }
        }

        if (!isPaused)
        {
            const Uint8 *keystate=SDL_GetKeyboardState(NULL);

            // Player 1 controls
            if(keystate[SDL_SCANCODE_UP])player1.move(0,-1,walls);
            if(keystate[SDL_SCANCODE_DOWN])player1.move(0,1,walls);
            if(keystate[SDL_SCANCODE_LEFT])player1.move(-1,0,walls);
            if(keystate[SDL_SCANCODE_RIGHT])player1.move(1,0,walls);
            if(keystate[SDL_SCANCODE_SPACE])player1.shoot();

            // Player 2 controls(if two-player mode)
            if(twoPlayerMode && player2.active)
            {
                if(keystate[SDL_SCANCODE_W])player2.move(0,-1,walls);
                if(keystate[SDL_SCANCODE_S])player2.move(0,1,walls);
                if(keystate[SDL_SCANCODE_A])player2.move(-1,0,walls);
                if(keystate[SDL_SCANCODE_D])player2.move(1,0,walls);
                if(keystate[SDL_SCANCODE_K])player2.shoot();
            }
        }
    }

    void renderGameStatus()
    {
        TTF_Font *font=TTF_OpenFont("consola.ttf",20);
        if(!font)
        {
            std::cerr<<"Failed to load font!"<<std::endl;
            return;
        }
        SDL_Color textColor= {255,255,255,255};

        std::string enemyText="Địch còn lại: "+std::to_string(enemyCount);
        SDL_Surface *enemySurface=TTF_RenderUTF8_Solid(font,enemyText.c_str(),textColor);
        SDL_Texture *enemyTexture=SDL_CreateTextureFromSurface(renderer,enemySurface);
        SDL_Rect enemyRect= {10,10,enemySurface->w,enemySurface->h};
        SDL_RenderCopy(renderer,enemyTexture,NULL,&enemyRect);
        SDL_FreeSurface(enemySurface);
        SDL_DestroyTexture(enemyTexture);

        std::string player1Text="Xe tăng 1: "+std::to_string(player1Lives)+" mạng";
        SDL_Surface *player1Surface=TTF_RenderUTF8_Solid(font,player1Text.c_str(),textColor);
        SDL_Texture *player1Texture=SDL_CreateTextureFromSurface(renderer,player1Surface);
        SDL_Rect player1Rect= {10,40,player1Surface->w,player1Surface->h};
        SDL_RenderCopy(renderer,player1Texture,NULL,&player1Rect);
        SDL_FreeSurface(player1Surface);
        SDL_DestroyTexture(player1Texture);

        if(twoPlayerMode)
        {
            std::string player2Text="Xe tăng 2: "+std::to_string(player2Lives)+" mạng";
            SDL_Surface *player2Surface=TTF_RenderUTF8_Solid(font,player2Text.c_str(),textColor);
            SDL_Texture *player2Texture=SDL_CreateTextureFromSurface(renderer,player2Surface);
            SDL_Rect player2Rect= {10,70,player2Surface->w,player2Surface->h};
            SDL_RenderCopy(renderer,player2Texture,NULL,&player2Rect);
            SDL_FreeSurface(player2Surface);
            SDL_DestroyTexture(player2Texture);
        }

        TTF_CloseFont(font);
    }

    void renderGameResult()
    {
        TTF_Font *font=TTF_OpenFont("consola.ttf",48);
        if(!font)
        {
            std::cerr<<"Failed to load font!"<<std::endl;
            return;
        }

        SDL_Color textColor= {255,255,255,255};
        std::string resultText=victory?"Chiến Thắng!":"Thua Cuộc!";

        SDL_Surface *resultSurface=TTF_RenderUTF8_Solid(font,resultText.c_str(),textColor);
        SDL_Texture *resultTexture=SDL_CreateTextureFromSurface(renderer,resultSurface);
        SDL_Rect resultRect= {(SCREEN_WIDTH-resultSurface->w)/2,(SCREEN_HEIGHT-resultSurface->h)/2,resultSurface->w,resultSurface->h};
        SDL_SetRenderDrawColor(renderer,0,0,0,200);
        SDL_Rect overlay= {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer,&overlay);
        SDL_RenderCopy(renderer,resultTexture,NULL,&resultRect);
        SDL_FreeSurface(resultSurface);
        SDL_DestroyTexture(resultTexture);

        int buttonWidth=200;
        int buttonHeight=50;
        int startY=SCREEN_HEIGHT/2+50;
        restartButton= {(SCREEN_WIDTH-buttonWidth)/2-buttonWidth/2-10,startY,buttonWidth,buttonHeight};
        exitButton= {(SCREEN_WIDTH-buttonWidth)/2+buttonWidth/2+10,startY,buttonWidth,buttonHeight};
        SDL_SetRenderDrawColor(renderer,100,100,100,255);
        SDL_RenderFillRect(renderer,&restartButton);
        SDL_RenderFillRect(renderer,&exitButton);
        SDL_Color buttonTextColor= {255,255,255,255};
        renderText("Chơi lại",restartButton,buttonTextColor);
        renderText("Thoát",exitButton,buttonTextColor);

        TTF_CloseFont(font);
    }

    void renderPauseMenu()
    {
        SDL_SetRenderDrawColor(renderer,0,0,0,128);
        SDL_Rect overlay= {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer,&overlay);

        int buttonWidth=200;
        int buttonHeight=50;
        int startY=SCREEN_HEIGHT/2-buttonHeight-20;

        continueButton= {(SCREEN_WIDTH-buttonWidth)/2,startY,buttonWidth,buttonHeight};
        exitGameButton= {(SCREEN_WIDTH-buttonWidth)/2,startY+buttonHeight+20,buttonWidth,buttonHeight};

        SDL_SetRenderDrawColor(renderer,100,100,100,255);
        SDL_RenderFillRect(renderer,&continueButton);
        SDL_RenderFillRect(renderer,&exitGameButton);

        SDL_Color textColor= {255,255,255,255};
        renderText("Tiếp tục",continueButton,textColor);
        renderText("Thoát",exitGameButton,textColor);
    }

    void renderText(const char *text,SDL_Rect rect,SDL_Color color)
    {
        TTF_Font *font=TTF_OpenFont("consola.ttf",24);
        if (!font)
        {
            std::cerr<<"Failed to load font! TTF_Error: "<<TTF_GetError()<< std::endl;
            return;
        }

        SDL_Surface *textSurface=TTF_RenderUTF8_Solid(font,text,color);
        if (!textSurface)
        {
            TTF_CloseFont(font);
            return;
        }

        SDL_Texture *textTexture=SDL_CreateTextureFromSurface(renderer,textSurface);
        SDL_Rect textRect= {rect.x+(rect.w-textSurface->w)/2,rect.y+(rect.h-textSurface->h)/2,textSurface->w,textSurface->h};
        SDL_RenderCopy(renderer,textTexture,NULL,&textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        TTF_CloseFont(font);
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        //Draw the grass background
        //if(grassTexture)
        //{
        //Draw the grass background only in the playable area
        //    SDL_Rect playArea={TILE_SIZE,TILE_SIZE,SCREEN_WIDTH-2*TILE_SIZE,SCREEN_HEIGHT-2*TILE_SIZE};
        //    SDL_RenderCopy(renderer,grassTexture,NULL,&playArea);
        //}

        //Draw the boundary using stone texture
        if(stoneTexture)
        {
            //Top boundary
            SDL_Rect topBoundary= {0,0,SCREEN_WIDTH,TILE_SIZE};
            SDL_RenderCopy(renderer, stoneTexture,NULL,&topBoundary);

            //Bottom boundary
            SDL_Rect bottomBoundary= {0,SCREEN_HEIGHT-TILE_SIZE,SCREEN_WIDTH,TILE_SIZE};
            SDL_RenderCopy(renderer,stoneTexture,NULL,&bottomBoundary);

            //Left boundary
            SDL_Rect leftBoundary= {0,0,TILE_SIZE,SCREEN_HEIGHT};
            SDL_RenderCopy(renderer,stoneTexture,NULL,&leftBoundary);

            //Right boundary
            SDL_Rect rightBoundary= {SCREEN_WIDTH-TILE_SIZE,0,TILE_SIZE,SCREEN_HEIGHT};
            SDL_RenderCopy(renderer,stoneTexture,NULL,&rightBoundary);
        }
        else
        {
            //Fallback to drawing colored rectangles if texture loading failed
            SDL_SetRenderDrawColor(renderer,128,128,128,255);

            //Top boundary
            SDL_Rect topBoundary= {0,0,SCREEN_WIDTH,TILE_SIZE};
            SDL_RenderFillRect(renderer,&topBoundary);

            //Bottom boundary
            SDL_Rect bottomBoundary= {0,SCREEN_HEIGHT-TILE_SIZE,SCREEN_WIDTH,TILE_SIZE};
            SDL_RenderFillRect(renderer,&bottomBoundary);

            //Left boundary
            SDL_Rect leftBoundary= {0,0,TILE_SIZE,SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer,&leftBoundary);

            //Right boundary
            SDL_Rect rightBoundary= {SCREEN_WIDTH-TILE_SIZE,0,TILE_SIZE,SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer,&rightBoundary);
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderFillRect(renderer,&pauseButton);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderDrawLine(renderer,pauseButton.x+10,pauseButton.y+10,pauseButton.x+10,pauseButton.y+20);
        SDL_RenderDrawLine(renderer,pauseButton.x+20,pauseButton.y+10,pauseButton.x+20, pauseButton.y+20);


        //Draw walls with brick texture instead of solid color
        for(int i=0; i<int(walls.size()); ++i)
        {
            if(walls[i].active)
            {
                if(brickTexture)
                {
                    SDL_RenderCopy(renderer,brickTexture,NULL,&walls[i].rect);
                }
                else
                {
                    //Fallback to drawing colored rectangles if texture loading failed
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
                for(auto &bullet:player1.bullets)
                {
                    bullet.render(renderer);
                }
            }
            else
            {
                player1.render(renderer,1);
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
                for(auto &bullet:player2.bullets)
                {
                    bullet.render(renderer);
                }
            }
            else
            {
                player2.render(renderer,2);
            }
        }

        for(auto &enemy:enemies)
        {
            if(redTankTexture)
            {
                double angle=0;
                if(enemy.dirX==0&&enemy.dirY==-1)angle=0;
                else if(enemy.dirX==0&&enemy.dirY==1)angle=180;
                else if(enemy.dirX==-1&&enemy.dirY==0)angle=270;
                else if(enemy.dirX==1&&enemy.dirY==0)angle=90;
                SDL_RenderCopyEx(renderer,redTankTexture,NULL,&enemy.rect,angle,NULL,SDL_FLIP_NONE);
                for(auto &bullet:enemy.bullets)
                {
                    bullet.render(renderer);
                }
            }
            else
            {
                enemy.render(renderer);
            }
        }

        if(explosionTexture)
        {
            for(auto i=explosions.begin(); i!= explosions.end();)
            {
                if (i->isExpired())
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

        if (isPaused)
        {
            renderPauseMenu();
        }

        SDL_RenderPresent(renderer);
    }

    void run()
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

    ~Game()
    {
        //if(grassTexture)
        //{
        //    SDL_DestroyTexture(grassTexture);
        //}
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
        Mix_CloseAudio();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc,char *argv[])
{
    //srand(time(nullptr));//Random theo thời gian thực, đảm bảo luôn ngẫu nhiên

    //Kiểm tra khởi tạo thư viện IMG
    int imgFlags=IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags)&imgFlags))
    {
        std::cerr<<"SDL_image could not initialize! SDL_image Error: "<<IMG_GetError()<<std::endl;
    }

    while(true)
    {
        //Chạy menu game
        Menu menu;
        if(menu.running)
        {
            menu.run();
        }

        //Chạy game
        if(menu.startGame)
        {
            Game game(menu.playerNum==2);
            if(game.running)
            {
                game.run();
            }

            //Xử lí lựa chọn ở màn hình game over
            if(game.returnToMenu)
            {
                continue;//Trở về menu
            }
            else
            {
                break;//Thoát chương trình
            }
        }
        else
        {
            break;//Thoát khi menu dừng
        }
    }

    //Đóng các thư viện SDL
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    return 0;
}
