#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "Actor.h"
#include "GameWorld.h"
#include "Board.h"
#include <vector>

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool EmptySquare(int x, int y); //Checks if the next square is empty
    Player* pPeach(); //Return pointer for player 1
    Player* pYoshi(); //Return pointer to player 2
    int getBBalance() const; //Returns Bank Balance
    void setBBalance(int amount); //Sets Bank Balance
    void ChangeSquareToDrop(int x, int y);
    Actor* contactVortex(int x, int y);
    void pushVortex(int x, int y, int startDirection);
    
private:
    std::vector<Actor*> m_actors;
    Player* m_peach; //Pointer to Peach
    Player* m_yoshi; //Pointer to Yoshi
    Board bd;
    int m_bankAccount;
};

#endif // STUDENTWORLD_H_

