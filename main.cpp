#include <iostream>
#include<vector>
#include <SDL.h>

using namespace std;

const int SCREEN_WIDTH=800;
const int SCREEN_HEIGHT=600;
const int TILE_SIZE=40;
const int MAP_WIDTH=SCREEN_WIDTH/TILE_SIZE;
const int MAP_HEIGHT=SCREEN_HEIGHT/TILE_SIZE;

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

class Game
{
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    vector<Wall>walls;

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

    Game()
    {
        bool running=true;
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

    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer,128,128,128,255);//Set draw color to gray for boundaries
        SDL_RenderClear(renderer);//Clear the renderer with the set color

        SDL_SetRenderDrawColor(renderer,0,0,0,255);//Set draw color to black for tiles
        for(int i=1; i<MAP_HEIGHT-1; ++i)
        {
            for(int j=1; j<MAP_WIDTH-1; ++j)
            {
                SDL_Rect tile= {j*TILE_SIZE,i*TILE_SIZE,TILE_SIZE,TILE_SIZE}; //Define tile rectangle
                SDL_RenderFillRect(renderer,&tile);//Fill the tile rectangle with the current draw color
            }
        }

        for(int i=0;i<walls.size();++i)
        {
            walls[i].render(renderer);
        }

        SDL_RenderPresent(renderer);//Update the screen with the rendered content
    }
    void run()
    {
        while(running)
        {
            render();
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
    Game game;
    if(game.running)
    {
        game.run();
    }
    return 0;
}
