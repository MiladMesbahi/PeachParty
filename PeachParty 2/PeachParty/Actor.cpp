#include "Actor.h"
#include "StudentWorld.h"


//Actor class
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, int startDirection, int depth, double size) : GraphObject(imageID, startX, startY, startDirection, depth, size), m_actorAlive(true) //This is the constructor implementation for the Actor class. The constructor initializes the GraphObject base class with these parameters using the initializer list. It also sets the m_isAlive boolean data member to true.
{
    m_world = world;
}

StudentWorld* Actor::getWorld() const //This function returns a pointer to the StudentWorld object that the Actor belongs to.
{
    return m_world;
}

bool Actor::actorAlive() const
{
    return m_actorAlive; // Base implementation, assume all Actors are alive
}

bool Actor::isPlayerEnemy()
{
    return false; //Default to false so dont have to turn them off for all objects that arent Peach Yoshi Bowser Boo
    
}

void Actor::setDead()
{
    m_actorAlive = false;  //Sets actor to dead if function called
}

bool Actor::impactableActor() const {
    return false;
}
void Actor::ifHit(){}

//AvatarMovement Class

AvatarMovement::AvatarMovement(StudentWorld* world, int imageID, int startX, int startY)
: Actor(world, imageID, startX, startY, right, 0, 1), m_walkDir(right), m_state(DEFAULT), m_ticksLeft(0){}

bool AvatarMovement::isPlayerEnemy()
{
    return true;
}

int AvatarMovement::moveRandDirection()
{
    int dir;
    do
    {
        dir = randInt(0, 3) * 90;
    }
    while (!openDirection(dir));
    return dir;
    }

bool AvatarMovement::openDirection(int direction) {
    int newX, newY; //New location to move
    getPositionInThisDirection(direction, 16, newX, newY);
    newX /=16; //must divide both values by 16 to convert from pixel to get Board coordinates
    newY /=16;
    return !getWorld()->EmptySquare(newX,newY);
}
void AvatarMovement::corner() {
    if(getWalkDir() == right || getWalkDir() == left) openDirection(up) ? setWalkDir(up) : setWalkDir(down);
        else if(getWalkDir() == up || getWalkDir() == down) openDirection(right) ? setWalkDir(right) : setWalkDir(left);
    }
bool AvatarMovement::fork()
{
    int numValidDirs = 0; //Number of valid directions
    if(openDirection(up)) numValidDirs++;
    if(openDirection(right)) numValidDirs++;
    if(openDirection(down)) numValidDirs++;
    if(openDirection(left)) numValidDirs++;
    if(numValidDirs>=3) return true;
        return false;
}

void AvatarMovement::moveAvatar(){
    moveAtAngle(m_walkDir, 2); // Moves Avatar 2 pixels when called
    setTicks(getTicks()-1); //Decrement ticks to move count by 1
    if(getTicks() == 0)
    {
        specMove();
    }
}
void AvatarMovement::teleport(){
    int x,y; //New x,y coorindaye until no longer empty
    do{
        x = randInt(0, 15);
        y = randInt(0, 15);
    }
    while (getWorld()->EmptySquare(x,y) || (x==getX()/16 && y==getY()/16));
    moveTo(x*16, y*16);
    }

void AvatarMovement::setWalkDir(int dir)
{
    m_walkDir = dir;
    (dir == left) ? setDirection(left) : setDirection(right); //Set sprite to 180 when walking left or 0 otherwise
}

int AvatarMovement::getWalkDir() const
{
    return m_walkDir;
}

void AvatarMovement::setState(int state)
{
    m_state = state;
}

int AvatarMovement::getState() const
{
    return m_state;
}

void AvatarMovement::setTicks(int amount)
{
    m_ticksLeft = amount;
}

int AvatarMovement::getTicks() const
{
    return m_ticksLeft;
}

//Player Class

Player::Player(StudentWorld* world, int imageID, int startX, int startY, int playerNum) :AvatarMovement(world, imageID, startX, startY), m_roll(0), m_num_coins(0),m_num_stars(0), m_vortex(false), m_justLanded(false), m_onIt(false), m_onDirSquare(false), m_teleported(false), m_playerNum(playerNum){}

void Player::doSomething()
{
    if(getState()== WAITING_TO_ROLL)
    {
        if(m_teleported && !openDirection(getWalkDir()))
        {
            int dir = moveRandDirection();
            setWalkDir(dir);
            setTeleport(false);
        }
        int userInput = getWorld()->getAction(m_playerNum);
        //User deciding to roll
        if(userInput==ACTION_ROLL){
            m_roll = randInt(1,10); //Ramndom 10 sided die roll
            setTicks(m_roll*8); //Ticks need to change to move
            setState(WALKING_ON_SQUARES); //Change player state to walking
            m_justLanded = false; //Player no longer on square
            m_onIt = false; //Resets for square interaction
        }
        //If user decides to shoot vortex
        else if(hasVortex() && userInput==ACTION_FIRE)
        {
            getWorld()->pushVortex(getX(), getY(), getWalkDir());//Creates vortex and plays sound
            setVortex();//Updates player to no longer have vortex
        }
        else return; //User doesn't press key, or presses another key
    }
    if(getState()==WALKING_ON_SQUARES)
    {
        if (getX()%16==0 && getY()%16==0)
        {
            if(m_onDirSquare)
            {
                setPlayerOnDirSquare(false);
            }
            else if(fork())
            {
                int dir = getWorld() ->getAction(m_playerNum);
                switch(dir)
                {
                    case(ACTION_UP):
                    {
                        dir = up;
                        break;
                    }
                    case(ACTION_RIGHT):
                    {
                        dir = right;
                        break;
                    }
                    case(ACTION_DOWN):
                    {
                        dir = down;
                        break;
                    }
                    case(ACTION_LEFT):
                    {
                        dir = left;
                        break;
                    }
                    default:  //No selection
                    {
                        dir = -1;
                        break;
                    }
                }
                if(dir != -1 && openDirection(dir))
                {
                    setWalkDir(dir);
                }
                else return;
            }
            else if(!openDirection(getWalkDir()))
            {
                corner();
            }
        }
        moveAvatar();
    }
}

void Player::specMove()
{
    setState(WAITING_TO_ROLL); //change state to waiting to roll once tick moves to zero
    m_justLanded = true; //Lands on square
}

int Player::isWaiting() const
{
    return getState() == WAITING_TO_ROLL;
}
bool Player::squareLanded() const
{
    return m_justLanded;
}
bool Player::onSquare() const
{
    return m_onIt;
}

int Player::getStars() const
{
    return m_num_stars;
}

int Player::getCoins() const
{
    return m_num_coins;
}

bool Player::hasVortex() const
{
    return m_vortex;
}

int Player::getRoll() const
{
    return m_roll;
}

void Player::isLocation(bool loc)
{
    m_onIt = loc;
}

void Player::setStars(int amount)
{
    m_num_stars += amount;
}

void Player::setCoins(int amount)
{
    m_num_coins += amount;
}

void Player::setVortex()
{
    m_vortex = !m_vortex;
}

void Player::setRoll(int numRolls)
{
    m_roll = numRolls;
}

void Player::setPlayerOnDirSquare(bool dir)
{
    m_onDirSquare = dir;
}

void Player::setTeleport(bool tel)
{
    m_teleported = tel;
}

//SQUARE CLASS
Square::Square(StudentWorld* world, int imageID, int startX, int startY):Actor(world, imageID,startX,startY,right,1,1){}

void Square::doSomething()
{
    if(!actorAlive()) //Is square alive?
    {
        return;
    }
    if(getX()==getWorld()->pPeach()->getX() && getY()==getWorld()->pPeach()->getY()) //Checks if Peach traverses square
    {
        if(getWorld()->pPeach()->squareLanded()) //Peach lands on square
        {
            if(!getWorld()->pPeach()->onSquare())
            {
                getWorld()->pPeach()->isLocation(true); //Peach Activated
                
                landSquare(getWorld()->pPeach());
            }
        }
        else traverseSquare(getWorld()->pPeach());
    }

    if(getX()==getWorld()->pYoshi()->getX() && getY()==getWorld()->pYoshi()->getY())    //Check is Yoshi traverses a square
    {
        
        if(getWorld()->pYoshi()->squareLanded())//yoshi lands on square
        {
    
            if(!getWorld()->pYoshi()->onSquare())
            {
                getWorld()->pYoshi()->isLocation(true);
                
                landSquare(getWorld()->pYoshi());
            }
        }
        else traverseSquare(getWorld()->pYoshi());
    }
}

int Boo::numSquares()
{
    return randInt(1,3);
}

//COIN SQUARE

CoinSquare::CoinSquare(StudentWorld* world, int imageID, int startX, int startY, int amount):Square(world,imageID, startX, startY), m_adjustCoinCount(amount){}

void CoinSquare::landSquare(Player* p)
{
    if(m_adjustCoinCount>0)
    {
        p->setCoins(m_adjustCoinCount);  //Blue Coin adds 3 coins
        getWorld()->playSound(SOUND_GIVE_COIN); //Blue Square Sound
    }
    else{
        
        int pCoins = p->getCoins(); //red coin square cant go below 0
        pCoins < 3 ? p->setCoins(-pCoins) : p->setCoins(m_adjustCoinCount);
        getWorld()->playSound(SOUND_TAKE_COIN); //Red Square Sound
    }
}
void CoinSquare::traverseSquare(Player *p){}

//STAR SQUARE
StarSquare::StarSquare(StudentWorld* world, int imageID, int startX, int startY):Square(world,imageID, startX, startY){}

void StarSquare::landSquare(Player *p)
{
    if(p->getCoins()<20) return;//If can't purchase star then immediately return
    p->setCoins(-20); //If can, then purchase star and deduct 20 coins from balance
    p->setStars(1);
    getWorld()->playSound(SOUND_GIVE_STAR); //Star sound
}
void StarSquare::traverseSquare(Player* p)
{
    landSquare(p);
}

//DIRECTIONAL SQUARE
Directional::Directional(StudentWorld* world, int imageID, int startX, int startY, int nDir):Square(world,imageID, startX, startY), chosenDirection(nDir)
{
    setDirection(nDir);
}

void Directional::landSquare(Player* p)
{
    p->setWalkDir(chosenDirection); //Change direction and state on directional square
    p->setPlayerOnDirSquare(true);
}

void Directional::traverseSquare(Player* p)
{
    landSquare(p);
    
}

//BANK SQUARE

BankSquare::BankSquare(StudentWorld* world, int imageID, int startX, int startY):Square(world,imageID, startX, startY){}

void BankSquare::landSquare(Player* p)
{
    p->setCoins(getWorld()->getBBalance()); //Add coins
    getWorld()->setBBalance(0); //Reset bank
}

void BankSquare::traverseSquare(Player* p)
{
    int pBalance = p->getCoins();
    if(pBalance<5)
    {
       
        p->setCoins(-pBalance);  //Sets players coins to 0
      
        getWorld()->setBBalance(getWorld()->getBBalance()+pBalance); //Coins to central bank
        getWorld()->playSound(SOUND_DEPOSIT_BANK); //Bank sound deposit
    }
    else
    {
        p->setCoins(-5); //Deduct 5 coins
        getWorld()->setBBalance(getWorld()->getBBalance()+5); //Place these 5 coins in bank
        getWorld()->playSound(SOUND_WITHDRAW_BANK); //Bank sound withdrawl
    }
}

EventSquare::EventSquare(StudentWorld* world, int imageID, int startX, int startY):Square(world,imageID, startX, startY){}

void EventSquare::landSquare(Player* p)
{
    int rEvent = randInt(1,3); //1/3 chance for 3 cases
    switch(rEvent){
        case 1: //Teleport to random square
        {
            p->teleport();
            getWorld()->playSound(SOUND_PLAYER_TELEPORT); //teleport sound
            p->setTeleport(true);
            break;
        }
        case 2: //Swap player positions, ticks, direction, sprite and state on board
        {
            Player* player1 = getWorld()->pPeach();
            Player* player2 = getWorld()->pYoshi();
            
            int tempX = player1->getX(); //Temp values for player 1 to store
            int tempY = player1->getY();
            int tempNumTicks = player1->getTicks();
            int tempDirection = player1->getWalkDir();
            int tempRoll = player1->getRoll();
            int tempState = player1->getState();
            
   
            player1->moveTo(player2->getX(),player2->getY()); //Swap player 1 with 2
            player1->setTicks(player2->getTicks());
            player1->setWalkDir(player2->getWalkDir());
            player1->setRoll(player2->getRoll());
            player1->setState(player2->getState());
            
            player2->moveTo(tempX,tempY); //Swap player 2 with 1
            player2->setTicks(tempNumTicks);
            player2->setWalkDir(tempDirection);
            player2->setRoll(tempRoll);
            player2->setState(tempState);
        
            if(p==player1) //Can't reactivate event square after swap
            {
                player1->isLocation(false);
                player2->isLocation(true);
            }
            else{
                player1->isLocation(true);
                player2->isLocation(false);
            }
            
            //Play teleport sound
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
            break;
        }
        case 3: //Vortex case
        {
            if(!p->hasVortex()) //Give player vortex if they dont have one already
            {
                p->setVortex();
            }
            getWorld()->playSound(SOUND_GIVE_VORTEX);//Vortex sound
            break;
        }
    }
}
void EventSquare::traverseSquare(Player* p){}

DroppingSquare::DroppingSquare(StudentWorld* world, int imageID, int startX, int startY):Square(world,imageID, startX, startY){}

void DroppingSquare::landSquare(Player* p)
{
    int randomOption = randInt(1,2); // 50/50 chance
    switch(randomOption){
        case 1: //Deduct as many coins as possible up to 10
        {
            int pBalance = p->getCoins();
            pBalance<10 ? p->setCoins(-pBalance) : p->setCoins(-10);
            break;
        }
        case 2: //Deduct star if they have at least one
        {
            if(p->getStars()>0) p->setStars(-1);
        }
    }
    getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE); //Dropping sound
}

void DroppingSquare::traverseSquare(Player* p){}

Enemy::Enemy(StudentWorld* world, int imageID, int startX, int startY):AvatarMovement(world, imageID, startX, startY), m_distTravel(0), m_pauseCounter(180), m_squaresToMove(0), m_contactPeach(false), m_contactYoshi(false){}

void Enemy::doSomething()
{
    if(getState()==PAUSED_ENEMY) //When paused
    {
        if(getX()==getWorld()->pPeach()->getX() && getY()==getWorld()->pPeach()->getY() && getWorld()->pPeach()->isWaiting()) //Enemy on square that Peach is waiitng on
        {
    
            if(!getPeach()) //Is peach new?
            {
                paused(getWorld()->pPeach());
                setPeach(true); //Records that peach and enemy have interacted
            }
        }
        else setPeach(false); //Peach leaves
        
        if(getX()==getWorld()->pYoshi()->getX() && getY()==getWorld()->pYoshi()->getY() && getWorld()->pYoshi()->isWaiting()) //Same thing as above but with Yoshi
        {
        
            if(!getYoshi())
            {
                paused(getWorld()->pYoshi());
                setYoshi(true);
            }
        }
        else setYoshi(false); //Yoshi leaves
        
        m_pauseCounter--;
        if(m_pauseCounter==0)
        {
            m_squaresToMove = numSquares();
            setTicks(m_squaresToMove*8);

            int dir = moveRandDirection();
            setWalkDir(dir);

            
            setState(WALKING_ENEMY); //Change state
            setPeach(false);
            setYoshi(false);
        }
    }
    if(getState()==WALKING_ENEMY) //If Enemy is walking
    {
        if(getX()%16==0 && getY()%16==0 && fork()) //on Top of square
        {
            int dir = moveRandDirection(); //Pick Random direction to walk in
            setWalkDir(dir);
        }
        else if(getX()%16==0 && getY()%16==0 && !openDirection(getWalkDir())) //can't move in direction facing
        {
            corner();
        }
        //Move avatar
        moveAvatar();
    }
}

bool Enemy::getPeach() const
{
    return m_contactPeach;
}

bool Enemy::getYoshi() const{
    return m_contactYoshi;
}

bool Enemy::impactableActor() const
{
    return true;
}

void Enemy::setPeach(bool touch)
{
    m_contactPeach = touch;
}

void Enemy::setYoshi(bool touch)
{
    m_contactYoshi = touch;
}

void Enemy::setPauseCounter(int amount)
{
    m_pauseCounter = amount;
}

void Enemy::getHit()
{
    teleport(); //Teleport to another square
    setWalkDir(right);
    setState(PAUSED_ENEMY);
    setPauseCounter(180);
}
Bowser::Bowser(StudentWorld* world, int imageID, int startX, int startY): Enemy(world, imageID, startX, startY){}
void Bowser::paused(Player* p)
{
    int fiftyFifty = randInt(1,2); //coin flip chance
    if(fiftyFifty==1)
    {
        p->setCoins(-p->getCoins()); //Player loses coins and stars
        p->setStars(-p->getStars());
        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
    }
}
void Bowser::specMove()
{
    setState(PAUSED_ENEMY);
    setPauseCounter(180);
    int outcome = randInt(1,4); //Simulate 1/4 chance
    if(outcome==1){
        getWorld()->ChangeSquareToDrop (getX()/16, getY()/16);
    }
}

int Bowser::numSquares()
{
    return randInt(1,10);
}

//BOO CLASS

Boo::Boo(StudentWorld* world, int imageID, int startX, int startY): Enemy(world, imageID, startX, startY){}

void Boo::paused(Player* p){
    
    Player* player1 = getWorld()->pPeach();
    Player* player2 = getWorld()->pYoshi();

    int fiftyFifty = randInt(1,2);
    switch(fiftyFifty)
    {
        case 1: //Swap coin value of players
        {
            int tempCoin = player1->getCoins();
            player1->setCoins(-tempCoin+player2->getCoins());
            player2->setCoins(-player2->getCoins()+tempCoin);
            break;
        }
        case 2: //Swap players stars
        {
            int tempStar = player1->getStars();
            player1->setStars(-tempStar+player2->getStars());
            player2->setStars(-player2->getStars()+tempStar);
            break;
        }
    }
}
void Boo::specMove()
{
    setState(PAUSED_ENEMY);
    setPauseCounter(180);
}

//VORTEX CLASS

Vortex::Vortex(StudentWorld* world, int imageID, int startX, int startY, int startDirection):
Actor(world, imageID, startX, startY, right, 0, 1), walkDir(startDirection){}

void Vortex::doSomething()
{
    if(!actorAlive()) return;
    moveAtAngle(walkDir, 2); //2 pixels in direction
    if(getX()<0 || getX()>=VIEW_WIDTH || getY()<0 || getY()>=VIEW_HEIGHT)
    {
        setDead(); //sets vortex to dead if out of bounds
        return;
    }
    Actor* impactableActor = getWorld()->contactVortex(getX(), getY());//any overlap with actor that is impactable
    if(impactableActor != nullptr)
    {
        impactableActor->ifHit();
        setDead();
        getWorld()->playSound(SOUND_HIT_BY_VORTEX); //Vortex sound
    }
}
