#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include "Compiler.h"


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {
    public:
        Actor(int imageID, int startX, int startY, StudentWorld* studentWorld);
        virtual void doSomething() = 0; //Actor is an abstract class
        StudentWorld* getStudentWorld();
        //GET
        bool isDead() const;
        bool getMoved() const;
        //SET
        void setDead();
        void setMoved(bool set);
        //TRYING TO AVOID DYNAMIC CAST BUT ITS NOT WORKING
        virtual bool isPebble() {return false;}
        virtual bool isFood() {return false;}
        virtual bool isWater() {return false;}
        virtual bool isPoison() {return false;}
        virtual bool isInsect() {return false;}
        virtual bool isAnt() {return false;}
        virtual bool isBabyGrasshopper() {return false;}
        virtual bool isAdultGrasshopper() {return false;}
        virtual bool isDangerous() {return false;}
        virtual bool isPheromone() {return false;}
        virtual bool isAnthill() {return false;}
        virtual bool isEnemy(int colony) {return false;}
    private:
        bool m_dead;
        bool m_moved;
        StudentWorld* m_studentWorld;
};


//PEBBLE

class Pebble : public Actor {
    public:
        Pebble(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* studentWorld);
        virtual bool isPebble() {return true;}
        virtual void doSomething() {}; //does nothing
};

//WATER AND POISON

class Water : public Actor {
    public:
        Water(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* studentWorld);
        virtual bool isWater() {return true;}
        virtual void doSomething();
        virtual bool isDangerous() {return true;}
};

class Poison : public Actor {
    public:
        Poison(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* studentWorld);
        virtual bool isPoison() {return true;}
        virtual void doSomething();
        virtual bool isDangerous() {return true;}
};


//ENERGY HOLDERS

class EnergyHolder : public Actor {
    public:
        EnergyHolder(int imageID, int startX, int startY, StudentWorld* studentWorld, int energy);
        virtual void doSomething() = 0; //EnergyHolder is an abstract class
        //GET
        int getEnergy();
        //MUTATE
        void subtractEnergy(int n);
        void addEnergy(int n);
    private:
        int m_energy;
};

class Food : public EnergyHolder {
    public:
        Food(int imageID, int startX, int startY, StudentWorld* studentWorld, int energy, Direction dir, int depth);
        virtual bool isFood() {return true;}
        virtual void doSomething();
};

class Anthill : public EnergyHolder {
    public:
        Anthill(int imageID, int startX, int startY, StudentWorld* studentWorld, int energy, Direction dir, int depth, int colonyNumber, Compiler* c);
        virtual void doSomething();
        void giveBirth();
        int getColonyNumber();
        Compiler* getCompiler();
        virtual bool isAnthill() {return true;}
        void eatFood();
    private:
        int m_colonyNumber;
        int m_numOfAnts;
        Compiler* m_c;
};

class Pheromone : public EnergyHolder {
    public:
        Pheromone(int imageID, int startX, int startY, StudentWorld* studentWorld, int energy, Direction dir, int depth);
        virtual void doSomething();
        virtual bool isPheromone() {return true;}
};

//INSECT

class Insect : public EnergyHolder {
    public:
        Insect(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, int colony, int stunned = 0, int depth = 1);
        virtual void doSomething() = 0; //Insect is an abstract class
        virtual bool isInsect() {return true;}
        //GET
        bool isStunned() const;
        int getStunned() const;
        virtual bool isStunnedByWater() const;
        virtual bool canBePoisoned() const;
        bool isEnemy(int colony);
        //SET
        void setStunned();
        void setPoisoned(bool set);
        void setStunnedByWater(bool set);
        //MUTATE
        virtual void decrementStunned();
        virtual void addStunned(int n);
        void bite(int damage);
        virtual int getColony() {return m_colony;}
private:
        int m_colony;
        int m_stunned;
        int m_stunnedByWater;
        int m_poisoned;
};

class Ant : public Insect {
    public:
        Ant(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, Compiler* c, int colony);
        virtual void doSomething();
        Compiler* getCompiler();
        int getIC();
        bool trueCondition(int op1);
        bool getBitten();
        void setBitten(bool set);
        void move();
        void rotate(string direction);
        bool pickUpFood();
        void dropFood();
        bool eatFood();
        bool interpreter(int& ic);
        bool detectDanger();
        bool detectPheromone();
        void emitPheromone();
        virtual int getColony() {return m_colony;}
    private:
        int m_colony;
        bool m_blocked;
        bool m_bitten;
        int m_lastRandomNumber;
        int m_ic;
        int m_food;
        Compiler* m_c;
};

//GRASSHOPPER

class Grasshopper : public Insect {
    public:
        Grasshopper(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy,
        int stunned, int distance);
        virtual void doSomething() = 0; //grasshopper is an abstract class
        //GET
        int getDistance();
        bool justAte() const;
        //MUTATE
        void decrementDistance();
        void resetDistanceAndDirection();
        void setJustAte(bool set);
        //OTHER
        void attemptMove();
        void getFood();
        virtual int getColony() {return 4;}
    private:
        int m_distance;
        bool m_justAte;
};

class BabyGrasshopper : public Grasshopper {
    public:
        BabyGrasshopper(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, int stunned, int distance);
        virtual void doSomething();
        virtual bool isBabyGrasshopper() {return true;}
};

class AdultGrasshopper : public Grasshopper {
    public:
        AdultGrasshopper(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, int stunned, int distance);
        //Set Adult Grasshopper to always be stunned by water/poisoned
        //so that it never executes the statements in the "stun" and "poison" methods
        virtual bool isStunnedByWater() const {return true;}
        virtual bool canBePoisoned() const {return false;}
        virtual bool isAdultGrasshopper() {return true;}
        virtual void doSomething();
        void biteback(int x, int y, Insect* ip);
        void giveBirth();
        void jump();
    private:
        bool m_poisoned;
        bool m_stunnedByWater;
};

#endif // ACTOR_H_
