#ifndef CONSTANTS_H
#define CONSTANTS_H
#include<vector>

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

extern const std::vector<std::vector<int>>Map;

#endif
