#include "StudentWorld.h"
#include "GraphObject.h"
#include <string>
#include <algorithm>
#include <queue>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld *createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
StudentWorld::StudentWorld(string assetDir) : GameWorld(assetDir)
{
}

StudentWorld::~StudentWorld()
{
}

int StudentWorld::init()
{
	// Resets the member vars
	m_barrels = 0;
	m_nuggets = 0;
	m_Protesters = 0;
	m_firstTick = true;
	m_tickCount = 0;
	for (int i = 0; i < 60; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			m_earth[i][j] = new Earth(this, j, i);
			if (i > 3 && i < 60 && j > 29 && j < 34) // sets invisible the center tunnel
			{
				m_earth[i][j]->setVisible(false);
			}
		}
	}

	int B = min((int)getLevel() / 2 + 2, 9); // number of boulders
	int G = max(5 - (int)getLevel() / 2, 2); // number of gold nuggets
	int L = min(2 + (int)getLevel(), 21);	 // number of barrels

	// One time addition of game objects at the start of the game (barrel, gold nuggets, boulders)
	addBoulder(B);
	addNugBar(G, 'N');
	addNugBar(L, 'B');

	m_player = new TunnelMan(this); // adds tunnelman to the game

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	addProtester();
	addGoodies();

	textFormatter(); // update the status text

	// iterate through the actor vector and if an actor is alive do something
	// if player is dead decrease lives
	// barrels left in the oil field become 0 then finish level
	vector<Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->isAlive())
		{
			(*it)->doSomething();
		}
		if (!m_player->isAlive())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		if (m_barrels == 0)
		{
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
	}
	m_player->doSomething();

	// delete dead objects
	for (it = m_actors.begin(); it != m_actors.end();)
	{
		if (!(*it)->isAlive())
		{
			delete *it;
			it = m_actors.erase(it);
		}
		else
			it++;
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	// delete earth objects
	for (int row = 0; row < 60; row++)
	{
		for (int col = 0; col < 64; col++)
		{
			if (m_earth[row][col] != nullptr)
			{
				delete m_earth[row][col];
				m_earth[row][col] = nullptr;
			}
		}
	}

	// iterate through the actor vector and delete all actor objects
	vector<Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		delete *it;
	}

	// clear the m_actors vector from the actor pointers
	m_actors.clear();

	// delete tunnelman object
	delete m_player;
}

void StudentWorld::textFormatter()
{
	// get all the stats
	int level = getLevel();
	int lives = getLives();
	int health = m_player->getHealth();
	int water = m_player->getWater();
	int gold = m_player->getGold();
	int sonar = m_player->getSonar();
	int score = getScore();

	// format them into a string
	stringstream s;

	s.fill(' ');
	s << "Lvl: ";
	s << setw(2) << level;

	s << " Lives: ";
	s << setw(1) << lives;

	s << " Hlth: ";
	s << setw(3) << health * 10;
	s << '%';

	s << " Wtr: ";
	s << setw(2) << water;

	s << " Gld: ";
	s << setw(2) << gold;

	s << " Oil Left: ";
	s << setw(2) << m_barrels;

	s << " Sonar: ";
	s << setw(2) << sonar;

	s.fill('0');
	s << " Scr: ";
	s << setw(6) << score;

	// setGameText to the formatted string
	setGameStatText(s.str());
}

bool StudentWorld::removeEarth(int x, int y)
{
	// if the earth is visible 4x4 right and up from the coordinate of the player set it to invisible and return true
	int row = y;
	int col = x;

	bool removed = false;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (row + i > 59 || col + j > 63 || !m_earth[row + i][col + j]->isVisible())
			{
				continue;
			}
			else
			{
				m_earth[row + i][col + j]->setVisible(false);
				removed = true;
			}
		}
	}
	return removed;
}

bool StudentWorld::earthUnderneath(int x, int y)
{
	// if the earth is visible 1x4 one down from the coordinate of the player return true
	y--;
	for (int i = 0; i < 4; i++)
	{
		if (m_earth[y][x + i]->isVisible())
		{
			return true;
		}
	}
	return false;
}

bool StudentWorld::earthExists(int x, int y)
{
	// if the earth is visible 4x4 right and up from the coordinate of the player return true
	bool exists = false;

	for (int i = y; i < y + 4 && i >= 0 && i < 60; i++)
	{
		for (int j = x; j < x + 4 && j >= 0 && j < 64; j++)
		{
			if (m_earth[i][j]->isVisible())
			{
				exists = true;
			}
		}
	}

	return exists;
}

bool StudentWorld::boulderExists(int x, int y)
{
	// Iterate through m_actors vector and find the boulder object
	vector<Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->getID() == TID_BOULDER)
		{
			// if any of the pixels between a game character and boulder overlap -3 to +3 horizontally and vertically return true
			for (int i = -3; i < 4; i++)
			{
				for (int j = -3; j < 4; j++)
				{
					if ((*it)->getX() + j == x && (*it)->getY() + i == y)
					{
						return true;
					}
				}
			}

			for (int i = -3; i < 4; i++)
			{
				for (int j = -3; j < 4; j++)
				{
					if (x + j == (*it)->getX() && y + i == (*it)->getY())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool StudentWorld::withinRadius(int x, int y, int x2, int y2, int radius)
{
	// euclidian distance
	return (x - x2) * (x - x2) + (y - y2) * (y - y2) <= radius * radius;
}

bool StudentWorld::playerWithinRadius(Actor *a, int radius)
{
	// use withinradius to see if tunnelman is within radius
	return withinRadius(a->getX(), a->getY(), m_player->getX(), m_player->getY(), radius);
}

vector<Protester *> StudentWorld::allProtesterInRadius(Actor *actor, int radius)
{
	// iterate through the actor vector if the image id is protester or hardcore protester and if it is within radius of the actor
	// passed in return the pointer to protester
	vector<Protester *> p;
	vector<Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER)
		{
			if (withinRadius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY(), radius))
			{
				Protester *ptr = dynamic_cast<Protester *>(*it);
				if (ptr->getTickWait() > 3 || ptr -> getLeaveState())
					continue;
				else
					p.push_back(ptr);
			}
		}
	}

	return p;
}

void StudentWorld::sonarActivate(int x, int y)
{
	// iterate through the actor vector if the image id is barrel or gold and within radius of 12 of x and y coordinate set it visible
	vector<Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->getID() == TID_BARREL || (*it)->getID() == TID_GOLD)
		{
			if (withinRadius(x, y, (*it)->getX(), (*it)->getY(), 12))
			{
				(*it)->setVisible(true);
			}
		}
	}
}

bool StudentWorld::objectsWithinRadius(int x, int y, int radius)
{
	// If object is within radius of x and y return true else false (used for randomly distributing game items in the oil field)
	vector<Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if (withinRadius((*it)->getX(), (*it)->getY(), x, y, radius))
			return true;
	}
	return false;
}

bool StudentWorld::validMove(int x, int y, GraphObject::Direction dir)
{
	// Checks boundaries and checks that there are no earth or boulders  for each direction
	switch (dir)
	{
	case GraphObject::up:
		return (y != 60 && !earthExists(x, y + 1) && !boulderExists(x, y + 1));
	case GraphObject::down:
		return (y != 0 && !earthExists(x, y - 1) && !boulderExists(x, y - 1));
	case GraphObject::left:
		return (x != 0 && !earthExists(x - 1, y) && !boulderExists(x, y));
	case GraphObject::right:
		return (x != 60 && !earthExists(x + 1, y) && !boulderExists(x + 1, y));
	case GraphObject::none:
		return false;
	}
	return false;
}

void StudentWorld::moveToExit(Protester *Protester)
{
	// using BFS (queue) to find the exit: Referenced https://stackoverflow.com/a/65601593/17621095
	// Code explained in report
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			maze[i][j] = 0;
		}
	}

	queue<Coord> q;
	q.push(Coord(60, 60));
	maze[60][60] = 1;
	while (!q.empty())
	{
		Coord c = q.front();
		q.pop();
		int x = c.x;
		int y = c.y;

		if (Protester->getX() == 60 && Protester->getY() == 60)
		{
			break;
		}
		// Try going up
		if (validMove(x, y, GraphObject::up) && maze[x][y + 1] == 0)
		{
			q.push(Coord(x, y + 1));
			maze[x][y + 1] = maze[x][y] + 1;
		}
		// Try going down
		if (validMove(x, y, GraphObject::down) && maze[x][y - 1] == 0)
		{
			q.push(Coord(x, y - 1));
			maze[x][y - 1] = maze[x][y] + 1;
		}

		// Try going left
		if (validMove(x, y, GraphObject::left) && maze[x - 1][y] == 0)
		{
			q.push(Coord(x - 1, y));
			maze[x - 1][y] = maze[x][y] + 1;
		}
		// Try going right
		if (validMove(x, y, GraphObject::right) && maze[x + 1][y] == 0)
		{
			q.push(Coord(x + 1, y));
			maze[x + 1][y] = maze[x][y] + 1;
		}
	}

	int a = Protester->getX();
	int b = Protester->getY();

	// We mapped out the most optimal path on the 2D array for maze now the protester just needs to take steps in wherever direction it is
	// one less than the step count it is currently on
	if (validMove(a, b, GraphObject::up) && maze[a][b + 1] ==  maze[a][b] - 1)
	{
		Protester->moveDirection(GraphObject::up);
	}
	else if (validMove(a, b, GraphObject::down) && maze[a][b - 1] == maze[a][b] - 1)
	{
		Protester->moveDirection(GraphObject::down);
	}
	else if (validMove(a, b, GraphObject::left) && maze[a - 1][b] == maze[a][b] - 1)
	{
		Protester->moveDirection(GraphObject::left);
	}
	else if (validMove(a, b, GraphObject::right) && maze[a + 1][b] == maze[a][b] - 1)
	{
		Protester->moveDirection(GraphObject::right);
	}

	return;
}

GraphObject::Direction StudentWorld::senseTunnelMan(Protester *Protester, int M)
{
	// Using moveToExit to find shortest path to TunnelMan not Coordinate (60,60)
	// Move only when M is more than the move count for shortest path to TunnelMan - written down on our maze 2D array
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			maze[i][j] = 0;
		}
	}

	queue<Coord> q;
	q.push(Coord(getTunnelMan()->getX(), getTunnelMan()->getY()));
	maze[getTunnelMan()->getX()][getTunnelMan()->getY()] = 1;
	while (!q.empty())
	{
		Coord c = q.front();
		q.pop();
		int x = c.x;
		int y = c.y;

		if (Protester->getX() == 60 && Protester->getY() == 60)
		{
			break;
		}

		// Try going up
		if (validMove(x, y, GraphObject::up) && maze[x][y + 1] == 0)
		{
			q.push(Coord(x, y + 1));
			maze[x][y + 1] = maze[x][y] + 1;
		}
		// Try going down
		if (validMove(x, y, GraphObject::down) && maze[x][y - 1] == 0)
		{
			q.push(Coord(x, y - 1));
			maze[x][y - 1] = maze[x][y] + 1;
		}
		// Try going left
		if (validMove(x, y, GraphObject::left) && maze[x - 1][y] == 0)
		{
			q.push(Coord(x - 1, y));
			maze[x - 1][y] = maze[x][y] + 1;
		}
		// Try going right
		if (validMove(x, y, GraphObject::right) && maze[x + 1][y] == 0)
		{
			q.push(Coord(x + 1, y));
			maze[x + 1][y] = maze[x][y] + 1;
		}
	}

	int a = Protester->getX();
	int b = Protester->getY();
	// If the calculated move count is less than the protester's ability to sense tunnelman return a direction
	// that can lead the protester one step closer to tunnelman
	if (maze[a][b] <= M + 1)
	{
		if (validMove(a, b, GraphObject::up) && maze[a][b + 1] == maze[a][b] - 1)
		{
			return GraphObject::up;
		}
		else if (validMove(a, b, GraphObject::down) && maze[a][b - 1] == maze[a][b] - 1)
		{
			return GraphObject::down;
		}
		else if (validMove(a, b, GraphObject::left) && maze[a - 1][b] == maze[a][b] - 1)
		{
			return GraphObject::left;
		}
		else if (validMove(a, b, GraphObject::right) && maze[a + 1][b] == maze[a][b] - 1)
		{
			return GraphObject::right;
		}
	}
	return GraphObject::none;
}

void StudentWorld::addActor(Actor *a)
{
	m_actors.push_back(a);
}

void StudentWorld::addBoulder(int num)
{
	int filled = 0;

	// keep generating random col, and row that meets conditions given in spec
	// Add boulders when there are no objects 6 radius from it and covered by at least one pixel of earth
	// Do this until its filled with the correct amount for the according level
	while (filled < num)
	{
		int col = rand() % 60 + 1;
		int row = rand() % 36 + 1 + 20;

		if (!objectsWithinRadius(col, row, 6) && ((col > 0 && col < 26) || (col > 34 && col < 60)) && row < 56)
		{
			addActor(new Boulder(this, col, row));
			filled++;
		}
	}
}

void StudentWorld::addProtester()
{
	int T = max(25, 200 - (int)getLevel());
	int P = fmin(15, 2 + getLevel() * 1.5);
	int probabilityOfHardcore = min(90, (int)getLevel() * 10 + 30);
	if (m_firstTick || (m_tickCount > T && m_Protesters < P)) // First protester must be added during the first tick or added after at least T counts have passed when there are less than P protesters
	{
		if (rand() % 100 + 1 <= probabilityOfHardcore) // Within prob of hardcore add hardcore protester
		{
			addActor(new HardCoreProtester(this));
		}

		else
		{
			addActor(new RegularProtester(this)); // not within prob of hardcore therefore add regular protester
		}
		m_firstTick = false; // its no longer the first tick
		m_tickCount = 0;	 // reset tick count as we added a protester
		m_Protesters++;		 // increment number of protester by 1 as we have added a protester
	}
	m_tickCount++; // Keep ticking the clock
}

void StudentWorld::addGoodies()
{
	int G = (int)getLevel() * 25 + 300;
	if (int(rand() % G) + 1 == 1) // 1 in G chance that a new waterpool or sonar kit will be added
	{
		if (int(rand() % 5) + 1 == 1) // 1/5 prob of a sonar being added
		{
			addActor(new Sonar(this, 0, 60));
		}
		else // 4/5 prob of waterpool being added
		{
			int row, col = 0;
			do
			{
				col = rand() % 60 + 1; // somewhere in the boundary of the oil field or on top
				row = rand() % 60 + 1;
			} while (earthExists(col, row) || objectsWithinRadius(col, row, 6)); // keep generating random col and row until the coordinate we want to place water pool has no earth and no objects within 6 radius
			addActor(new WaterPool(this, col, row));
		}
	}
}
void StudentWorld::addNugBar(int num, char type)
{
	int filled = 0;
	while (filled < num)
	{
		// Add within boundary of oil field inside the oil field only
		int col = rand() % 60 + 1;
		int row = rand() % 56 + 1;

		// Make sure there are no other game objects within 6 radius and not in the tunnel
		if (!objectsWithinRadius(col, row, 6) && (col < 26 || col > 34))
		{
			// either adds a barrel or a gold nugget
			switch (type)
			{
			case 'B':
				addActor(new Barrel(this, col, row));
				m_barrels++;
				filled++;
				break;
			case 'N':
				addActor(new Nugget(this, col, row, false));
				filled++;
				break;
			}
		}
	}
}

void StudentWorld::decBarrels()
{
	m_barrels--;
}

TunnelMan *StudentWorld::getTunnelMan()
{
	return m_player;
}

void StudentWorld::removeProtester()
{
	m_Protesters--;
}
