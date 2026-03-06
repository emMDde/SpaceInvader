#include "S_Inv.h"
#include <stdio.h>
#include <cstdlib>

S_Inv::S_Inv()
{
    clearBoard();
    playerX=width/2;
    alienDir=DIR_RIGHT;
    points=0;
    gameOver=false;

    //Ustawienie statkow przeciwnika
    int ind=0;
    const int startX=0;
    const int startY=0;
    const int xSpacing=2; //"co dwie kolumny"
    const int ySpacing=2; //"co dwa rzędy"

    for(int row=0;row<rowswithaliens;++row)
    {
        for(int col=0;col<aliensperrow;++col)
        {
            if(ind>=amountofaliens)
            {
                break;
            }

            int currentX=startX+col*xSpacing;
            int currentY=startY+row*ySpacing;

            aliens[ind].x=currentX;
            aliens[ind].y=currentY;
            aliens[ind].alive=true;

            ind++;
        }
        if (ind>=amountofaliens)
        {
            break;
        }
    }

    //Ustawienie Tarcz nad graczem
    ind=0;
    const int highestShieldRow=height-6;
    const int shieldSlotWidth=width/amountofshields;
    const int shieldMarginX=(shieldSlotWidth-widthofshields)/2;


    for(int s=0;s<amountofshields;++s)
    {
        int currentShieldStartX=s*shieldSlotWidth+shieldMarginX;

        for(int row=0;row<heightofshields;++row)
        {
            int currentY=highestShieldRow+row;

            for(int col=0;col<widthofshields;++col)
            {
                if(ind>=amountofshieldspixels)
                {
                    break;
                }
                int currentX=currentShieldStartX+col;

                shields[ind].x=currentX;
                shields[ind].y=currentY;
                shields[ind].health=3;

                ind++;
            }
            if(ind>=amountofshieldspixels) break;
        }
        if(ind>=amountofshieldspixels) break;
    }

    //Ustawinie pociskow na niekatywne
    for(auto& b:playerBullets)
    {
        b.active=false; //Początkowo żaden pocisk nie leci
    }

    for(auto& b:enemiesBullets)
    {
        b.active=false;//Początkowo żaden pocisk nie leci
    }
}

void S_Inv::clearBoard()
{
    for(auto &row:si_board)
    {
        for(auto &elem:row)
        {
            elem=emptySymbol;
        }
    }
}

void S_Inv::printBoard()
{
    for(int ht=0;ht<height;++ht)
    {
        for(int wh=0;wh<width;++wh)
        {
            printf("%c",si_board[ht][wh]);
        }
        printf("\r\n");
    }
}

char S_Inv::getChar(int h,int w)
{
    if (h>=0 && h<height && w>=0 && w<width)
    {
        return si_board[h][w];
    }
    return errorSymbol; //Znak błędu
}

void S_Inv::setChar(int h,int w,char c)
{
    if (h>=0 && h<height && w>=0 && w<width)
    {
        si_board[h][w]=c;
    }
}

void S_Inv::movePlayerLeft()
{
    if (gameOver)
    {
        return;
    }

    if(playerX>0)
    {
        playerX--;
    }
}

void S_Inv::movePlayerRight()
{
    if (gameOver)
    {
        return;
    }

    if(playerX<(width-1))
    {
        playerX++;
    }
}

void S_Inv::moveEnemiesRight()
{
    for (auto& alien : aliens)
    {
        if (alien.alive)
        {
            alien.x++;
        }
    }
}

void S_Inv::moveEnemiesLeft()
{
    for (auto& alien : aliens)
    {
        if (alien.alive)
        {
            alien.x--;
        }
    }
}

void S_Inv::moveEnemiesDown()
{
    for (auto& alien : aliens)
    {
        if (alien.alive)
        {
            alien.y++;
        }
    }
}

void S_Inv::updateEnemiesPositions()
{
    bool wallHit=false;

    for (const auto& alien : aliens)
    {
        if (!alien.alive)
        {
            continue; //Pomiń martwych
        }

        //Sprawdź prawą ścianę, jeśli poruszamy się w prawo
        if (alienDir==DIR_RIGHT && alien.x==(width-1))
        {
            wallHit=true;
            break; //Wystarczy, że JEDEN uderzy
        }

        //Sprawdź lewą ścianę, jeśli poruszamy się w lewo
        if (alienDir==DIR_LEFT && alien.x==0)
        {
            wallHit=true;
            break; //Wystarczy, że JEDEN uderzy
        }
    }

    if (wallHit)
    {
        moveEnemiesDown();

        if(alienDir==DIR_RIGHT)
        {
            alienDir=DIR_LEFT;
        }
        else
        {
            alienDir=DIR_RIGHT;
        }
    }
    else
    {
        if(alienDir==DIR_RIGHT)
        {
            moveEnemiesRight();
        }
        else
        {
            moveEnemiesLeft();
        }
    }
}

void S_Inv::renderGame()
{
    clearBoard();

    drawBulletsOnBoard();
    drawEnemyBulletsOnBoard();

    for (const auto& shield:shields)
    {
        if (shield.health>0)
        {
            setChar(shield.y,shield.x,shieldSymbol);
        }
    }
    for (const auto& alien:aliens)
    {
        if (alien.alive)
        {
            setChar(alien.y,alien.x,enemySymbol);
        }
    }

    setChar(playerY,playerX,playerSymbol);
}

void S_Inv::playerShoot()
{
    if (gameOver)
    {
        return;
    }

    // Szukamy pierwszego pocisku, który NIE jest aktywny
    for(auto& b:playerBullets)
    {
        if(!b.active)
        {
            b.active=true;
            b.x=playerX;
            b.y=playerY-1;
            break;
        }
    }
}

void S_Inv::updatePlayerBullets()
{
    for(auto& b:playerBullets)
    {
        //nieaktywny-pomiń
        if(!b.active) continue;

        b.y--;

        if(b.y<0)
        {
            b.active=false;
            continue;
        }

        //DETEKCJA KOLIZJI Z WROGAMI
        for(auto& alien:aliens)
        {
            if(alien.alive)
            {
                if(b.x==alien.x && b.y==alien.y)
                {
                    alien.alive=false;
                    b.active=false;
                    bonusPoints(10);
                    break;
                }
            }
        }

        if(!b.active) continue;


        //DETEKCJA KOLIZJI Z TARCZAMI
        for(auto& shield:shields)
        {
            //Sprawdzamy istniejące fragmenty tarczy(health>0)
            if(shield.health>0)
            {
                if(b.x==shield.x && b.y==shield.y)
                {
                    //Trafiono tarczę własnym pociskiem
                    b.active=false; //Pocisk znika
                    shield.health--; //zakomentowane - nie niszczy tarczy pocisk gracza
                    if(shield.health==0)
                    {
                        bonusPoints(-5);
                    }

                    break;
                }
            }
        }
    }
}

void S_Inv::drawBulletsOnBoard()
{
    for(const auto& b:playerBullets)
    {
        if(b.active)
        {
            if (b.x>=0 && b.x<width && b.y>=0 && b.y<height)
            {
                si_board[b.y][b.x]=playerBulletSymbol;
            }
        }
    }
}

void S_Inv::enemyShoot()
{
    Bullet *freeBullet=nullptr;
    for (auto &b:enemiesBullets)
    {
        if (!b.active)
        {
            freeBullet=&b;
            break;
        }
    }
    if (freeBullet==nullptr) return; //brak wolnego pocisku w magazynku wroga

    for (int i=0;i<10;++i)
    {
        int randInd=rand()%amountofaliens; //wybor losowego strzelca do 10 prób.

        if (aliens[randInd].alive)
        {
            freeBullet->active=true;
            freeBullet->x=aliens[randInd].x;
            freeBullet->y=aliens[randInd].y+1;
            break;
        }
    }
}

void S_Inv::updateEnemyBullets()
{
    for(auto& b:enemiesBullets)
    {
        //nieaktywny-pomiń
        if(!b.active) continue;

        b.y++;

        if(b.y>=height)
        {
            b.active=false;
            continue;
        }

        //DETEKCJA KOLIZJI Z GRACZEM
        if(b.y==playerY && b.x==playerX)
        {
            b.active=false;
            gameOver=true;
            continue;
        }

        //DETEKCJA KOLIZJI Z TARCZAMI
        for(auto& shield:shields)
        {
            //Sprawdzamy istniejące fragmenty tarczy(health>0)
            if(shield.health>0)
            {
                if(b.x==shield.x && b.y==shield.y)
                {
                    //Trafiono tarczę
                    b.active=false;
                    shield.health--;

                    break;
                }
            }
        }
    }
}

void S_Inv::drawEnemyBulletsOnBoard()
{
    for(const auto& b:enemiesBullets)
    {
        if(b.active)
        {
            if (b.x>=0 && b.x<width && b.y>=0 && b.y<height)
            {
                si_board[b.y][b.x]=enemyBulletSymbol;
            }
        }
    }
}

int S_Inv::getPoints() const
{
    return points;
}

void S_Inv::bonusPoints(int bonus)
{
    points=points+bonus;
}
