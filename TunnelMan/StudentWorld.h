#ifndef STUDENTWORLD_H
#define STUDENTWORLD_H

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include "Actor.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class GraphObject;
class Earth;
class Actor;
class TunnelMan;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void textFormatter();

	bool removeEarth(int x, int y);
	bool earthUnderneath(int x, int y);
	bool earthExists(int x, int y);
	bool boulderExists(int x, int y);

	bool withinRadius(int x, int y, int x2, int y2, int radius);
	bool playerWithinRadius(Actor *actor, int radius);
	std::vector<Protester*> allProtesterInRadius(Actor *actor, int radius);
	void sonarActivate(int x, int y);
	bool objectsWithinRadius(int x, int y, int radius);

	bool validMove(int x, int y, GraphObject::Direction dir);
	void moveToExit(Protester *Protester);
	GraphObject::Direction senseTunnelMan(Protester *ptr, int M);

	void addActor(Actor *a);
	void addBoulder(int num);
	void addProtester();
	void addGoodies();
	void addNugBar(int num, char type);
	void decBarrels();
	TunnelMan *getTunnelMan();
	void removeProtester();

private:
	int m_Protesters;
	int m_barrels;
	int m_nuggets;
	int m_sonar;
	bool m_firstTick;
	int m_tickCount;

	Earth *m_earth[64][64] = {nullptr};
	TunnelMan *m_player;
	std::vector<Actor *> m_actors;

	int maze[64][64];

	struct Coord
	{
		int x;
		int y;
		Coord(int x, int y) : x(x), y(y) {}
	};
};

#endif // STUDENTWORLD_H
