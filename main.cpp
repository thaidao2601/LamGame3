#include<iostream>
#include<vector>
#include<algorithm>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<SDL.h>

using namespace std;

const int SCREEN_WIDTH=800;
const int SCREEN_HEIGHT=600;
const int TILE_SIZE=40;
const int MAP_WIDTH=SCREEN_WIDTH/TILE_SIZE;
const int MAP_HEIGHT=SCREEN_HEIGHT/TILE_SIZE;
const int BULLET_SIZE=10;
const int EnemiesNum=4;
const int EnemymoveDelay=5;
const int EnemyshootDelay=3;

//Menu class for game start screen
class Menu
{
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    bool startGame;
    int playerNum;

    SDL_Rect onePlayerButton;
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

        if(TTF_Init() < 0)
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
        int startY=(SCREEN_HEIGHT/2)-buttonHeight-(buttonGap/2);

        onePlayerButton={(SCREEN_WIDTH-buttonWidth)/2,startY,buttonWidth,buttonHeight};
        exitButton={(SCREEN_WIDTH-buttonWidth)/2,startY+buttonHeight+buttonGap,buttonWidth,buttonHeight};
    }

    void renderText(const char *text,SDL_Rect rect,SDL_Color color)
    {
        TTF_Font *font=TTF_OpenFont("arial.ttf",24);
        if(!font)
        {
            //Fallback if font file not found
            font=TTF_OpenFont("arial.ttf",24);
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
        TTF_Font* titleFont=TTF_OpenFont("arial.ttf",48);
        if(!titleFont)
        {
            //Fallback if font file not found
            titleFont=TTF_OpenFont("arial.ttf",48);
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

        //Draw "Exit" button
        SDL_SetRenderDrawColor(renderer,255,100,100,255);//Red button
        SDL_RenderFillRect(renderer,&exitButton);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);//White border
        SDL_RenderDrawRect(renderer,&exitButton);

        //Draw button text
        SDL_Color textColor={255,255,255,255};//White text
        renderText("Một người chơi",onePlayerButton,textColor);
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
        x+=dx;
        y+=dy;
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

    PlayerTank(int startX,int startY)
    {
        x=startX;
        y=startY;
        rect= {x,y,TILE_SIZE,TILE_SIZE};
        dirX=0;
        dirY=-1;//Default direction up
    }

    void move(int dx,int dy,const vector<Wall>&walls)
    {
        int newX=x+dx;
        int newY=y+dy;
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
        bullets.push_back(Bullet(x+TILE_SIZE/2-BULLET_SIZE/2,y+TILE_SIZE/2-BULLET_SIZE/2,this->dirX,this->dirY));
    }

    void updateBullets()
    {
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

    void render(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer,255,255,0,255);
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
    }

    void move(const vector<Wall>&walls)
    {
        if(--moveDelay>0)return;
        moveDelay=EnemymoveDelay;
        int r=rand()%4;
        if(r==0)//Up
        {
            this->dirX=0;
            this->dirY=-5;
        }
        else if(r==1)//Down
        {
            this->dirX=0;
            this->dirY=5;
        }
        else if(r==2)//Left
        {
            this->dirY=0;
            this->dirX=-5;
        }
        else if(r==3)//Right
        {
            this->dirY=0;
            this->dirX=5;
        }

        int newX=x+this->dirX;
        int newY=y+this->dirY;
        SDL_Rect newRect={newX,newY,TILE_SIZE,TILE_SIZE};
        for(const auto&wall:walls)
        {
            if(wall.active&&SDL_HasIntersection(&newRect,&wall.rect))
            {
                return;
            }
        }
        if(newX>=TILE_SIZE&&newX<=SCREEN_WIDTH-TILE_SIZE*2&&
           newY>=TILE_SIZE&&newY<=SCREEN_HEIGHT-TILE_SIZE*2)
        {
            x=newX;
            y=newY;
            rect.x=x;
            rect.y=y;
        }
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
    bool running;
    vector<Wall>walls;
    PlayerTank player1=spawnPlayer1();
    //PlayerTank player2;
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
        for(int i=0; i<EnemiesNum; ++i)
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
        return PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE);
    }

    Game()
    {
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

        generateWalls();
        spawnEnemies();

    }

    void update()
    {
        player1.updateBullets();

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
                //Update
                if(SDL_HasIntersection(&bullet.rect,&player1.rect))
                {
                    running=false;
                    return;
                }
            }
        }
    }

    void handleEvents1()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT)
            {
                running=false;
            }
            else if(event.type==SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                case SDLK_UP:
                    player1.move(0,-5,walls);
                    break;
                case SDLK_DOWN:
                    player1.move(0,5,walls);
                    break;
                case SDLK_LEFT:
                    player1.move(-5,0,walls);
                    break;
                case SDLK_RIGHT:
                    player1.move(5,0,walls);
                    break;
                case SDLK_SPACE:
                    player1.shoot();
                    break;
                }
            }
        }
    }

    //void handleEvents2()

    void render()
    {
        SDL_SetRenderDrawColor(renderer,128,128,128,255);//Set draw color to gray for boundaries
        SDL_RenderClear(renderer);//Clear the renderer with the set color

        SDL_SetRenderDrawColor(renderer,0,0,0,255);//Set draw color to black for tiles
        for(int i=1;i<MAP_HEIGHT-1;++i)
        {
            for(int j=1;j<MAP_WIDTH-1;++j)
            {
                SDL_Rect tile={j*TILE_SIZE,i*TILE_SIZE,TILE_SIZE,TILE_SIZE};//Define tile rectangle
                SDL_RenderFillRect(renderer,&tile);//Fill the tile rectangle with the current draw color
            }
        }

        for(int i=0;i<int(walls.size());++i)
        {
            walls[i].render(renderer);
        }

        player1.render(renderer);

        for(auto&enemy:enemies)
        {
            enemy.render(renderer);
        }

        SDL_RenderPresent(renderer);//Update the screen with the rendered content
    }

    void run()
    {
        while(running)
        {
            update();
            render();
            handleEvents1();
            SDL_Delay(16);
        }
    }

    ~Game()
    {
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

    Menu menu;
    if(menu.running)
    {
        menu.run();
    }

    if(menu.startGame)
    {
        Game game;
        if(game.running)
        {
            game.run();
        }
    }

    TTF_Quit();
    return 0;
}
