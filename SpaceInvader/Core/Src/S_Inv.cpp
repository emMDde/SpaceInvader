#include <stdio.h>
#include <S_Inv.hh>
#include <cstdlib>

void S_Inv::drawShields()
{
    for (const Shield& shield : shields)
    {
        if(shield.health==0) continue;
        drawShield(shield.x, shield.y, shield.health);
    }
}

void S_Inv::drawEnemies()
{
    for(const Enemy& enemy : enemies)
    {
        if(enemy.alive) drawEnemy(enemy.x+0.5f, enemy.y);
    }
}

void S_Inv::drawPlayerBullets()
{
    for(const Bullet& b : playerBullets)
    {
        if(b.active) drawBullet(b.x, b.y);
    }
}

void S_Inv::drawEnemyBullets()
{
    for(const Bullet& b : enemiesBullets)
    {
        if(b.active) drawEnemyBullet(b.x, b.y);
    }
}

void S_Inv::moveEnemiesRight()
{
    for (Enemy& enemy : enemies)
    {
        if(enemy.alive) enemy.x+=enemySpeed*time_step;
    }
}

void S_Inv::moveEnemiesLeft()
{
    for (Enemy& enemy : enemies)
    {
        if(enemy.alive) enemy.x-=enemySpeed*time_step;
    }
}

void S_Inv::moveEnemiesDown()
{
    for (Enemy& enemy : enemies)
    {
        if(enemy.alive) enemy.y+=ENEMY_HEIGHT/4;
    }
}

void S_Inv::updateEnemiesPositions()
{
    bool wallHit=false;

    for(const Enemy& enemy : enemies)
    {
        if(!enemy.alive) continue;

        //Sprawdź prawą ścianę, jeśli poruszamy się w prawo
        if(enemyDir==DIR_RIGHT && enemy.x>=(width-1)) wallHit=true;

        //Sprawdź lewą ścianę, jeśli poruszamy się w lewo
        if(enemyDir==DIR_LEFT && enemy.x<=ENEMY_WIDTH) wallHit=true;

        //if(wallHit) break; //Wystarczy, ze jeden uderzy
        for(Shield& shield : shields)
        {
        	if(shield.health==0) continue;
        	if(shield.x-SHIELD_WIDTH<=enemy.x && shield.x>=enemy.x-ENEMY_WIDTH && shield.y<=enemy.y+ENEMY_HEIGHT)
        	{
        		shield.health=0;
        	}
        }

        if( (playerX-PLAYER_WIDTH<=enemy.x && playerX>=enemy.x-ENEMY_WIDTH && playerY<=enemy.y+ENEMY_HEIGHT) || enemy.y+ENEMY_HEIGHT+5>=height)
        {
        	gameOver=true;
        	return;
        }
    }

    if(wallHit)
    {
        moveEnemiesDown();
        if(enemyDir==DIR_RIGHT) enemyDir=DIR_LEFT;
        else enemyDir=DIR_RIGHT;
    }
    else
    {
        if(enemyDir==DIR_RIGHT) moveEnemiesRight();
        else moveEnemiesLeft();
    }
}

void S_Inv::enemyShoot()
{
    Bullet *freeBullet=nullptr;
    for(Bullet& b : enemiesBullets)
    {
        if (!b.active)
        {
            freeBullet=&b;
            break;
        }
    }
    if(freeBullet==nullptr) return; //brak wolnego pocisku w magazynku wroga

    for(int i=0; i<20; ++i)
    {
        int randInd=rand()%amountofenemys; //wybor losowego strzelca do 20 prób.

        if(!enemies[randInd].alive || isEnemyUnder(randInd)) continue;
        freeBullet->active=true;
        freeBullet->x=enemies[randInd].x-ENEMY_WIDTH/2+BULLET_WIDTH/2-1;
        freeBullet->y=enemies[randInd].y+ENEMY_HEIGHT;
        break;
    }
}

void S_Inv::updateEnemyBullets()
{
    for(Bullet& b : enemiesBullets)
    {
        //nieaktywny-pomiń
        if(!b.active) continue;

        b.y+=enemyBulletSpeed*time_step;

        if(b.y+BULLET_HEIGHT>=(height-1))
        {
            b.active=false;
            continue;
        }

        //DETEKCJA KOLIZJI Z GRACZEM
        if(b.x-BULLET_WIDTH/2<=playerX && b.x-BULLET_WIDTH/2>=playerX-PLAYER_WIDTH && b.y+BULLET_HEIGHT>=playerY+3 && b.y+BULLET_HEIGHT<=playerY+PLAYER_HEIGHT)
        {
            b.active=false;
            gameOver=true;
            break;
        }

        //DETEKCJA KOLIZJI Z TARCZAMI
        for(Shield& shield : shields)
        {
            //Sprawdzamy istniejące fragmenty tarczy(health>0)
            if(shield.health==0) continue;
            if(b.x-BULLET_WIDTH+2<=shield.x && b.x-2>=shield.x-SHIELD_WIDTH  && b.y+BULLET_HEIGHT>=shield.y && b.y+BULLET_HEIGHT<=shield.y+SHIELD_HEIGHT)
            {
            	//Trafiono tarczę
                b.active=false;
                shield.health--;
                break;
            }
        }
    }
}

void S_Inv::movePlayerLeft()
{
    if(playerX>PLAYER_WIDTH) playerX-=playerSpeed*time_step;
    if(playerX<PLAYER_WIDTH) playerX=PLAYER_WIDTH;
}

void S_Inv::movePlayerRight()
{
    if(playerX<(width-1)) playerX+=playerSpeed*time_step;
    if(playerX>(width-1)) playerX=width-1;
}

void S_Inv::playerShoot()
{
    // Szukamy pierwszego pocisku, który NIE jest aktywny
    for(Bullet& b : playerBullets)
    {
        if(!b.active)
        {
            b.active=true;
            b.x=playerX-PLAYER_WIDTH/2+BULLET_WIDTH/2-1;
            b.y=playerY-BULLET_HEIGHT;
            break;
        }
    }
}

void S_Inv::updatePlayerBullets()
{
    for(Bullet& b : playerBullets)
    {
        //nieaktywny-pomiń
        if(!b.active) continue;

        b.y-=playerBulletSpeed*time_step;

        if(b.y<=0)
        {
            b.active=false;
            continue;
        }

        //DETEKCJA KOLIZJI Z WROGAMI
        for(Enemy& enemy : enemies)
        {
            if(!enemy.alive) continue;
            if(b.x-BULLET_WIDTH/2<=enemy.x && b.x-BULLET_WIDTH/2>=enemy.x-ENEMY_WIDTH && b.y>=enemy.y && b.y<=enemy.y+ENEMY_HEIGHT)
            {
            	enemy.alive=false;
                b.active=false;
                updatePoints(10);
                checkIfWon();
                break;
            }
        }

        if(!b.active) continue;

        //DETEKCJA KOLIZJI Z TARCZAMI
        for(Shield& shield : shields)
        {
            //Sprawdzamy istniejące fragmenty tarczy(health>0)
            if(shield.health==0) continue;
            if(b.x-BULLET_WIDTH+2<=shield.x && b.x-2>=shield.x-SHIELD_WIDTH  && b.y>=shield.y && b.y<=shield.y+SHIELD_HEIGHT)
            {
            	//Trafiono tarczę własnym pociskiem
            	b.active=false; //Pocisk znika
            	if(!hardMode) break;
                shield.health--; //zakomentowane - nie niszczy tarczy pocisk gracza
                if(shield.health==0) updatePoints(-5);
                break;
            }
        }

        if(!b.active) continue;

        for(Bullet& enemyB : enemiesBullets)
        {
        	if(!enemyB.active) continue;
            if(b.x-BULLET_WIDTH/2-3<=enemyB.x && b.x-BULLET_WIDTH/2+3>=enemyB.x-BULLET_WIDTH && b.y>=enemyB.y && b.y<=enemyB.y+BULLET_HEIGHT)
            {
            	enemyB.active=false;
                b.active=false;
                updatePoints(1);
                break;
            }
        }
    }
}

void S_Inv::updatePoints(int bonus)
{
    if(!hardMode) points=points+bonus;
    else points=points+bonus*1.4;
}

void S_Inv::checkIfWon()
{
    isWon=true;
    for(const Enemy& enemy : enemies)
    {
    	if(enemy.alive) isWon=false;
    }
    gameOver=isWon;
}

bool S_Inv::isEnemyUnder(int shooter)
{
    for(const Enemy& enemy : enemies)
    {
    	if(!enemy.alive) continue;
    	if(enemy.y==enemies[shooter].y+10+ENEMY_HEIGHT && enemy.x==enemies[shooter].x) return true;
    }
    return false;
}

S_Inv::S_Inv(bool isHard)
{
	hardMode=isHard;
	if(hardMode)
	{
	    playerShootCooldown=1.6f;
	    enemyShootCooldown=1.3f;
	    enemySpeed=55;
	    enemyBulletSpeed=95;
	}
	else
	{
	    playerShootCooldown=1.3f;
	    enemyShootCooldown=1.8f;
	}

    //Ustawienie statkow przeciwnika
    const int startX=5;
    const int startY=10;
    const int xSpacing=(width-2*startX-enemysperrow*ENEMY_WIDTH)/(enemysperrow+1);
    const int ySpacing=10+ENEMY_HEIGHT;
    int ind=0;

    for(int row=0;row<rowswithenemys;++row)
    {
        for(int col=0;col<enemysperrow;++col)
        {
            if(ind==amountofenemys) break;

            int currentX=startX+(col+1)*(xSpacing+ENEMY_WIDTH);
            int currentY=startY+row*ySpacing;

            enemies[ind].x=currentX;
            enemies[ind].y=currentY;
            enemies[ind].alive=true;

            ind++;
        }
        if(ind==amountofenemys) break;
    }

    //Ustawienie Tarcz nad graczem
    const int shieldX=10;
    const int shieldY=height-2*PLAYER_HEIGHT;
    const int shieldSpacingX=(width-2*shieldX-SHIELD_WIDTH*amountofshields)/(amountofshields+1);

    for(int s=0;s<amountofshields;++s)
    {
    	int currentX=shieldX+(s+1)*(shieldSpacingX+SHIELD_WIDTH);
        shields[s].x=currentX;
        shields[s].y=shieldY;
        shields[s].health=3;
    }

    //Ustawinie pociskow na niekatywne
    for(Bullet& b : playerBullets)
    {
        b.active=false; //Początkowo żaden pocisk nie leci
        b.x=0;
        b.y=0;
    }

    for(Bullet& b : enemiesBullets)
    {
        b.active=false;//Początkowo żaden pocisk nie leci
        b.x=0;
        b.y=0;
    }
}

void S_Inv::updatePhysics(float dt, bool moveLeft, bool moveRight)
{
	time_step=dt;
	updateEnemiesPositions();
	if(moveLeft) movePlayerLeft();
	if(moveRight) movePlayerRight();

	playerReloadTimer+=dt;
	if(playerReloadTimer>=playerShootCooldown)
	{
		playerShoot();
		playerReloadTimer-=playerShootCooldown;
	}
	enemyReloadTimer+=dt;
	if(enemyReloadTimer>=enemyShootCooldown)
	{
		enemyShoot();
		enemyReloadTimer-=enemyShootCooldown;
	}

	updatePlayerBullets();
	updateEnemyBullets();
}

void S_Inv::renderGame()
{
	setBackgroundColor(0xFF000000);
	drawShields();
    drawEnemyBullets();
    drawPlayerBullets();
	drawPlayer(playerX, playerY);
	drawEnemies();
}

int S_Inv::getPoints() const
{
    return points>=0 ? points : 0;
}

bool S_Inv::isOver() const
{
	return gameOver;
}

bool S_Inv::getResult() const
{
	return isWon;
}
