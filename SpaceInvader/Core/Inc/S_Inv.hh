#ifndef S_INV_H
#define S_INV_H

extern "C"
{
#include "drawFunctions.h"
#include "main.h"
}

#include<array>

class S_Inv
{
private:

    static constexpr int height=LCD_WIDTH;
    static constexpr int width=LCD_HEIGHT;

    bool gameOver=false;
    bool isWon=false;
    bool hardMode;

    float playerReloadTimer=0.0f;
    float enemyReloadTimer=0.0f;
    float playerShootCooldown=0.0f;
    float enemyShootCooldown=0.0f;

    float time_step=0.0f;
    int points=0;

    float playerX=width/2+PLAYER_WIDTH/2;
    float playerY=LCD_WIDTH-1-PLAYER_HEIGHT;

    static constexpr int rowswithenemys=2;
    static constexpr int enemysperrow=6;
    static constexpr int amountofenemys=rowswithenemys*enemysperrow;
    static constexpr int heightofshields=SHIELD_HEIGHT;
    static constexpr int widthofshields=SHIELD_WIDTH;
    static constexpr int amountofshields=2;

    int playerSpeed=50;
    int enemySpeed=40;
    int playerBulletSpeed=80;
    int enemyBulletSpeed=80;

    struct Enemy
    {
        float x,y;
        bool alive;
    };
    std::array<Enemy,amountofenemys> enemies; //Tablica kosmitów

    struct Shield
    {
        int x,y;
        int health;
    };
    std::array<Shield,amountofshields> shields; //Tablica tarcz

    struct Bullet
    {
        float x,y;
        bool active; //true = leci, false = dostępny w magazynku
    };

    static constexpr int maxBullets=5;
    std::array<Bullet, maxBullets> playerBullets;
    std::array<Bullet, maxBullets> enemiesBullets;

    enum Direction {
        DIR_LEFT,
        DIR_RIGHT
    };
    Direction enemyDir=DIR_RIGHT;

    void drawShields();
    void drawEnemies();
    void drawPlayerBullets();
    void drawEnemyBullets();

    void moveEnemiesLeft();
    void moveEnemiesRight();
    void moveEnemiesDown();
    void updateEnemiesPositions();
    void enemyShoot();

    void updateEnemyBullets();

    void movePlayerRight();
    void movePlayerLeft();
    void playerShoot();

    void updatePlayerBullets();

    void updatePoints(int bonus);
    void checkIfWon();
    bool isEnemyUnder(int shooter);

public:

    S_Inv(bool isHard);

    int getPoints() const;

    void updatePhysics(float dt, bool movePlayerLeft, bool movePlayerRight);
    void renderGame();

    bool isOver() const;
    bool getResult() const;
};

#endif
