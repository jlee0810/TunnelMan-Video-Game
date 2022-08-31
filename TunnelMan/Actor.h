#ifndef ACTOR_H
#define ACTOR_H

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

//-------------------------------------------base class---------------------------------------------------------------------------//
class Actor : public GraphObject
{
public:
    Actor(StudentWorld *world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
    virtual ~Actor();
    StudentWorld *getWorld();
    bool isAlive();
    void die();
    virtual void doSomething() = 0;
    virtual void annoyed(int health) {}

private:
    StudentWorld *m_world;
    bool m_alive;
};

class Items : public Actor
{
public:
    Items(StudentWorld *world, int imageID, int startX, int startY);
    virtual void disappear(int ticks);

private:
    int m_ticks;
};

class Goodie : public Items
{
public:
    Goodie(StudentWorld *world, int imageID, int startX, int startY);
    virtual void doSomething();
};

class Man : public Actor
{
public:
    Man(StudentWorld *world, int imageID, int startX, int startY, Direction dir, int health);
    int getHealth();
    void decHealth(int points);
    virtual void moveDirection(Direction direction) = 0;
    virtual void isAnnoyed(int health) = 0;

private:
    int m_health;
};

//-------------------------------------------Trivial classes-----------------------------------------------------------------------//
class Earth : public Actor
{
public:
    Earth(StudentWorld *world, int startX, int startY);
    virtual ~Earth();
    virtual void doSomething();
};

class Boulder : public Actor
{
public:
    Boulder(StudentWorld *world, int startX, int startY);
    virtual void doSomething();
    void annoy();

private:
    bool m_stable;
    bool m_falling;
    int m_ticks;
};

class Squirt : public Actor
{
public:
    Squirt(StudentWorld *world, int startX, int startY, Direction dir);
    virtual void doSomething();
    bool squirtProtester();

private:
    int m_travel;
};

//-------------------------------------------Items------------------------------------------------------------------------//
class Barrel : public Items
{
public:
    Barrel(StudentWorld *world, int startX, int startY);
    virtual void doSomething();
};

class Sonar : public Goodie
{
public:
    Sonar(StudentWorld *world, int startX, int startY);
};

class WaterPool : public Goodie
{
public:
    WaterPool(StudentWorld *world, int startX, int startY);
};

class Nugget : public Items
{
public:
    Nugget(StudentWorld *world, int startX, int startY, bool dropped);
    virtual void doSomething();

private:
    bool dropped;
};

//-------------------------------------------People classes--------------------------------------------------------------------//
class TunnelMan : public Man
{
public:
    TunnelMan(StudentWorld *world);
    virtual ~TunnelMan();
    int getWater();
    int getGold();
    int getSonar();
    void addNuggets();
    void addGoodieItems(Goodie *g);
    void doSomething();
    void squirt();
    void moveDirection(Direction direction);
    void isAnnoyed(int health);

private:
    int m_water;
    int m_gold;
    int m_sonar;
};

class Protester : public Man
{
public:
    Protester(StudentWorld *world, int imageID, int health);
    virtual void doSomething();
    bool isFacingTunnelMan();
    Direction dirToTunnelMan();
    bool clearPath(Direction dir);
    Direction getRandomDir();
    bool atIntersection();
    void viableTurn();
    void moveDirection(Direction direction);
    void isAnnoyed(int health);
    int getTickWait();
    bool getLeaveState();
    void bribed();


private:
    bool m_leave;
    int m_tickWait;
    int m_sinceLastYell;
    int m_lastPerpendicularTurn;
    int m_numSquaresMove;
    
};

class RegularProtester : public Protester
{
public:
    RegularProtester(StudentWorld *world);
};

class HardCoreProtester : public Protester
{
public:
    HardCoreProtester(StudentWorld *world);
};

#endif // ACTOR_H
