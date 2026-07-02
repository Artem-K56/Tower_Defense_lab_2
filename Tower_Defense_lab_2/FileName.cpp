#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

//Структура позиции
typedef struct  {
    int pos_x;
    int pos_y;
}Position;

//Структура базы
typedef struct {
    int health; //Значение здоровья базы
} Tower_Main;

//Структура главного контроллера
typedef struct {
    int gold_Player;  //Количество золота у игрока
    int score;        //Количество очков у игрока
} GameManager;

//Структура сущности
typedef struct {
    int health; //Значение здоровья сущности

} Entity;

//Структура союзной башни
typedef struct {
    Entity tower;

} Tower_Friend;

//Структура вражеской сущности
typedef struct {
    Entity enemy;
    int score;      //Количество очков, в находящейся вражеской сущности(врага)
    float speed;    //Скорость перемещения вражеской сущности(врага) по карте
    float damage;   //Урон, вражеской сущности
} Enemy;

//Структура снаряда
typedef struct {
    float speed;   //Скорость перемещения снаряда 
    float damage;  //Урон снаряда
} Projectile;
 
//Структура игрового поля 
typedef struct {
    Position pos[5][9];
}Map;

//Структура игры
typedef struct {
    int level;
    int gameOver;
    Map map;
}Game;



/* ИНИЦИАЛИЗАЦИЯ */
// Инициализация позиции
void initPosition(Position* pos) {
    pos->pos_x = 0;
    pos->pos_y = 0;
}

//Инициализация игрового поля
void initMap(Map* map) {
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 9; j++) {
            map->pos[i][j].pos_x = j;
            map->pos[i][j].pos_y = i;
        }
}

//Инициализация игры
void initGame(Game* game) {
    game->level = 1;
    game->gameOver = 0;
    initMap(&game->map); //Инициализация карты

}

int main()
{
    setlocale(LC_ALL, "Rus");
    Game game;
    initGame(&game);
}