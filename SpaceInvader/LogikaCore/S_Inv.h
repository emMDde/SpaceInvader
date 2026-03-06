#ifndef WSTEP_DO_PROJEKTU_SPACEINV_S_INV_H
#define WSTEP_DO_PROJEKTU_SPACEINV_S_INV_H

#include<array>

class S_Inv
{
public:
    static constexpr int height=30;
    static constexpr int width=40;

private:
    std::array<std::array<char,width>,height> si_board; //bufor do rysowania objektow
    int points;
    bool gameOver;

    int playerX;
    static constexpr int playerY=height-1; //gracz zawsze w ostatnim wierszu

    static constexpr int rowswithaliens=8;
    static constexpr int aliensperrow=width/2;
    static constexpr int amountofaliens=rowswithaliens*aliensperrow;
    static constexpr int heightofshields=3;
    static constexpr int widthofshields=width/10;
    static constexpr int amountofshields=5;
    static constexpr int amountofshieldspixels=amountofshields*widthofshields*heightofshields;

    struct Alien
    {
        int x,y;
        bool alive;
    };
    std::array<Alien,amountofaliens> aliens; //Tablica kosmitów

    struct ShieldPixel
    {
        int x,y;
        int health;
    };
    std::array<ShieldPixel,amountofshieldspixels> shields; //Tablica tarcz

    struct Bullet
    {
        int x,y;
        bool active; //true = leci, false = dostępny w magazynku
    };

    static constexpr int maxBullets=10;
    std::array<Bullet, maxBullets> playerBullets;
    std::array<Bullet, maxBullets/2> enemiesBullets;


    enum AlienDirection {
        DIR_LEFT,
        DIR_RIGHT
    };
    AlienDirection alienDir;

    void moveEnemiesRight();
    void moveEnemiesLeft();
    void moveEnemiesDown();

    static constexpr char playerSymbol='P';
    static constexpr char enemySymbol='E';
    static constexpr char emptySymbol='0';
    static constexpr char shieldSymbol='S';
    static constexpr char playerBulletSymbol='+';
    static constexpr char enemyBulletSymbol='-';
    static constexpr char errorSymbol='X';

    void clearBoard();
    void setChar(int ht, int wh, char c);
public:
    S_Inv();

    void printBoard();

    char getChar(int ht, int wh);

    int getPoints() const;
    void bonusPoints(int bonus);

    void movePlayerRight();
    void movePlayerLeft();

    void playerShoot();
    void updatePlayerBullets();
    void drawBulletsOnBoard();

    void enemyShoot();
    void updateEnemyBullets();
    void drawEnemyBulletsOnBoard();

    void updateEnemiesPositions();

    void renderGame();
};

#endif //WSTEP_DO_PROJEKTU_SPACEINV_S_INV_H
