#include "Actor.h"
#include "StudentWorld.h"
#include <cassert>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, int startX, int startY, StudentWorld* studentWorld) :
GraphObject (imageID, startX, startY), m_dead(false), m_studentWorld(studentWorld), m_moved(false) {}

StudentWorld* Actor::getStudentWorld() {
    return m_studentWorld;
}

bool Actor::getMoved() const {
    return m_moved;
}

void Actor::setMoved(bool set) {
    m_moved = set;
}

bool Actor::isDead() const
{
    return m_dead;
}

void Actor::setDead() {
    m_dead = true;
}

//WATER AND POISON IMPLEMENTATION

Water::Water(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* studentWorld) :
Actor(imageID, startX, startY, studentWorld) {}

void Water::doSomething() {
    getStudentWorld()->stunInsectsAt(getX(), getY());
}

Poison::Poison(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* studentWorld) :
Actor(imageID, startX, startY, studentWorld) {}

void Poison::doSomething() {
    getStudentWorld()->poisonInsectsAt(getX(), getY());
}

//PEBBLE IMPLEMENTATION
Pebble::Pebble(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* studentWorld) :
Actor (IID_ROCK, startX, startY, studentWorld) {}

//ENERGY HOLDER IMPLEMENTATION
EnergyHolder::EnergyHolder(int imageID, int startX, int startY, StudentWorld* studentWorld, int energy) :
Actor(imageID, startX, startY, studentWorld), m_energy(energy) {}

int EnergyHolder::getEnergy() {
    return m_energy;
}

void EnergyHolder::addEnergy(int n) {
    m_energy += n;
}

void EnergyHolder::subtractEnergy(int n) {
    m_energy -= n;
}

//FOOD IMPLEMENTATION
Food::Food(int imageID, int startX, int startY, StudentWorld* studentWorld, int energy, Direction dir, int depth) :
EnergyHolder(imageID, startX, startY, studentWorld, energy) {}

void Food::doSomething() {
    if (getEnergy() <= 0)
        setDead();
}

//ANTHILL
Anthill::Anthill(int imageID, int startX, int startY, StudentWorld* studentWorld, int energy, Direction dir, int depth, int colonyNumber, Compiler* c) :
EnergyHolder(imageID, startX, startY, studentWorld, energy), m_colonyNumber(colonyNumber), m_c(c) {}

int Anthill::getColonyNumber() {
    return m_colonyNumber;
}

Compiler* Anthill::getCompiler() {
    return m_c;
}

void Anthill::eatFood() {
    Food* fp = getStudentWorld()->getFoodAt(getX(), getY());
    if (fp != nullptr) { //there's food at this spot
        cerr << "ANTHILL EATING" << endl;
        
        if (fp->getEnergy() <= 10000) {
            int temp = fp->getEnergy();
            fp->subtractEnergy(temp);
            addEnergy(temp);
            fp->setDead();
        }
        else {
            fp->subtractEnergy(10000);
            addEnergy(10000);
        }
        if (fp->getEnergy() <= 0)
            fp->setDead();
    }
}

void Anthill::giveBirth() {
    getStudentWorld()->birthAnt(getX(), getY(), getColonyNumber(), this, getCompiler());
}

void Anthill::doSomething() {
    if (getEnergy() >= 2000) {
        giveBirth();
        subtractEnergy(1500);
        //cerr << "creating ants" << endl;
    }
    
    eatFood();
}

//PHEROMONES
Pheromone::Pheromone(int imageID, int startX, int startY, StudentWorld* studentWorld, int energy, Direction dir, int depth) :
EnergyHolder(imageID, startX, startY, studentWorld, energy) {}

void Pheromone::doSomething() {
    subtractEnergy(1);
    if (getEnergy() == 0)
        setDead();
}

//INSECTS
Insect::Insect(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, int colony, int stunned, int depth) :
EnergyHolder(imageID, startX, startY, studentWorld, energy), m_stunned(stunned) {}


bool Insect::isStunned() const {
    if (m_stunned != 0) {
        return true;
    }
    return false;
}

int Insect::getStunned() const{
    return m_stunned;
}

void Insect::setStunned() {
    m_stunned = 2;
}

void Insect::decrementStunned() {
    m_stunned--;
}

void Insect::addStunned(int n) {
    m_stunned += n;
    //cerr << m_stunned << endl;
}

void Insect::setStunnedByWater(bool set) {
    m_stunnedByWater = set;
}

bool Insect::isStunnedByWater() const {
    return m_stunnedByWater;
}

void Insect::setPoisoned(bool set) {
    m_poisoned = set;
}

bool Insect::canBePoisoned() const {
    return true;
}

bool Insect::isEnemy(int colony) { //colony of insect you're comparing it to
    if (colony != getColony())
        return true;
    return false;
}

void Insect::bite(int damage) {
    Insect* bitee = getStudentWorld()->obtainRandomEnemy(getX(), getY(), this);
    //if obtainRandomEnemy doesn't return this, that means there's an enemy to bite
    if (bitee != this) {
        
        bitee->subtractEnergy(damage);
        cerr << "energy after being bitten is " << bitee->getEnergy() << endl;
        //check if ant was bitten
        if (getEnergy() <= 0) {
            setDead();
        }
        
        if (!bitee->isDead()) {
            Ant* a = dynamic_cast<Ant*>(bitee);
            if (a != nullptr) {
                a->setBitten(true);
                return;
            }
            //50% chance that the insect just bitten will try to bite back
            AdultGrasshopper* newBiter = dynamic_cast<AdultGrasshopper*>(bitee);
            if (newBiter != nullptr) {
                if (randInt(0, 1) == 0) {
                    if (getStudentWorld()->getTicks() != 0) {
                        newBiter->bite(50);
                        cerr << "bit back" << endl;
                    }
                }
            }
        }
    }
}

//ANTS

Ant::Ant(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, Compiler* c, int colony) : Insect(imageID, startX, startY, dir, studentWorld, energy, colony), m_c(c), m_ic(0), m_bitten(false), m_food(0), m_colony(colony) {}

Compiler* Ant::getCompiler() {
    return m_c;
}

bool Ant::getBitten() {
    return m_bitten;
}

void Ant::setBitten(bool set) {
    m_bitten = true;
}

int Ant::getIC() {
    return m_ic;
}

bool Ant::pickUpFood() {
    if (m_food <= 1800) {
        Food* fp = getStudentWorld()->getFoodAt(getX(), getY());
        if (fp != nullptr) { //there's food at this spot
            if (fp->getEnergy() >= 400 && m_food <= 1400) {
                m_food += 400;
                fp->subtractEnergy(400);
            }
            else if (fp->getEnergy() < 400 && m_food <= (1800-getEnergy())) {
                m_food += getEnergy();
                fp->subtractEnergy(getEnergy());
            }
            else {
                m_food += (1800-m_food);
                fp->subtractEnergy((1800-m_food));
            }
            if (fp <= 0)
                fp->setDead();
            return true;
        }
    }
    return false;
}

void Ant::dropFood() {
    if (m_food > 0)
        getStudentWorld()->addFood(getX(), getY(), m_food);
}

bool Ant::eatFood() {
    if (m_food > 0) {
        if (m_food < 100) {
            addEnergy(m_food);
            m_food -= m_food;
        }
        else {
            addEnergy(100);
            m_food -= 100;
        }
        return true;
    }
    return false;
}

void Ant::move() {
    switch(getDirection()) {
        case none:
            break;
        case up:
            if (!getStudentWorld()->isPebble(getX(), getY()+1)) {
                moveTo(getX(), getY()+1);
                setBitten(false);
                m_blocked = false;
            }
            else
                m_blocked = true;
            break;
        case down:
            if (!getStudentWorld()->isPebble(getX(), getY()-1)) {
                moveTo(getX(), getY()-1);
                setBitten(false);
                m_blocked = false;
            }
            else
                m_blocked = true;
            break;
        case right:
            if (!getStudentWorld()->isPebble(getX()+1, getY())) {
                moveTo(getX()+1, getY());
                setBitten(false);
                m_blocked = false;
            }
            else
                m_blocked = true;
            break;
        case left:
            if (!getStudentWorld()->isPebble(getX()-1, getY())) {
                moveTo(getX()-1, getY());
                setBitten(false);
                m_blocked = false;
            }
            else
                m_blocked = true;
            break;
    }
}

void Ant::rotate(string direction) {
    if (direction == "clockwise") {
        switch(getDirection()) {
            case none:
                break;
            case up:
                setDirection(right);
                break;
            case down:
                setDirection(left);
                break;
            case right:
                setDirection(down);
                break;
            case left:
                setDirection(up);
                break;
            default:
                break;
        }
        
    }
    else if (direction == "counterclockwise") {
        switch(getDirection()) {
            case none:
                break;
            case up:
                setDirection(left);
                break;
            case down:
                setDirection(right);
                break;
            case right:
                setDirection(up);
                break;
            case left:
                setDirection(down);
                break;
        }
    }
}

bool Ant::detectDanger() {
    switch(getDirection()) {
        case up:
            if (getStudentWorld()->isDangerous(getX(), getY()+1, getColony())) {
                return true;
            }
            break;
        case down:
            if (getStudentWorld()->isDangerous(getX(), getY()-1, getColony())) {
                return true;
            }
            break;
        case right:
            if (getStudentWorld()->isDangerous(getX()+1, getY(), getColony())) {
                return true;
            }
            break;
        case left:
            if (getStudentWorld()->isDangerous(getX()-1, getY(), getColony())) {
                return true;
            }
            break;
    }
    return false;
}

bool Ant::detectPheromone() {
    switch(getDirection()) {
        case up:
            if (getStudentWorld()->isTherePheromone(getX(), getY()+1)) {
                return true;
            }
            break;
        case down:
            if (getStudentWorld()->isTherePheromone(getX(), getY()-1)) {
                return true;
            }
            break;
        case right:
            if (getStudentWorld()->isTherePheromone(getX()+1, getY())) {
                return true;
            }
            break;
        case left:
            if (getStudentWorld()->isTherePheromone(getX()-1, getY())) {
                return true;
            }
            break;
    }
    return false;
}

bool Ant::trueCondition(int op1) {
    switch(op1) {
        case Compiler::i_smell_danger_in_front_of_me:
            return detectDanger();
            break;
        case Compiler::i_smell_pheromone_in_front_of_me:
            return detectPheromone();
            break;
        case Compiler::i_was_bit:
            if (getBitten())
                return true;
            break;
        case Compiler::i_am_carrying_food:
            if (m_food > 0)
                return true;
            break;
        case Compiler::i_am_hungry:
            if (getEnergy() <= 25)
                return true;
            break;
        case Compiler::i_am_standing_on_my_anthill:
            if (getStudentWorld()->isMyAnthill(getX(), getY(), getColony())) {
                //cerr << "i'm standing on my antill" << endl;
                return true;
            }
            break;
        case Compiler::i_am_standing_on_food:
            if (getStudentWorld()->isThereFood(getX(), getY())) {
                //cerr << "i'm standing on food" << endl;
                return true;
            }
            break;
        case Compiler::i_am_standing_with_an_enemy:
            if (getStudentWorld()->isThereEnemy(getX(), getY(), getColony()))
                return true;
            break;
        case Compiler::i_was_blocked_from_moving:
            if (m_blocked == true)
                return true;
            break;
        case Compiler::last_random_number_was_zero:
            if (m_lastRandomNumber == 0)
                return true;
            break;
        default:
            break;
    }
    return false;
}

bool Ant::interpreter(int &ic) {
    int count = 0;
    Compiler::Command cmd;
    while (count < 10) {
        if (!(m_c->getCommand(ic, cmd)))
            return false;
        else {
            switch(cmd.opcode) {
                case Compiler::if_command:
                    if(trueCondition(stoi(cmd.operand1)))
                        ic = stoi(cmd.operand2);
                    else
                        ic++;
                    break;
                case Compiler::goto_command:
                    ic = stoi(cmd.operand1);
                    break;
                case Compiler::generateRandomNumber:
                    m_lastRandomNumber = randInt(0, stoi(cmd.operand1));
                    break;
                case Compiler::moveForward:
                    move();
                    ic++;
                    return true;
                    break;
                case Compiler::emitPheromone:
                    getStudentWorld()->addPheromone(getX(), getY(), getColony());
                    ic++;
                    return true;
                    break;
                case Compiler::faceRandomDirection:
                    setDirection(static_cast<Direction>(randInt(1, 4)));
                    ic++;
                    return true;
                    break;
                case Compiler::rotateClockwise:
                    rotate("clockwise");
                    ic++;
                    return true;
                    break;
                case Compiler::rotateCounterClockwise:
                    rotate("counterclockwise");
                    ic++;
                    return true;
                    break;
                case Compiler::bite:
                    if (getStudentWorld()->isThereEnemy(getX(), getY(), getColony())) {
                        cerr << "biting" << endl;
                        bite(15);
                        ic++;
                        return true;
                    }
                    ic++;
                    break;
                case Compiler::pickupFood:
                    if (pickUpFood()) {
                        ic++;
                        return true;
                    }
                    ic++;
                    break;
                case Compiler::dropFood:
                    if (m_food > 0) {
                        dropFood();
                        ic++;
                        return true;
                    }
                    ic++;
                    break;
                case Compiler::eatFood:
                    if (eatFood()) {
                        ic++;
                        return true;
                    }
                    ic++;
                    break;
            }
        }
        count++;
    }
    return true;
}

void Ant::doSomething() {
    subtractEnergy(1);
    
    
    if (isStunned()) {
        decrementStunned();
        return;
    }
    
    if (getEnergy() <= 0) {
        getStudentWorld()->addFood(getX(), getY(), 100);
        setDead();
        return;
    }
    
    else {
        if (!interpreter(m_ic)) {
            cerr << "setting dead now" << endl;
            setDead();
            getStudentWorld()->addFood(getX(), getY(), 100);
            return;
        }
    }
    
    setStunnedByWater(false);
}

Grasshopper::Grasshopper(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, int stunned, int distance) : Insect(imageID, startX, startY, dir, studentWorld, energy, stunned), m_distance(distance), m_justAte(false) {}


int Grasshopper::getDistance() {
    return m_distance;
}

void Grasshopper::resetDistanceAndDirection() {
    m_distance = randInt(2, 10);
    setDirection(static_cast<Direction>(randInt(1, 4)));
}

void Grasshopper::decrementDistance() {
    m_distance--;
}

bool Grasshopper::justAte() const {
    return m_justAte;
}

void Grasshopper::setJustAte(bool set) {
    m_justAte = set;
}

void Grasshopper::getFood() {
    Food* fp = getStudentWorld()->getFoodAt(getX(), getY());
    if (fp != nullptr) { //there's food at this spot
        //cerr << "food value is " << fp->getEnergy() << endl;
        if (fp->getEnergy() <= 200) {
            int temp = fp->getEnergy();
            fp->subtractEnergy(temp);
            addEnergy(temp);
            fp->setDead();
        }
        else {
            fp->subtractEnergy(200);
            addEnergy(200);
        }
        //cerr << "food value is now " << fp->getEnergy() << endl;

    }
}

void Grasshopper::attemptMove() {
    switch(getDirection()) {
        case none:
            break;
        case up:
            if (!getStudentWorld()->isPebble(getX(), getY()+1))
                moveTo(getX(), getY()+1);
            else
                resetDistanceAndDirection();
            break;
        case down:
            if (!getStudentWorld()->isPebble(getX(), getY()-1))
                moveTo(getX(), getY()-1);
            else {
                resetDistanceAndDirection();
            }
            break;
        case right:
            if (!getStudentWorld()->isPebble(getX()+1, getY()))
                moveTo(getX()+1, getY());
            else
                resetDistanceAndDirection();
            break;
        case left:
            if (!getStudentWorld()->isPebble(getX()-1, getY()))
                moveTo(getX()-1, getY());
            else
                resetDistanceAndDirection();
            break;
    }
}

BabyGrasshopper::BabyGrasshopper(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, int stunned, int distance) : Grasshopper(imageID, startX, startY, dir, studentWorld, energy, stunned, distance) {}

void BabyGrasshopper::doSomething() {
    subtractEnergy(1);
    if (getEnergy() == 0) {
        getStudentWorld()->addFood(getX(), getY(), 100);
        setDead();
        return;
    }

    //checks stunned
    if (isStunned()) {
        //cerr << getStunned() << endl;
        decrementStunned();
        return;
    }
    
    //checks if energy is high enough to become adult
    if (getEnergy() >= 1600) {
        //add adult grasshopper
        getStudentWorld()->addAdultGrasshopper(getX(), getY());
        setDead();
        getStudentWorld()->addFood(getX(), getY(), 100);
        return;
    }
    
    if (getStudentWorld()->isThereFood(getX(), getY())) {
        getFood();
        setJustAte(true);
    }
    
    if (justAte()) {
        setJustAte(false);
        switch(randInt(0, 1)) {
            case 0:
                setStunned();
                return;
            case 1:
                break;
        }
    }
    
    //if hasn't finished walking in desired direction w desired distance, move one step
    if (getDistance() != 0) {
        decrementDistance();
        attemptMove();
    }
    
    //else change direction and distance
    else if (getDistance() == 0) {
        resetDistanceAndDirection();
        return;
    }
    
    setStunned();
    
    //makes sure they don't get stuck on the pools and poison (bc they would've already been done being stunned if they got to this point)
    setStunnedByWater(false);
    setPoisoned(false);
}

//ADULT GRASSHOPPER
AdultGrasshopper::AdultGrasshopper(int imageID, int startX, int startY, Direction dir, StudentWorld* studentWorld, int energy, int stunned, int distance) : Grasshopper(imageID, startX, startY, dir, studentWorld, energy, stunned, distance) {}


void AdultGrasshopper::jump() {
    int i = 0;
    while (i < 10) {
        int radius = randInt(1, 10);
        int degrees = randInt(0, 360);
        double radians = (M_PI*degrees)/(180);
        int distanceX = radius * cos(radians);
        int distanceY = radius * sin(radians);
    if (!getStudentWorld()->isPebble(getX()+distanceX, getY()+distanceY) && getX()+distanceX < 64 && getY()+distanceY < 64 && getX()+distanceX >= 0 && getY()+distanceY >= 0)
        {
            moveTo(getX()+distanceX, getY()+distanceY);
            if (degrees <= 45 || degrees > 315)
                setDirection(right);
            else if (degrees <= 135)
                setDirection(up);
            else if (degrees <= 225)
                setDirection(left);
            else if (degrees <= 315)
                setDirection(down);
            return;
        }
        i++;
    }
}

void AdultGrasshopper::doSomething() {
    subtractEnergy(1);
    
    //if energy reaches 0, set dead & add 100 units of food to the arena
    if (getEnergy() == 0) {
        getStudentWorld()->addFood(getX(), getY(), 100);
        setDead();
        return;
    }
    
    //if it's currently stunned, return
    if (isStunned()) {
        decrementStunned();
        return;
    }
    
    //else, there's a 1 in 3 chance it'll attempt to bite a random enemy on the square (if there are any)
    if (randInt(0, 2) == 0) {
        if (getStudentWorld()->isThereEnemy(getX(), getY(), getColony())) {
            //cerr << "yes" << endl;
            bite(50);
        }
        setStunned();
    }
    
    if (randInt(0, 9) == 0) {
        jump();
        return;
    }
    
    if (getStudentWorld()->isThereFood(getX(), getY())) {
        getFood();
        setJustAte(true);
    }
    
    if (justAte()) {
        setJustAte(false);
        //50% chance a grasshopper that has just eaten will immediately want to rest
        switch(randInt(0, 1)) {
            case 0:
                setStunned();
                return;
            case 1:
                break;
        }
    }

    //if the grasshopper hasn't finished walking the desired distance in the desire direction, take one step
    if (getDistance() != 0) {
        decrementDistance();
        attemptMove();
    }
    
    //if it has, change distance and direction
    else if (getDistance() == 0) {
        resetDistanceAndDirection();
        return;
    }
    
    setStunned();
}
