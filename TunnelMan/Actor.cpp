#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>

using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

///////////////////////////////////////////////The Ultimate Base Class : Actor//////////////////////////////////////////////////////

Actor::Actor(StudentWorld *world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
    : GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), m_alive(true)
{
    setVisible(true);
}

Actor::~Actor()
{
    setVisible(false);
}

StudentWorld *Actor::getWorld()
{
    return m_world;
}

bool Actor::isAlive()
{
    return m_alive;
}

void Actor::die()
{
    m_alive = false;
}

///////////////////////////////////////////////Class : Earth /////////////////////////////////////////////////////////////////////////

Earth::Earth(StudentWorld *world, int startX, int startY)
    : Actor(world, TID_EARTH, startX, startY, right, 0.25, 3) {}

Earth::~Earth() {}

void Earth::doSomething() {}

///////////////////////////////////////////////Base Class For Characters : Man///////////////////////////////////////////////////////

Man::Man(StudentWorld *world, int imageID, int startX, int startY, Direction dir, int health)
    : Actor(world, imageID, startX, startY, dir, 1.0, 0), m_health(health) {}
int Man::getHealth()
{
    return m_health;
}

void Man::decHealth(int points)
{
    m_health -= points;
}

///////////////////////////////////////////////Base Class For Items That Characters Interact With///////////////////////////////////////

Items::Items(StudentWorld *world, int imageID, int startX, int startY)
    : Actor(world, imageID, startX, startY, right, 1.0, 2), m_ticks(0) {}

// Game Items can disappear
// Set tick to 0, keep ticking, and if it surpasses the lifetime of the object call kill the object and set visibility to false
void Items::disappear(int ticks)
{
    if (m_ticks == ticks)
    {
        die();
        setVisible(false);
    }
    else
    {
        m_ticks++;
    }
}

///////////////////////////////////////////////Base Class For Simple Items That Characters Interact With (Sonar, Waterpool) : Goodie//////

Goodie::Goodie(StudentWorld *world, int imageID, int startX, int startY)
    : Items(world, imageID, startX, startY) {}

void Goodie::doSomething()
{
    if (!isAlive())
        return;
    if (getWorld()->playerWithinRadius(this, 3))
    {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getTunnelMan()->addGoodieItems(this);
        return;
    }
    disappear(max(100, 300 - 10 * (int)getWorld()->getLevel()));
}

///////////////////////////////////////////////Class : Boulder////////////////////////////////////////////////////////////////////////////

Boulder::Boulder(StudentWorld *world, int startX, int startY)
    : Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1), m_stable(true), m_ticks(0)
{
    world->removeEarth(startX, startY);
}

void Boulder::annoy()
{
    if (getWorld()->playerWithinRadius(this, 3))
    {
        getWorld()->getTunnelMan()->isAnnoyed(100);
    }

    vector<Protester *> protesters = getWorld()->allProtesterInRadius(this, 3);
    for (size_t i = 0; i < protesters.size(); i++)
    {
        protesters[i]->isAnnoyed(100);
    }
}

void Boulder::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    else
    {
        if (m_stable)
        {

            if (getWorld()->earthUnderneath(getX(), getY()))
            {
                return;
            }
            else
            {
                m_stable = false;
            }
        }
        m_ticks++;
        if (m_ticks == 30)
        {
            m_falling = true;
            getWorld()->playSound(SOUND_FALLING_ROCK);
        }
        if (m_falling)
        {
            if (getY() == 0 || getWorld()->boulderExists(getX(), getY() - 4) || getWorld()->earthUnderneath(getX(), getY()))
            {
                die();
            }
            else
            {
                moveTo(getX(), getY() - 1);
                annoy();
            }
        }
    }
};

///////////////////////////////////////////////Class : Squirt////////////////////////////////////////////////////////////////////////////

Squirt::Squirt(StudentWorld *world, int startX, int startY, Direction dir)
    : Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1), m_travel(4) {}

void Squirt::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    if (squirtProtester() || m_travel <= 0)
    {
        die();
        return;
    }
    else
    {
        m_travel--;
        switch (getDirection())
        {
        case up:
            if (getWorld()->earthExists(getX(), getY() + 1) || getWorld()->boulderExists(getX(), getY() + 1) || getY() > 60)
            {
                die();
                return;
            }
            else
            {
                moveTo(getX(), getY() + 1);
            }
            break;
        case down:
            if (getWorld()->earthExists(getX(), getY() - 1) || getWorld()->boulderExists(getX(), getY() - 1) || getY() < 0)
            {
                die();
                return;
            }
            else
            {
                moveTo(getX(), getY() - 1);
            }
            break;
        case left:
            if (getWorld()->earthExists(getX() - 1, getY()) || getWorld()->boulderExists(getX() - 1, getY()) || getX() < 0)
            {
                die();
                return;
            }
            else
            {
                moveTo(getX() - 1, getY());
            }
            break;
        case right:
            if (getWorld()->earthExists(getX() + 1, getY()) || getWorld()->boulderExists(getX() + 1, getY()) || getX() > 60)
            {
                die();
                return;
            }
            else
            {
                moveTo(getX() + 1, getY());
            }
            break;
        case none:
            return;
        }
    }
}

bool Squirt::squirtProtester()
{
    vector<Protester *> protesters = getWorld()->allProtesterInRadius(this, 3);
    if (protesters.size() == 0)
    {
        return false;
    }
    else
    {
        protesters[0]->isAnnoyed(2);
        return true;
    }
}

///////////////////////////////////////////////Class : Barrel////////////////////////////////////////////////////////////////////////////

Barrel::Barrel(StudentWorld *world, int startX, int startY)
    : Items(world, TID_BARREL, startX, startY)
{
    setVisible(false);
}

void Barrel::doSomething()
{
    if (!isAlive())
        return;
    if (!isVisible() && getWorld()->playerWithinRadius(this, 4))
    {
        setVisible(true);
        return;
    }
    if (getWorld()->playerWithinRadius(this, 3))
    {
        die();
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->decBarrels();
        return;
    }
}

///////////////////////////////////////////////Class : Sonar////////////////////////////////////////////////////////////////////////////

Sonar::Sonar(StudentWorld *world, int startX, int startY)
    : Goodie(world, TID_SONAR, startX, startY)
{
}

///////////////////////////////////////////////Class : WaterPool////////////////////////////////////////////////////////////////////////

WaterPool::WaterPool(StudentWorld *world, int startX, int startY)
    : Goodie(world, TID_WATER_POOL, startX, startY)
{
}

///////////////////////////////////////////////Class : Nugget///////////////////////////////////////////////////////////////////////////

Nugget::Nugget(StudentWorld *world, int startX, int startY, bool dropped)
    : Items(world, TID_GOLD, startX, startY), dropped(dropped)
{
    if (dropped)
    {
        setVisible(true);
    }

    else
    {
        setVisible(false);
    }
}

void Nugget::doSomething()
{
    if (!isAlive())
        return;
    if (!isVisible() && getWorld()->playerWithinRadius(this, 4))
    {
        setVisible(true);
        return;
    }
    if (!dropped && getWorld()->playerWithinRadius(this, 3))
    {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(10);
        getWorld()->getTunnelMan()->addNuggets();
        return;
    }
    if (dropped)
    {
        vector<Protester *> protesters = getWorld()->allProtesterInRadius(this, 3);
        if (!protesters.empty())
        {
            Protester *ptr = protesters[0];
            if (ptr != nullptr)
            {
                die();
                getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
                getWorld()->increaseScore(25);
                ptr->bribed();
            }
        }
        disappear(100);
    }
}

///////////////////////////////////////////////Class : TunnelMan////////////////////////////////////////////////////////////////////

TunnelMan::TunnelMan(StudentWorld *world)
    : Man(world, TID_PLAYER, 30, 60, right, 10), m_water(5), m_sonar(1), m_gold(0) {}

TunnelMan::~TunnelMan() {}

void TunnelMan::doSomething()
{
    if (!isAlive())
    {
        return;
    }

    int ch;
    if (getWorld()->getKey(ch) == true)
    {
        switch (ch)
        {
        case KEY_PRESS_ESCAPE:
            getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
            die();
            break;
        case KEY_PRESS_SPACE:
            if (m_water > 0)
            {
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                m_water--;
                squirt();
            }
            break;
        case KEY_PRESS_UP:
            moveDirection(up);
            break;
        case KEY_PRESS_DOWN:
            moveDirection(down);
            break;
        case KEY_PRESS_LEFT:
            moveDirection(left);
            break;
        case KEY_PRESS_RIGHT:
            moveDirection(right);
            break;
        case 'z':
        case 'Z':
            if (m_sonar > 0)
            {
                m_sonar--;
                getWorld()->sonarActivate(getX(), getY());
                getWorld()->playSound(SOUND_SONAR);
            }
            break;
        case KEY_PRESS_TAB:
            if (m_gold > 0)
            {
                getWorld()->addActor(new Nugget(getWorld(), getX(), getY(), true));
                m_gold--;
            }
            break;
        }
    }
}

void TunnelMan::moveDirection(Direction direction)
{
    switch (direction)
    {
    case up:
        if (getDirection() == up)
        {
            if (getY() < 60 && !getWorld()->boulderExists(getX(), getY() + 1))
            {
                moveTo(getX(), getY() + 1);
                if (getWorld()->removeEarth(getX(), getY()))
                {
                    getWorld()->playSound(SOUND_DIG);
                }
            }
        }
        else
        {
            setDirection(up);
        }
        break;
    case down:
        if (getDirection() == down)
        {
            if (getY() > 0 && !getWorld()->boulderExists(getX(), getY() - 1))
            {
                moveTo(getX(), getY() - 1);
                if (getWorld()->removeEarth(getX(), getY()))
                {
                    getWorld()->playSound(SOUND_DIG);
                }
            }
        }
        else
        {
            setDirection(down);
        }
        break;
    case left:
        if (getDirection() == left)
        {
            if (getX() > 0 && !getWorld()->boulderExists(getX() - 1, getY()))
            {
                moveTo(getX() - 1, getY());
                if (getWorld()->removeEarth(getX(), getY()))
                {
                    getWorld()->playSound(SOUND_DIG);
                }
            }
        }
        else
        {
            setDirection(left);
        }
        break;
    case right:
        if (getDirection() == right)
        {
            if (getX() < 60 && !getWorld()->boulderExists(getX() + 1, getY()))
            {
                moveTo(getX() + 1, getY());
                if (getWorld()->removeEarth(getX(), getY()))
                {
                    getWorld()->playSound(SOUND_DIG);
                }
            }
        }
        else
        {
            setDirection(right);
        }
        break;
    case none:
        break;
    }
}

void TunnelMan::squirt()
{
    Direction TMdir = getDirection();
    switch (TMdir)
    {
    case up:
        if (!getWorld()->earthExists(getX(), getY() + 4) && !getWorld()->boulderExists(getX(), getY() + 4) && getY() + 4 <= 60)
        {
            getWorld()->addActor(new Squirt(getWorld(), getX(), getY() + 4, TMdir));
        }
        break;
    case down:
        if (!getWorld()->earthExists(getX(), getY() - 4) && !getWorld()->boulderExists(getX(), getY() - 4) && getY() - 4 >= 0)
        {
            getWorld()->addActor(new Squirt(getWorld(), getX(), getY() - 4, TMdir));
        }
        break;
    case left:
        if (!getWorld()->earthExists(getX() - 4, getY()) && !getWorld()->boulderExists(getX() - 4, getY()) && getX() - 4 >= 0)
        {
            getWorld()->addActor(new Squirt(getWorld(), getX() - 4, getY(), TMdir));
        }
        break;
    case right:
        if (!getWorld()->earthExists(getX() + 4, getY()) && !getWorld()->boulderExists(getX() + 4, getY()) && getX() + 4 <= 60)
        {
            getWorld()->addActor(new Squirt(getWorld(), getX() + 4, getY(), TMdir));
        }
        break;
    case none:
        break;
    }
}

void TunnelMan::isAnnoyed(int health)
{
    decHealth(health);
    if (getHealth() <= 0)
    {
        die();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

int TunnelMan::getWater()
{
    return m_water;
}

int TunnelMan::getSonar()
{
    return m_sonar;
}

void TunnelMan::addGoodieItems(Goodie *g)
{
    switch (g->getID())
    {
    case TID_SONAR:
        getWorld()->increaseScore(75);
        m_sonar += 2;
        break;
    case TID_WATER_POOL:
        getWorld()->increaseScore(100);
        m_water += 5;
        break;
    }
}

void TunnelMan::addNuggets()
{
    m_gold++;
}

int TunnelMan::getGold()
{
    return m_gold;
}

///////////////////////////////////////////////Class : Protester////////////////////////////////////////////////////////////////////////

Protester::Protester(StudentWorld *world, int imageID, int health)
    : Man(world, imageID, 60, 60, left, health), m_leave(false), m_lastPerpendicularTurn(200), m_sinceLastYell(15)
{
    m_numSquaresMove = rand() % 53 + 8;
    m_tickWait = max(0, 3 - (int)getWorld()->getLevel() / 4);
}

void Protester::doSomething()
{
    if (!isAlive())
    {
        return;
    }

    if (m_tickWait > 0)
    {
        m_tickWait--;
        return;
    }
    else
    {
        m_tickWait = max(0, 3 - (int)getWorld()->getLevel() / 4);
        m_sinceLastYell++;
        m_lastPerpendicularTurn++;
    }

    if (m_leave)
    {
        if (getX() == 60 && getY() == 60)
        {
            die();
            setVisible(false);
            getWorld()->removeProtester();
            return;
        }
        else
        {
            getWorld()->moveToExit(this);
            return;
        }
    }

    if (getWorld()->playerWithinRadius(this, 4) && isFacingTunnelMan())
    {
        if (m_sinceLastYell > 15)
        {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getTunnelMan()->isAnnoyed(2);
            m_sinceLastYell = 0;
            return;
        }
        return;
    }

    // ONLY FOR HARDCORE PROTESTOR - GO TO PLAYER IF LESS THAN M moves
    if (getID() == TID_HARD_CORE_PROTESTER)
    {
        int M = 16 + int(getWorld()->getLevel() * 2);
        Direction towardTunnelMan = getWorld()->senseTunnelMan(this, M);
        if (towardTunnelMan != none)
        {
            setDirection(towardTunnelMan);
            moveDirection(towardTunnelMan);
            return;
        }
    }

    if (dirToTunnelMan() != none && clearPath(dirToTunnelMan()) && !getWorld()->playerWithinRadius(this, 4))
    {
        setDirection(dirToTunnelMan());
        moveDirection(dirToTunnelMan());
        m_numSquaresMove = 0;
        return;
    }
    m_numSquaresMove--;
    if (m_numSquaresMove <= 0)
    {
        Direction dir = none;
        while (true)
        {
            dir = getRandomDir();
            if (getWorld()->validMove(getX(), getY(), dir))
            {
                break;
            }
        }

        setDirection(dir);
        m_numSquaresMove = rand() % 53 + 8;
    }
    else if (atIntersection() && m_lastPerpendicularTurn > 200)
    {
        viableTurn();
        m_numSquaresMove = rand() % 53 + 8;
        m_lastPerpendicularTurn = 0;
    }
    if (!getWorld()->validMove(getX(), getY(), getDirection()))
    {
        m_numSquaresMove = 0;
    }
    else
    {
        moveDirection(getDirection());
    }
}

void Protester::isAnnoyed(int amt)
{
    if (m_leave)
    {
        return;
    }

    decHealth(amt);
    if (getHealth() > 0)
    {
        m_tickWait = max(50, 100 - (int)getWorld()->getLevel() * 10);
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    }
    else
    {
        m_leave = true;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_tickWait = 0;
        if (amt == 100)
        {
            getWorld()->increaseScore(500);
        }
        else if (getID() == TID_PROTESTER)
        {
            getWorld()->increaseScore(100);
        }
        else
        {
            getWorld()->increaseScore(250);
        }
    }
}

int Protester::getTickWait()
{
    return m_tickWait;
}

bool Protester::getLeaveState()
{
    return m_leave;
}

void Protester::moveDirection(Direction dir)
{
    switch (dir)
    {
    case up:
        if (getDirection() == up)
        {
            if (getY() == 60)
                setDirection(down);
            else
                moveTo(getX(), getY() + 1);
        }
        else
            setDirection(up);
        break;
    case down:
        if (getDirection() == down)
        {
            if (getY() == 0)
                setDirection(up);
            else
                moveTo(getX(), getY() - 1);
        }
        else
            setDirection(down);
        break;
    case left:
        if (getDirection() == left)
        {
            if (getX() == 0)
                setDirection(right);
            else
                moveTo(getX() - 1, getY());
        }
        else
            setDirection(left);
        break;
    case right:
        if (getDirection() == right)
        {
            if (getX() == 60)
                setDirection(left);
            else
                moveTo(getX() + 1, getY());
        }
        else
            setDirection(right);
        break;
    case none:
        return;
    }
}

bool Protester::isFacingTunnelMan()
{
    Direction Protester = getDirection();
    if (Protester == left)
    {
        return getWorld()->getTunnelMan()->getX() <= getX();
    }
    if (Protester == right)
    {
        return getWorld()->getTunnelMan()->getX() >= getX();
    }
    if (Protester == up)
    {
        return getWorld()->getTunnelMan()->getY() >= getY();
    }
    if (Protester == down)
    {
        return getWorld()->getTunnelMan()->getY() <= getY();
    }
    if (Protester == none)
    {
        return false;
    }

    return false;
}

GraphObject::Direction Protester::dirToTunnelMan()
{
    int tunnelManX = getWorld()->getTunnelMan()->getX();
    int tunnelManY = getWorld()->getTunnelMan()->getY();

    if (getX() == tunnelManX && getY() == tunnelManY)
    {
        return getDirection();
    }

    if (getX() == tunnelManX)
    {
        if (getY() < tunnelManY)
        {
            return GraphObject::up;
        }
        else
        {
            return GraphObject::down;
        }
    }

    if (getY() == tunnelManY)
    {
        if (getX() < tunnelManX)
        {
            return GraphObject::right;
        }
        else
        {
            return GraphObject::left;
        }
    }
    return none;
}

bool Protester::clearPath(Direction dir)
{
    int TunnelManX = getWorld()->getTunnelMan()->getX();
    int TunnelManY = getWorld()->getTunnelMan()->getY();

    switch (dir)
    {
    case up:
        for (int i = getY(); i < TunnelManY; i++)
        {
            if (!getWorld()->validMove(getX(), i, dir))
            {
                return false;
            }
        }
        return true;
        break;

    case down:
        for (int i = getY(); i > TunnelManY; i--)
        {
            if (!getWorld()->validMove(getX(), i, dir))
            {
                return false;
            }
        }
        return true;
        break;

    case left:
        for (int i = getX(); i > TunnelManX; i--)
        {
            if (!getWorld()->validMove(i, getY(), dir))
            {
                return false;
            }
        }
        return true;
        break;

    case right:
        for (int i = getX(); i < TunnelManX; i++)
        {
            if (!getWorld()->validMove(i, getY(), dir))
            {
                return false;
            }
        }
        return true;
        break;

    case none:
        return false;
    }
    return false;
}

GraphObject::Direction Protester::getRandomDir()
{
    int dir = rand() % 4 + 1;
    switch (dir)
    {
    case 1:
        return GraphObject::up;
    case 2:
        return GraphObject::down;
    case 3:
        return GraphObject::left;
    case 4:
        return GraphObject::right;
    }
    return none;
}

bool Protester::atIntersection()
{
    if (getDirection() == left || getDirection() == right)
    {
        return (getWorld()->validMove(getX(), getY(), up) || getWorld()->validMove(getX(), getY(), down));
    }
    else
    {
        return (getWorld()->validMove(getX(), getY(), left) || getWorld()->validMove(getX(), getY(), right));
    }
}

void Protester::viableTurn()
{
    if (getDirection() == up || getDirection() == down)
    {
        if (!getWorld()->validMove(getX(), getY(), left))
            setDirection(right);
        else if (!getWorld()->validMove(getX(), getY(), right))
            setDirection(left);
        else
        {
            switch (rand() % 2)
            {
            case 0:
                setDirection(left);
            case 1:
                setDirection(right);
            }
        }
    }
    else
    {
        if (!getWorld()->validMove(getX(), getY(), up))
            setDirection(down);
        else if (!getWorld()->validMove(getX(), getY(), down))
            setDirection(up);
        else
        {
            switch (rand() % 2)
            {
            case 0:
                setDirection(up);
            case 1:
                setDirection(down);
            }
        }
    }
}

void Protester::bribed()
{
    if (getID() == TID_PROTESTER)
    {
        m_leave = true;
    }
    else
    {
        getWorld()->increaseScore(25);
        m_tickWait = max(50, 100 - int(getWorld()->getLevel()) * 10);
    }
}

RegularProtester::RegularProtester(StudentWorld *world)
    : Protester(world, TID_PROTESTER, 5) {}

HardCoreProtester::HardCoreProtester(StudentWorld *world)
    : Protester(world, TID_HARD_CORE_PROTESTER, 20) {}