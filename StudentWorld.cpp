#include "StudentWorld.h"
#include "Field.h"
#include "Actor.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <list>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

void StudentWorld::setWinningColony(int col) {
    if (antsPerColony[col] >= 6)
        m_winningColony = col;
}

int StudentWorld::getWinningColony() {
    return m_winningColony;
}

void StudentWorld::updateDisplayText() {
    std::ostringstream oss;
    oss << "Ticks:" << std::setw(5) <<
    m_ticks;
    std::string ticks = std::to_string(m_ticks);
    if (m_numColonies > 0) {
        oss << " - ";
    }
    
    for (int i = 0; i < m_numColonies; i++) {
        if (i != 0)
            oss << "  ";
        oss << m_names[i];
        if (i == m_winningColony)
            oss << "*";
        oss << ": ";
        oss << setw(2) << setfill('0') << antsPerColony[i];
    }
    
    setGameStatText(oss.str());
}

bool StudentWorld::isPebble(int x, int y) {
    if (m_f.getContentsOf(x, y) == Field::FieldItem::rock)
        return true;
    return false;
}

bool StudentWorld::isDangerous(int x, int y, int colony) {
    if (!m_actors[y][x].empty()) {
        list<Actor*>::iterator it = m_actors[y][x].begin();
        while (it != m_actors[y][x].end()) {
            if ((*it)->isEnemy(colony) || (*it)->isWater() || (*it)->isPoison()) {
                return true;
            }
            ++it;
        }
    }
    return false;
}

/*
Actor* StudentWorld::getActor(int x, int y) {
    if (m_actors[y][x].empty() == false) {
        return *(m_actors[y][x].begin());
    }
    return nullptr;
}
 */

bool StudentWorld::isThereFood(int x, int y) {
    if (m_actors[y][x].empty() == false) {
        list<Actor*>::iterator it = m_actors[y][x].begin();
        while (it != m_actors[y][x].end()) {
            if ((*it)->isFood()) {
                return true;
            }
            ++it;
        }
    }
    return false;
}

Food* StudentWorld::getFoodAt(int x, int y) {
    if (isThereFood(x, y)) { //if there's food, then we know the list isn't empty
        list<Actor*>::iterator it = m_actors[y][x].begin();
        while (it != m_actors[y][x].end()) {
            Food* temp = dynamic_cast<Food*>(*it);
            //means this is food
            if (temp != nullptr)
                return temp;
            ++it;
        }
    }
    return nullptr;
}

void StudentWorld::addFood(int x, int y, int amount) {
    if (!isThereFood(x, y)) {
        m_actors[y][x].push_back(new Food(IID_FOOD, x, y, this, amount, GraphObject::right, 2));
    }
    else if (!m_actors[y][x].empty()) {
        list<Actor*>::iterator it = m_actors[y][x].begin();
        //if there's already food in this position, only add energy to the food
        if ((*it)->isFood()) {
            Food* f = dynamic_cast<Food*>(*it);
            if (f != nullptr)
                f->addEnergy(amount);
        }
    }
}

bool StudentWorld::isTherePheromone(int x, int y) {
    if (m_actors[y][x].empty() == false) {
        list<Actor*>::iterator it = m_actors[y][x].begin();
        while (it != m_actors[y][x].end()) {
            if ((*it)->isPheromone()) {
                return true;
            }
            ++it;
        }
    }
    return false;
}

void StudentWorld::addPheromone(int x, int y, int colony) {
    if (!isTherePheromone(x, y)) {
        switch(colony) {
            case 0:
                m_actors[y][x].push_back(new Pheromone(IID_PHEROMONE_TYPE0, x, y, this, 256, GraphObject::right, 2));
                break;
            case 1:
                m_actors[y][x].push_back(new Pheromone(IID_PHEROMONE_TYPE1, x, y, this, 256, GraphObject::right, 2));
                break;
            case 2:
                m_actors[y][x].push_back(new Pheromone(IID_PHEROMONE_TYPE2, x, y, this, 256, GraphObject::right, 2));
                break;
            case 3:
                m_actors[y][x].push_back(new Pheromone(IID_PHEROMONE_TYPE3, x, y, this, 256, GraphObject::right, 2));
                break;
        }
    }
    
    else {
        if(!m_actors[y][x].empty()) {
            list<Actor*>::iterator it = m_actors[y][x].begin();
            //if there's already a pheromone in this position, only add energy to the pheromone
            if ((*it)->isPheromone()) {
                Pheromone* p = dynamic_cast<Pheromone*>(*it);
                if (p != nullptr) {
                    if ((p->getEnergy() + 256) > 768) {
                        p->addEnergy(768 - (768 - p->getEnergy()));
                    }
                    else
                        p->addEnergy(256);
                }
            }
        }
    }
}

bool StudentWorld::isMyAnthill(int x, int y, int colony) {
    if (!m_actors[y][x].empty()) {
        list<Actor*>::iterator it = m_actors[y][x].begin();
        if ((*it)->isAnthill()) {
            Anthill* a = dynamic_cast<Anthill*>(*it);
            if (a != nullptr) {
                if (a->getColonyNumber() == colony)
                    return true;
            }
        }
    }
    return false;
}

void StudentWorld::birthAnt(int x, int y, int colonyNumber, Anthill* a, Compiler* c) {
    switch (colonyNumber) {
        case 0:
            m_actors[y][x].push_back(new Ant(IID_ANT_TYPE0, x, y, static_cast<EnergyHolder::Direction>(randInt(1, 4)), this, 1500, c, 0));
            addAntToColony(0);
            break;
        case 1:
            m_actors[y][x].push_back(new Ant(IID_ANT_TYPE1, x, y, static_cast<EnergyHolder::Direction>(randInt(1, 4)), this, 1500, c, 1));
            addAntToColony(1);
            break;
        case 2:
            m_actors[y][x].push_back(new Ant(IID_ANT_TYPE2, x, y, static_cast<EnergyHolder::Direction>(randInt(1, 4)), this, 1500, c, 2));
            addAntToColony(2);
            break;
        case 3:
            m_actors[y][x].push_back(new Ant(IID_ANT_TYPE3, x, y, static_cast<EnergyHolder::Direction>(randInt(1, 4)), this, 1500, c, 3));
            addAntToColony(3);
            break;
    }
    
    if (antsPerColony[colonyNumber] > antsPerColony[m_winningColony]) {
        setWinningColony(colonyNumber);
    }
}

void StudentWorld::addAntToColony(int colonyNumber) {
    antsPerColony[colonyNumber]++;
}

void StudentWorld::addAdultGrasshopper(int x, int y) {
    m_actors[y][x].push_back(new AdultGrasshopper(IID_ADULT_GRASSHOPPER, x, y, static_cast<EnergyHolder::Direction>(randInt(1, 4)), this, 1600, 0, randInt(2, 10)));
}

void StudentWorld::stunInsectsAt(int x, int y) {
    if (m_actors[y][x].empty() == false) {
        list<Actor*>::iterator it = m_actors[y][x].begin();
        while (it != m_actors[y][x].end()) {
            Insect* ip = dynamic_cast<Insect*>(*it);
            if (ip != nullptr) {
                //if actor is not currently stunned by same water object
                if (!(ip->isStunnedByWater())) {
                    ip->addStunned(2);
                    //set that it is now stunned by this water object
                    ip->setStunnedByWater(true);
                }
            }
            ++it;
        }
    }
}

void StudentWorld::poisonInsectsAt(int x, int y) {
    if (m_actors[y][x].empty() == false) {
        list<Actor*>::iterator it = m_actors[y][x].begin();
        while (it != m_actors[y][x].end()) {
            Insect* ip = dynamic_cast<Insect*>(*it);
            if (ip != nullptr) {
                //poison all insects at this position
                //makes sure the insects don't get poisoned more than once (since they're stunned after they move onto poison)
                if ((ip->canBePoisoned())) {
                    if (ip->getEnergy() < 150) {
                        ip->subtractEnergy(ip->getEnergy());
                        ip->setDead();
                    }
                    else
                        ip->subtractEnergy(150);
                    cerr << "just poisoned" << endl;
                    cerr << "energy is now " << ip->getEnergy() << endl;
                }
            }
        ++it;
        }
    }
}

bool StudentWorld::isThereEnemy(int x, int y, int colony) {
    if (!m_actors[y][x].empty()) {
        list<Actor*>::iterator it = m_actors[y][x].begin();
        Insect* ip = dynamic_cast<Insect*>(*it);
            if (ip != nullptr) {
                if (ip->getColony() != colony || ip->getColony() == 4)
                    return true;
            }
        }
    return false;
}
                                                      //pass in a pointer to itself
Insect* StudentWorld::obtainRandomEnemy(int x, int y, Insect* ip) {
    //if there's only one actor in the position, it means it's itself
    if (m_actors[y][x].size() == 1)
        return ip;
    
    list<Actor*>::iterator it;
    int i;
    vector<int> posOfEnemies;
    //create vector of ints representing positions of enemies
    for (it = m_actors[y][x].begin(), i = 0; it != m_actors[y][x].end(); ++it, i++) {
        Insect* insect = dynamic_cast<Insect*>(*it);
        if (insect != nullptr) {
            if ((insect->isEnemy(ip->getColony()) || insect->getColony() == 4) && insect != ip) { //makes sure the insect doesn't bite itself
                posOfEnemies.push_back(i);
            }
        }
    }
    
    //empty vector means there are no enemies, so return itself
    if (posOfEnemies.size() == 0)
        return ip;
    
    //obtain target position by choosing random index of vector of ints
    int targetPos = posOfEnemies.at(randInt(0, static_cast<int>(posOfEnemies.size()-1)));
    //loop to that target position in the list
    for (it = m_actors[y][x].begin(), i = 0; i != targetPos; ++it, i++) {
        ;
    }

    return dynamic_cast<Insect*>(*it);
}

int StudentWorld::init() {
    //attempt to load field - if not return error
    string fieldFileName;
    Field f;
    std::string fieldFile = getFieldFilename();
    string error;
    
    if (f.loadField(fieldFile, error) != Field::LoadResult::load_success) {
        return GWSTATUS_LEVEL_ERROR;
    }
    
    m_f = f; //sets private field member to loaded field
    
    std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
    std::string compilererror;
    m_numColonies = fileNames.size();
    
    if (fileNames.size() > 0) {
        compilerForEntrant0 = new Compiler;
        if (!compilerForEntrant0->compile(fileNames[0], compilererror)) {
            setError(fileNames[0] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        m_names.push_back(compilerForEntrant0->getColonyName());
    }
    if (fileNames.size() > 1) {
        compilerForEntrant1 = new Compiler;
        if (!compilerForEntrant1->compile(fileNames[1], compilererror)) {
            setError(fileNames[1] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        m_names.push_back(compilerForEntrant1->getColonyName());
    }
    if (fileNames.size() > 2) {
        compilerForEntrant2 = new Compiler;
        if (!compilerForEntrant2->compile(fileNames[2], compilererror)) {
            setError(fileNames[2] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        m_names.push_back(compilerForEntrant2->getColonyName());
    }
    if (fileNames.size() > 3) {
        compilerForEntrant3 = new Compiler;
        if (!compilerForEntrant3->compile(fileNames[3], compilererror)) {
            setError(fileNames[3] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        m_names.push_back(compilerForEntrant3->getColonyName());
    }
    
    //allocate objects to data struture
    for (int y = 0; y < VIEW_HEIGHT; y++) {
        for (int x = 0; x < VIEW_WIDTH; x++) {
            Field::FieldItem item = f.getContentsOf(x, y);
            Actor* a;
            switch(item) {
                case Field::FieldItem::empty:
                    break;
                case Field::FieldItem::rock:
                    a = new Pebble(IID_ROCK, x, y, GraphObject::right, 1, this);
                    m_actors[y][x].push_back(a);
                    break;
                case Field::FieldItem::water:
                    a = new Water(IID_WATER_POOL, x, y, GraphObject::right, 2, this);
                    m_actors[y][x].push_back(a);
                    break;
                case Field::FieldItem::poison:
                    a = new Poison(IID_POISON, x, y, GraphObject::right, 2, this);
                    m_actors[y][x].push_back(a);
                    break;
                case Field::FieldItem::food:
                    a = new Food(IID_FOOD, x, y, this, 6000, GraphObject::right, 2);
                    m_actors[y][x].push_back(a);
                    break;
                case Field::FieldItem::grasshopper:
                    a = new BabyGrasshopper(IID_BABY_GRASSHOPPER, x, y, static_cast<EnergyHolder::Direction>(randInt(1, 4)), this, 500, 0, randInt(2, 10));
                    m_actors[y][x].push_back(a);
                    break;
                case Field::FieldItem::anthill0:
                    if (fileNames.size() > 0) {
                        a = new Anthill(IID_ANT_HILL, x, y, this, 8999, GraphObject::right, 2, 0, compilerForEntrant0);
                        m_actors[y][x].push_back(a);
                    }
                    break;
                case Field::FieldItem::anthill1:
                    if (fileNames.size() > 1) {
                        a = new Anthill(IID_ANT_HILL, x, y, this, 8999, GraphObject::right, 2, 1, compilerForEntrant1);
                        m_actors[y][x].push_back(a);
                    }
                    break;
                case Field::FieldItem::anthill2:
                    if (fileNames.size() > 2) {
                        a = new Anthill(IID_ANT_HILL, x, y, this, 8999, GraphObject::right, 2, 2, compilerForEntrant2);
                        m_actors[y][x].push_back(a);
                    }
                    break;
                case Field::FieldItem::anthill3:
                    if (fileNames.size() > 3) {
                        a = new Anthill(IID_ANT_HILL, x, y, this, 8999, GraphObject::right, 2, 3, compilerForEntrant3);
                        m_actors[y][x].push_back(a);
                    }
                    break;
                }
            }
        }
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::removeDead() {
    for (int y = 0; y < VIEW_HEIGHT; y++) {
        for (int x = 0; x < VIEW_WIDTH; x++) {
            if (m_actors[y][x].empty() == false) {
                list<Actor*>::iterator it = m_actors[y][x].begin();
                while (it != m_actors[y][x].end()) {
                    if ((*it)->isDead()) {
                        delete (*it);
                        it = m_actors[y][x].erase(it);
                    }
                    else
                        ++it;
                }
            }
        }
    }
}

int StudentWorld::move() {
    while (m_ticks > 0) {
        m_ticks--;
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            for (int x = 0; x < VIEW_WIDTH; x++) {
                if (m_actors[y][x].empty() == false) {
                    list<Actor*>::iterator it = m_actors[y][x].begin();
                    while (it != m_actors[y][x].end()) {
                        int currentX = (*it)->getX();
                        int currentY = (*it)->getY();
                        //makes sure actors don't move twice in the same tick
                        if ((*it)->getMoved() == false) {
                            (*it)->doSomething();
                            (*it)->setMoved(true);
                        }
                        if ((*it)->getX() != currentX || (*it)->getY() != currentY) {
                            currentX = (*it)->getX();
                            currentY = (*it)->getY();
                            Actor* a = *it;
                            it = m_actors[y][x].erase(it);
                            m_actors[currentY][currentX].push_back(a);
                        }
                        else
                            ++it;
                    }
                }
            }
        }
        
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            for (int x = 0; x < VIEW_WIDTH; x++) {
                if (m_actors[y][x].empty() == false) {
                    list<Actor*>::iterator it = m_actors[y][x].begin();
                    while (it != m_actors[y][x].end()) {
                        //reset all actors to "unmoved" state
                        if ((*it)->getMoved()) {
                            (*it)->setMoved(false);
                        }
                        ++it;
                    }
                }
            }
        }
        
        removeDead();
        updateDisplayText();
        return GWSTATUS_CONTINUE_GAME;
    }
    
    if (getWinningColony() == -1)
        return GWSTATUS_NO_WINNER;
    else {
        setWinner(m_names[m_winningColony]);
        return GWSTATUS_PLAYER_WON;
        
    }
}

void StudentWorld::cleanUp() {
    for (int y = 0; y < VIEW_HEIGHT; y++) {
        for (int x = 0; x < VIEW_WIDTH; x++) {
            if (m_actors[y][x].empty() == false) {
                list<Actor*>::iterator it = m_actors[y][x].begin();
                while (it != m_actors[y][x].end()) {
                    delete *it;
                    it = m_actors[y][x].erase(it);
                }
            }
        }
    }
    
    delete compilerForEntrant0;
    compilerForEntrant0 = nullptr;
    delete compilerForEntrant1;
    compilerForEntrant1 = nullptr;
    delete compilerForEntrant2;
    compilerForEntrant2 = nullptr;
    delete compilerForEntrant3;
    compilerForEntrant3 = nullptr;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}
