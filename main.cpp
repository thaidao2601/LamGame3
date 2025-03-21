#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
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
const int bulletSpeed=4;
const int playerShootDelay=60;
const int enemySpeed=2;
const int BULLET_SIZE=10;
const int EnemiesNum=2;
const int EnemymoveDelay=5;
const int EnemyshootDelay=3;

class Menu
{
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    bool startGame;
    int playerNum;

    SDL_Rect onePlayerButton;
    SDL_Rect twoPlayerButton;
    SDL_Rect exitButton;

    Menu()
    {
        running=true;
        startGame=false;
        playerNum=0;

        if(SDL_Init(SDL_INIT_VIDEO)<0)
        {
            std::cerr<<"SDL could not initialize! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
            return;
        }

        if(TTF_Init()<0)
        {
            std::cerr<<"TTF could not initialize! TTF_Error: "<<TTF_GetError()<<std::endl;
            running=false;
            return;
        }

        window=SDL_CreateWindow("Battle City",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
        if(!window)
        {
            std::cerr<<"Window could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
            return;
        }

        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        if(!renderer)
        {
            std::cerr<<"Renderer could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
            return;
        }

        //Setup menu buttons (centered)
        int buttonWidth=300;
        int buttonHeight=60;
        int buttonGap=30;
        int startY=(SCREEN_HEIGHT/2)-buttonHeight-(buttonGap/2)-buttonHeight/2;

        onePlayerButton={(SCREEN_WIDTH-buttonWidth)/2,startY,buttonWidth,buttonHeight};
        twoPlayerButton={(SCREEN_WIDTH-buttonWidth)/2,startY+buttonHeight+buttonGap,buttonWidth,buttonHeight};
        exitButton={(SCREEN_WIDTH-buttonWidth)/2,startY+2*(buttonHeight+buttonGap),buttonWidth,buttonHeight};
    }

    void renderText(const char *text,SDL_Rect rect,SDL_Color color)
    {
        TTF_Font *font=TTF_OpenFont("timesbd.ttf",24);
        if(!font)
        {
            //Fallback if font file not found
            font=TTF_OpenFont("timesbd.ttf",24);
        }

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

        // Center text in button
        SDL_Rect textRect={rect.x+(rect.w-textSurface->w)/2,rect.y+(rect.h-textSurface->h)/2,textSurface->w,textSurface->h};

        SDL_RenderCopy(renderer,textTexture,NULL,&textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        TTF_CloseFont(font);
    }

    void render()
    {
        //Clear screen with black background
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        //Draw title
        SDL_Color titleColor={255,255,0,255};//Yellow
        TTF_Font* titleFont=TTF_OpenFont("timesbd.ttf",48);
        if(!titleFont)
        {
            //Fallback if font file not found
            titleFont=TTF_OpenFont("timesbd.ttf",48);
        }

        if(titleFont)
        {
            SDL_Surface *titleSurface=TTF_RenderUTF8_Solid(titleFont,"Battle City",titleColor);
            if(titleSurface)
            {
                SDL_Texture *titleTexture=SDL_CreateTextureFromSurface(renderer,titleSurface);
                SDL_Rect titleRect={(SCREEN_WIDTH-titleSurface->w)/2,100,titleSurface->w,titleSurface->h};
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
        SDL_Color textColor={255,255,255,255};//White text
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
        while(running)
        {
            handleEvents();
            render();
            SDL_Delay(16);
        }
    }

    ~Menu()
    {
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
        rect={x,y,TILE_SIZE,TILE_SIZE};
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
        rect={x,y,BULLET_SIZE,BULLET_SIZE};//Square shape bullet
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
        rect={x,y,TILE_SIZE,TILE_SIZE};
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

        SDL_Rect newRect={newX,newY,TILE_SIZE,TILE_SIZE};
        for(int i=0;i<int(walls.size());i++)
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
        rect={x,y,TILE_SIZE,TILE_SIZE};
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
            this->dirY=-enemySpeed;
        }
        else if(r==1)//Down
        {
            this->dirX=0;
            this->dirY=enemySpeed;
        }
        else if(r==2)//Left
        {
            this->dirY=0;
            this->dirX=-enemySpeed;
        }
        else if(r==3)//Right
        {
            this->dirY=0;
            this->dirX=enemySpeed;
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

        int newX=x+this->dirX;
        int newY=y+this->dirY;
        SDL_Rect newRect={newX,newY,TILE_SIZE,TILE_SIZE};

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
    SDL_Texture *grassTexture=nullptr;
    bool running;
    vector<Wall>walls;
    PlayerTank player1=spawnPlayer1();
    PlayerTank player2=spawnPlayer2();
    bool twoPlayerMode=false;
    vector<EnemyTank>enemies;

    void generateWalls()
    {
        for (int i=3;i<MAP_HEIGHT-3;i+=2)
        {
            for(int j=3;j<MAP_WIDTH-3;j+=2)
            {
                Wall w=Wall(j*TILE_SIZE,i*TILE_SIZE);
                walls.push_back(w);
            }
        }
    }

    void spawnEnemies()
    {
        enemies.clear();
        for(int i=0;i<EnemiesNum;++i)
        {
            int ex,ey;
            bool validPosition=false;
            while(!validPosition)
            {
                ex=(rand()%(MAP_WIDTH-2)+1)*TILE_SIZE;
                ey=(rand()%(MAP_HEIGHT-2)+1)*TILE_SIZE;
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
        twoPlayerMode=twoPlayer;
        running=true;
        if(SDL_Init(SDL_INIT_VIDEO)<0)
        {
            std::cerr<<"SDL could not initialize! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
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
        grassTexture=IMG_LoadTexture(renderer,"grass_background.png");
        if (!grassTexture)
        {
            std::cerr<<"Failed to load grass background! IMG_Error: "<<IMG_GetError()<<std::endl;
        }

        generateWalls();
        spawnEnemies();

    }

    void update()
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

        if(enemies.empty())
        {
            running=false;
        }

        for(auto&enemy:enemies)
        {
            for(auto&bullet:enemy.bullets)
            {
                if(SDL_HasIntersection(&bullet.rect,&player1.rect))
                {
                        player1.active=false;
                        bullet.active=false;
                        break;
                }
            }
        }

        if(!player1.active&&(!twoPlayerMode||!player2.active))
        {
            running = false;  // Dừng game khi player1 không hoạt động và (không phải chế độ 2 người chơi hoặc player2 không hoạt động)
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
        }

        //Kiểm tra trạng thái phím hiện tại(không phụ thuộc vào hàng đợi sự kiện)
        const Uint8 *keystate=SDL_GetKeyboardState(NULL);

        //Xử lý di chuyển cho người chơi 1(arrow keys+SPACE)
        if(keystate[SDL_SCANCODE_UP])player1.move(0,-1,walls);
        if(keystate[SDL_SCANCODE_DOWN])player1.move(0,1,walls);
        if(keystate[SDL_SCANCODE_LEFT])player1.move(-1,0,walls);
        if(keystate[SDL_SCANCODE_RIGHT])player1.move(1,0,walls);
        if(keystate[SDL_SCANCODE_SPACE])player1.shoot();

        // Xử lý di chuyển cho người chơi 2(WASD+K)
        if(twoPlayerMode && player2.active)
        {
            if(keystate[SDL_SCANCODE_W])player2.move(0,-1,walls);
            if(keystate[SDL_SCANCODE_S])player2.move(0,1,walls);
            if(keystate[SDL_SCANCODE_A])player2.move(-1,0,walls);
            if(keystate[SDL_SCANCODE_D])player2.move(1,0,walls);
            if(keystate[SDL_SCANCODE_K])player2.shoot();
        }
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer,128,128,128,255);
        SDL_RenderClear(renderer);

        //Draw the grass background
        if(grassTexture)
        {
            //Draw the grass background only in the playable area
            SDL_Rect playArea={TILE_SIZE,TILE_SIZE,SCREEN_WIDTH-2*TILE_SIZE,SCREEN_HEIGHT-2*TILE_SIZE};
            SDL_RenderCopy(renderer,grassTexture,NULL,&playArea);
        }

        //Draw the boundary(gray border)
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

        for(int i=0;i<int(walls.size());++i)
        {
            walls[i].render(renderer);
        }

        if(player1.active)player1.render(renderer,1);
        if(twoPlayerMode&&player2.active)player2.render(renderer,2);

        for(auto&enemy:enemies)
        {
            enemy.render(renderer);
        }

        SDL_RenderPresent(renderer);
    }

    void run()
    {
        while(running)
        {
            update();
            render();
            handleEvents();
            SDL_Delay(16);
        }
    }

    ~Game()
    {
        if(grassTexture)
        {
            SDL_DestroyTexture(grassTexture);
        }
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc,char *argv[])
{
    //Initialize SDL_ttf for menu text rendering
    if(TTF_Init()<0)
    {
        std::cerr<<"SDL_ttf could not initialize! SDL_ttf Error: "<<TTF_GetError()<<std::endl;
        return 1;
    }

    //Initialize SDL_image for loading PNG
    int imgFlags=IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags)&imgFlags))
    {
        std::cerr<<"SDL_image could not initialize! SDL_image Error: "<<IMG_GetError()<<std::endl;
        //Continue even if image loading fails
    }

    Menu menu;
    if(menu.running)
    {
        menu.run();
    }

    if(menu.startGame)
    {
        Game game(menu.playerNum==2);
        if(game.running)
        {
            game.run();
        }
    }

    IMG_Quit();
    TTF_Quit();
    return 0;
}
