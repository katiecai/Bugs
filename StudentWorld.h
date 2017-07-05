#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Compiler.h"
#include <string>
#include "Field.h"
#include <list>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Food;
class Insect;
class Anthill;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
        m_winningColony = -1;
        m_numWinningAnts = 5;
        m_ticks = 2000;
        for (int i = 0; i < m_numColonies; i++) {
            antsPerColony[i] = 0;
        }
        compilerForEntrant0 = nullptr;
        compilerForEntrant1 = nullptr;
        compilerForEntrant2 = nullptr;
        compilerForEntrant3 = nullptr;
	}
    
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void removeDead();
    
    //get
    Field getField();
    Actor* getActor(int x, int y);
    Food* getFoodAt(int x, int y);
    Insect* obtainRandomEnemy(int x, int y, Insect* ip);
    Insect* obtainAntEnemy(int x, int y, Insect* ip);
    int getTicks();
    
    //check
    bool isPebble(int x, int y);
    bool isThereFood(int x, int y);
    bool isDangerous(int x, int y, int colony);
    bool isTherePheromone(int x, int y);
    bool isThereEnemy(int x, int y, int colony);
    bool isMyAnthill(int x, int y, int colony);
    
    //other
    void addFood(int x, int y, int amount);
    void addPheromone(int x, int y, int colony);
    void birthAnt(int x, int y, int colonyNumber, Anthill* a, Compiler* c);
    void stunInsectsAt(int x, int y);
    void poisonInsectsAt(int x, int y);
    void addAdultGrasshopper(int x, int y);
    void addAntToColony(int colonyNumber);
    
    void updateDisplayText();
    
    int getWinningColony();
    void setWinningNumAnts();
    void setWinningColony(int col);
    
private:
    int m_ticks;
    int m_numWinningAnts;
    list<Actor*> m_actors[VIEW_HEIGHT][VIEW_WIDTH];
    int m_numColonies;
    int antsPerColony[4];
    vector<string> m_names;
    Field m_f;
    int m_winningColony;
    Compiler *compilerForEntrant0, *compilerForEntrant1,
    *compilerForEntrant2, *compilerForEntrant3;
    
};

#endif // STUDENTWORLD_H_
