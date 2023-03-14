#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject //Defines class Actor that inherits from GraphObject. Actor class represents objects that can exist in the game world of a particular game.
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, int startDirection = right, int depth = 0, double size = 1.0); //parameters to initialize including pointer to StudentWorld which allow actors to interact with each other and game world. Decided to use default argument values to provide flexibility and convenience for the caller. This can make the constructor easier to use and less error-prone since the caller doesn't have to provide all the arguments every time. I could have used an initilization list as well but went with this approach.
    virtual void doSomething() = 0; //Pure virtual as it will be overriden in any subclass of Actor. doSomething represents action every tick
    StudentWorld* getWorld() const; //returns a pointer to the StudentWorld object that the Actor is currently in
    virtual bool actorAlive() const; // Checks if actor is alive
    virtual bool isPlayerEnemy(); //Checks if actor is a player or an enemy
    virtual bool impactableActor() const; //Checks if actor is impactable
    void setDead(); //Sets actor to dead
    virtual void ifHit(); //What to do when hit by vortex
    
private:
    StudentWorld* m_world; //Pointer to the world actor is in
    bool m_actorAlive; //Shows whether an actor is dead or alive
    int m_spriteDirection; //Sprite Direction
    
};
//base class for Player and Enemy
class AvatarMovement : public Actor
{
public:
    AvatarMovement(StudentWorld* world, int imageID, int startX, int startY);
    virtual void doSomething() = 0;
    bool isPlayerEnemy(); //Only players and enemies can be avatars
    bool openDirection(int direction); //Check if we can we move in this direction
    int moveRandDirection(); //New random direction
    void teleport(); //Teleports actor
    bool fork(); //Checks for fork so user can choose direction
    void corner(); //Need function to allow movement to continue without direct user input. Different from forks
    virtual void moveAvatar(); //Function to move avatar
    virtual void specMove() = 0; //Pure virtual when ticks left zero specialized movement
    void setWalkDir(int dir); //Setter of the walking direction
    int getWalkDir() const; // Getter for walking direction
    void setState(int state); //Setter for the avatar's state (Walking or Waiting)
    int getState() const;// Getter for avatar's state
    void setTicks(int amount); //Setter for number of ticks left to move
    int getTicks() const; //Getter for number of ticks left to move
    
private:
    static const int DEFAULT = 0; //constant integer value that is used to represent the default state of a player or baddie. Value belongs to the class Actor rather than a specific instance of the class. This means that all objects of the Actor class will share the same value for Waiting.
    int m_state; //Walking or waiting state
    int m_walkDir; //Direction for walk
    int m_ticksLeft; //Ticks left
};

//Derived class specfic to player objects (Peach and Yoshi)
class Player : public AvatarMovement {
public:
    Player (StudentWorld* world, int imageID, int startX, int startY, int playerNum);
    void doSomething();
    void specMove(); // Calls specific move function
    bool squareLanded() const; //Check if player landed on square
    bool onSquare() const; //Checks if player is still on the square
    bool hasVortex() const; //Check if player has vortex equipped
    int isWaiting() const; // Checks to see if player is waiting. Chose int instead of bool so integer could reprsent various states objects could be in.
    void isLocation(bool loc);  //Sets location to true or false
    void setStars(int amount); //Sets stars for Player
    int getStars () const; //Getter for # Player stars
    void setCoins (int amount); //Setter for Player coins
    int getCoins() const; //Getter for # Player coins
    void setRoll(int numRolls); //Sets the players roll
    int getRoll () const; //Getter for player's die roll
    void setVortex(); //Sets vortex to flip states
    void setPlayerOnDirSquare(bool dir); //Setter for player on directional square
    void setTeleport(bool tel); //Setter for teleporting
private:
    //Constants
    static const int WAITING_TO_ROLL = 0; // Integer value 0 to represent state of player when waiting to roll
    static const int WALKING_ON_SQUARES = 1; // Integer value 1 represent state of player when walking along board
    const int PLAYER_1 = 1; //Represent player 1 (Peach)
    const int PLAYER_2 = 2; //Represent player 2 (Yoshi)
    
    int m_num_coins; //Number of player coins
    int m_num_stars; //Number of player stars
    int m_roll; //Players die roll
    int m_playerNum; //Player number
    bool m_vortex; //In possesion of vortex or not
    bool m_justLanded; //If player just came to square
    bool m_onIt;//If player is staying the square
    bool m_onDirSquare; //If player is on directional square
    bool m_teleported; //If player is teleported
};

//Base class for all the squares
class Square : public Actor {
public:
    Square(StudentWorld* world, int imageID, int startX, int startY);
    virtual void doSomething();
    virtual void landSquare(Player* p) = 0; // Function for square action if player lands on it. Pure virtual as each square acts differents
    virtual void traverseSquare(Player *p) = 0; // Function for square if player traverses over the square. Pure virtual as each square acts differents
private:
};

//Derived class for the star square
class StarSquare  : public Square {  //Star square not impactable
public:
    StarSquare(StudentWorld* world, int imageID, int startX, int startY);
    void landSquare(Player* p);
    void traverseSquare(Player* p);
private:
};

//Derived class for the coin square
class CoinSquare : public Square { //Coin Square not impactable
public:
    CoinSquare(StudentWorld* world, int imageID, int startX, int startY, int amount);
    void landSquare(Player* p); //Grants 3 coins or deducts 3 coins when player lands on it
    void traverseSquare(Player* p);
private:
    int m_adjustCoinCount; //Number of coins to take away or add
};

class BankSquare : public Square {
public:
    BankSquare(StudentWorld* world, int imageID, int startX, int startY);
    void landSquare(Player* p); //Gives all coins from central bank
    void traverseSquare(Player* p); //Deducts 5 coins and deposits in central bank
private:
};

//Derived class for the event square
class EventSquare : public Square {
public:
    EventSquare(StudentWorld* world, int imageID, int startX, int startY); //
    void landSquare(Player* p); //Teleport player, player swap, or gives player Vortex Projectile
    void traverseSquare(Player* p);
private:
};
//Derived class for the dropping square
class DroppingSquare: public Square {
public:
    DroppingSquare(StudentWorld* world, int imageID, int startX, int startY);
    void landSquare(Player* p);
    void traverseSquare(Player* p);
private:
};

//Derived class for the directional square
class Directional: public Square{
public:
    Directional(StudentWorld* world, int imageID, int startX, int startY, int nDir);
    void landSquare(Player* p);
    void traverseSquare(Player* p);
private:
    int chosenDirection;
    };

//Derived class specific to enemies in game (Bowser and Boo)
class Enemy : public AvatarMovement {
public:
    Enemy (StudentWorld* world, int imageID, int startX, int startY);
    void doSomething();
    virtual void specMove() = 0; //For what enemy does while moving
    virtual void paused (Player* p) = 0; // points to player object and pure virtual as different for bowser and boo
    virtual int numSquares () = 0;
    void setPeach (bool touch); //Setter for in contact with Peach
    bool getPeach () const; //Getter for peach contact
    void setYoshi (bool touch); //Setter for in contact with Yoshi
    bool getYoshi() const; //Getter for Yoshi contact
    void setPauseCounter(int amount); //Pause Counter
    void getHit(); //Enemy hit by vortex
    bool impactableActor() const; //Enemies are impactable
private:
    //Constants
    static const int PAUSED_ENEMY = 0;
    static const int WALKING_ENEMY = 1;
    
    bool m_contactPeach; //Checks if contact made with Peach
    bool m_contactYoshi; //Checks if contact made with Yoshi
    int m_pauseCounter; //Pause counter variable
    int m_distTravel; //Travel Distance
    int m_squaresToMove; //Squares left to move
};

class Boo: public Enemy {
public:
    Boo (StudentWorld* world, int imageID, int startX, int startY);
    virtual void specMove(); //function specific to Boo movement
    virtual void paused(Player* p); //Points to player object
    int numSquares(); //Random number of squares Boo moves
private:
    static const int PAUSED_ENEMY = 0;
    static const int WALKING_ENEMY = 1;
};


//Derived class for Bowser
class Bowser: public Enemy {
public:
    Bowser (StudentWorld* world, int imageID, int startX, int startY);
    virtual void specMove(); //function specific to Bowser movement
    virtual void paused(Player* p); //Points to player object
    int numSquares(); //Random number of squares Bowser moves
private:
    static const int PAUSED_ENEMY = 0;
    static const int WALKING_ENEMY = 1;
};

class Vortex: public Actor {
public:
    Vortex(StudentWorld* world, int imageID, int startX, int startY, int startDirection);
    void doSomething();
private:
    int walkDir;
};


#endif // ACTOR_H_

