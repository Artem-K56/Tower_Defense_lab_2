#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

// Контроллер
typedef struct {
    int pos_x; // Координата по x
    int pos_y; // Координата по y
}GameControl;

// Окно с ресурсами и счетом
typedef struct {
    int gold;   // Золото
    int score;  // Счет
}GameManager;

// База игрока
typedef struct {
    int max_health; // Максимальное значение здоровья базы
    int health;     // Значение здоровья базы
} Tower_Main;

// Союзная башня
typedef struct {
    int idEntity;   // id башни
    int health;     // Здоровье башни
    int buildcost;  // Стоимость башни
    int damage;     // Урон башни
    int pos_x;      // Координата по x
    int pos_y;      // Координата по y
    int range;      // Дистанция видимости башни
} Tower_Friend;

// Вражеская сущность
typedef struct {
    int idEntity;   // id сущности
    int health;     // Здоровье сущности
    int isAlive;    // Статус сущности
    int damage;     // Урон сущности
    int pos_x;      // Координата по x
    int pos_y;      // Координата по y
    int score;      // Количество очков, в находящейся вражеской сущности(врага)
} Enemy;

// Игровое поле 
typedef struct {
    int idCell[5][12];  // Массив id ячеек
}Map;

// Игра
typedef struct {
    int level;            // Уровень
    int gameOver;         // Статус игры
    GameManager manager;  // Окно с ресурсами и счетом
    Map map;              // Игровое поле
    Tower_Main base;      // База игрока
    GameControl control;  // Контроллер
    Tower_Friend towers[50];  // Массив союзных башен
    Enemy enemies[50];        // Массив вражеских сущностей
    int towerCount;             // Количество башен
    int enemyCount;             // Количество врагов
}Game;



/* ИНИЦИАЛИЗАЦИЯ */

// Инициализация игрового поля
void initMap(Map* map) {
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 12; j++)
            map->idCell[i][j] = 0;
}

// Инициализация базы
void initBase(Tower_Main* base) {
    base->max_health = 20;
    base->health = 20;
}

// Инициализация счета 
void initGameManager(GameManager* manager) {
    manager->gold = 4000;
    manager->score = 0;
};

// Инициализация контролера
void initControl(GameControl* player) {
    player->pos_x = 1;
    player->pos_y = 0;
    printf("The controller is initialized at coordinates (%d; %d)\n",
        player->pos_x, player->pos_y);
}

// Проверка границ для перемещения курсора игрока
int checkBorder(GameControl* pos) {
    // Проверка остальных границ карты
    if (pos->pos_y < 0 ||        // Выход за верх
        pos->pos_y >= 5 ||       // Выход за низ
        pos->pos_x < 1 ||        // Выход за левую зону 
        pos->pos_x >= 9) {       // Выход за правую границу
        return 1;
    }
    return 0;
}

// Проверка id ячейки карты
int checkCellMap(GameControl* control, Map* map) {
    return map->idCell[control->pos_y][control->pos_x];
};

// Вывод id ячейки карты на консоль
void outputCellMap(GameControl* control, Map* map) {
    int id = checkCellMap(control, map);
    if (id == 0)
        printf("This is cell empty\n");
    if (id == 1)
        printf("There is a tower on this cell.\n");
    if (id == 2)
        printf("There is a entity on this cell.\n");
};

// Перемещение позиции контролера игрока 
void movePos(GameControl* newPos, GameControl* oldPos, GameControl* player) {
    int result = checkBorder(newPos);

    if (result == 0) {
        *player = *newPos;  // Применение новой позиции
        printf("Cursor coordinates: (%d;%d)\n", player->pos_x, player->pos_y);
    }
    else {
        *player = *oldPos;  // Возвращение старой позиции
        printf("The border of the map! Moving is impossible! Your current position (%d; %d)\n",
            player->pos_x, player->pos_y);
    }
}

// Вывод текущего количества золота и очков на консоль
void outputManager(GameManager* manager) {
    printf("Gold = %d; Score = %d\n", manager->gold, manager->score);
}

// Спавн вражеской сущности
void spawnEnemy(Enemy* enemies, Map* map, int pos_x, int pos_y, int* enemyCount) {
    if (map->idCell[pos_y][pos_x] != 0) {
        printf("It is impossible to spawn a enemy. The cell is occupied.\n");
        return;
    }

    Enemy* enemy = &enemies[*enemyCount];
    enemy->health = 100;
    enemy->isAlive = 1;
    enemy->score = 100;
    enemy->damage = 10;
    enemy->pos_x = pos_x;
    enemy->pos_y = pos_y;
    enemy->idEntity = 2;

    // Отмечаем клетку
    map->idCell[pos_y][pos_x] = enemy->idEntity;
    (*enemyCount)++;

    printf("The enemy spawned at position (%d;%d)!\n",
        pos_y, pos_x);
}

// Постройка союзной башни
void buildTower(Map* map, GameManager* manager, Tower_Friend* towers, int* towerCount, GameControl* pos) {
    if (map->idCell[pos->pos_y][pos->pos_x] != 0) {
        printf("It is impossible to build a tower. The cell  is occupied.\n");
        return;
    }

    if (manager->gold < 100) {
        printf("There isn’t enough gold to build the tower.\n");
        return;
    }

    Tower_Friend* tower = &towers[*towerCount];
    tower->health = 100;
    tower->buildcost = 100;
    tower->damage = 50;
    tower->pos_x = pos->pos_x;
    tower->pos_y = pos->pos_y;
    tower->range = 12;
    tower->idEntity = 1;


    // Отмечается клетка
    map->idCell[pos->pos_y][pos->pos_x] = tower->idEntity;

    // Расход золота
    manager->gold -= tower->buildcost;

    (*towerCount)++;

    printf("Tower built at position (%d;%d)! Gold left: %d\n",
        pos->pos_y, pos->pos_x, manager->gold);
}

// Поиск врага на той же строке, в пределах радиуса
int findEnemyHorizontal(Tower_Friend* tower, Enemy* enemies, int enemyCount) {
    for (int i = 0; i < enemyCount; i++) {
        // Пропускаем мёртвых врагов (isAlive == 0)
        if (enemies[i].isAlive == 0) continue;
        if (enemies[i].pos_y != tower->pos_y) continue;

        int dx = enemies[i].pos_x - tower->pos_x;
        if (dx < 0) dx = -dx;

        if (dx <= tower->range) {
            return i;
        }
    }
    return -1;
}

// Атака союзной башни по вражеской сущности
void towerAttack(Tower_Friend* tower, Enemy* enemy) {
    if (enemy->isAlive == 0) return;

    enemy->health -= tower->damage;
    printf("Tower attacks! Enemy HP: %d\n", enemy->health);

    if (enemy->health <= 0) {
        enemy->isAlive = 0;
        printf("Enemy destroyed!\n");
    }
}

// Обновление башен (поиск и атака)
void updateTowers(Tower_Friend* towers, int towerCount, Enemy* enemies, int enemyCount) {
    for (int i = 0; i < towerCount; i++) {
        int enemyIndex = findEnemyHorizontal(&towers[i], enemies, enemyCount);
        if (enemyIndex != -1) {
            towerAttack(&towers[i], &enemies[enemyIndex]);
        }
    }
}

// Проверка окончания игры
void checkWin(Game* game) {
    int enemyCountisDead = 0;
    for (int i = 0; i < game->enemyCount; i++) {
        if (game->enemies[i].isAlive == 0)
            enemyCountisDead++;
    }

    if (game->enemyCount == enemyCountisDead) {
        printf("You win!\n");
        game->gameOver = 1;
    }
}

// Движение курсора игрока
void movePlayer(Map* map, GameManager* manager, Tower_Friend* towers, int* towerCount, GameControl* player) {
    if (_kbhit()) {
        char key = _getch();

        // Сохранение старой позиции
        GameControl oldPos = *player;
        // Создание новой позицию (копия старой)
        GameControl newPos = oldPos;

        // Изменяем новую позицию в зависимости от нажатой клавиши
        switch (key) {
        case 'W': case 'w':
            newPos.pos_y -= 1;
            movePos(&newPos, &oldPos, player);
            break;
        case 'S': case 's':
            newPos.pos_y += 1;
            movePos(&newPos, &oldPos, player);
            break;
        case 'A': case 'a':
            newPos.pos_x -= 1;
            movePos(&newPos, &oldPos, player);
            break;
        case 'D': case 'd':
            newPos.pos_x += 1;
            movePos(&newPos, &oldPos, player);
            break;
        case 'F': case 'f':
            outputCellMap(&newPos, map);
            break;
        case 'I': case 'i':
            outputManager(manager);
            break;
        case 'B': case 'b':
            buildTower(map, manager, towers, towerCount, player);
            break;
        default:
            return;
        }
    }
        }


//Вывод начальной статичной карты игры на консоль (для представления, как выглядит карта)
void printMap() {
    printf("\nTutorial\n");
    printf("|   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | \n");
    printf("| 0 | B | ! |   |   |   |   |   |   |   | S | S  | E  | \n");
    printf("| 1 | B |   |   |   |   |   |   |   |   | S | S  | E  | \n");
    printf("| 2 | B |   |   |   |   |   |   |   |   | S | S  | E  | \n");
    printf("| 3 | B |   |   |   |   |   |   |   |   | S | S  | E  | \n");
    printf("| 4 | B |   |   |   |   |   |   |   |   | S | S  | E  | \n\n");

    printf("\n B - Base\n S - Spawn zone enemy\n E - Enemy\n ! - Controller spawn\n");

    printf("\nController management:\n");
    printf(" W - Up\n S - Down\n A - Left\n D - Right\n B - Build a tower\n F - Output info about cell ID\n I - Output info about gold and score\n\n");

    printf("");
}

//Инициализация игры
void initGame(Game* game) {
    game->level = 1;
    game->gameOver = 0;
    initMap(&game->map); //Инициализация карты
    initGameManager(&game->manager);  // Инициализация счета
    initBase(&game->base);  //Инициализация базы
    initControl(&game->control); // Инициализация позиции контролера
    game->towerCount = 0;
    game->enemyCount = 0;
    game->gameOver = 0;
}

int main()
{
    Game game;
    printMap();
    initGame(&game);
    spawnEnemy(game.enemies, &game.map, 11, 0, &game.enemyCount);
    spawnEnemy(game.enemies, &game.map, 11, 1, &game.enemyCount);
    spawnEnemy(game.enemies, &game.map, 11, 2, &game.enemyCount);
    spawnEnemy(game.enemies, &game.map, 11, 3, &game.enemyCount);
    spawnEnemy(game.enemies, &game.map, 11, 4, &game.enemyCount);
    while (!game.gameOver) {
        // Обновляем башни
        updateTowers(game.towers, game.towerCount, game.enemies, game.enemyCount);
        movePlayer(&game.map, &game.manager, game.towers, &game.towerCount, &game.control);
        checkWin(&game);

    }

}