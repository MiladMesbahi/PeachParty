#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath),m_bankAccount(0)
{}

StudentWorld::~StudentWorld()
{
    if(m_actors.size()>0) {
        cleanUp();
    }
}
int StudentWorld::init() //Call this everytime game played. In charge of constructing representation  of the current board in your StudentWorld object and populating it with initial objects.
{
    //First must load board
        string board_file = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
        Board::LoadResult result = bd.loadBoard(board_file);
        if(result ==  Board::load_fail_file_not_found || result == Board::load_fail_bad_format)
            return GWSTATUS_BOARD_ERROR; //Returns GWSTATUS_BOARD_ERROR is board is improperly formatted or missing

    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            switch (bd.getContentsOf(y, x))
            {
                case(Board::empty):
                    break;
                case (Board::player):
                    m_peach = new Player (this, IID_PEACH, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1);
                    m_yoshi = new Player (this, IID_YOSHI, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 2);
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 3));
                    break;
                    
                case(Board::blue_coin_square):
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 3));
                    break;
                case(Board::red_coin_square):
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, -3));
                    break;
                case (Board::star_square):
                    m_actors.push_back(new StarSquare(this, IID_RED_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                    break;
                case(Board::up_dir_square):
                    m_actors.push_back(new Directional(this,IID_DIR_SQUARE, x*SPRITE_WIDTH,y*SPRITE_HEIGHT,Actor::up));
                    break;
                case (Board::right_dir_square):
                    m_actors.push_back(new Directional(this,IID_DIR_SQUARE, x*SPRITE_WIDTH,y*SPRITE_HEIGHT,Actor::right));
                    break;
                case(Board::down_dir_square):
                    m_actors.push_back(new Directional(this,IID_DIR_SQUARE, x*SPRITE_WIDTH,y*SPRITE_HEIGHT,Actor::down));
                    break;
                case(Board::left_dir_square):
                    m_actors.push_back(new Directional(this,IID_DIR_SQUARE, x*SPRITE_WIDTH,y*SPRITE_HEIGHT,Actor::left));
                    break;
                case(Board::bank_square):
                    m_actors.push_back(new BankSquare(this, IID_BANK_SQUARE,x*SPRITE_WIDTH,y*SPRITE_HEIGHT));
                    break;
                case (Board::event_square):
                    m_actors.push_back(new EventSquare(this,IID_EVENT_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                    break;
                case (Board::bowser):
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 3));
                    m_actors.push_back(new Bowser(this,IID_BOWSER,x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                    break;
                case (Board::boo):
                    m_actors.push_back(new CoinSquare(this,IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT,3));
                    m_actors.push_back(new Boo(this,IID_BOO, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                    break;
            }
        }
    }
    startCountdownTimer(99);
    return GWSTATUS_CONTINUE_GAME;
    
}

int StudentWorld::move()
{
    m_peach->doSomething(); //ask peach to do something
    m_yoshi->doSomething(); //ask yoshi to do something
    for(int i=0; i<m_actors.size(); i++)
    {
        m_actors[i]->doSomething(); //goes for all actors to do something
    }
    
    for(vector<Actor*>::iterator a = m_actors.begin(); a < m_actors.end();)
    {
        if(!((*a)->actorAlive())){
            Actor* nextDeleted = (*a);
            a = m_actors.erase(a);
            delete nextDeleted;
        }
        else a++;
    }
    
    string player1Stats = "P1 Roll: " + to_string(m_peach->getRoll()) + " Stars: " + to_string(m_peach->getStars()) + " $$: " + to_string(m_peach->getCoins());
    m_peach->hasVortex() ? player1Stats+=" VOR | " : player1Stats+=" | ";

    string player2Stats = "P2 Roll: " + to_string(m_yoshi->getRoll()) + " Stars: " + to_string(m_yoshi->getStars()) + " $$: " + to_string(m_yoshi->getCoins());
    if(m_yoshi->hasVortex()) player2Stats+=" VOR";

    setGameStatText(player1Stats + "Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(getBBalance()) + " | " + player2Stats );
    
    if (timeRemaining() <= 0){
        playSound(SOUND_GAME_FINISHED); //Play sound when game ends
        
        int peachStars = m_peach->getStars(); //
        int peachCoins = m_peach->getCoins(); //Number of coins/stars peach has at end of game
        int yoshiStars = m_yoshi->getStars();
        int yoshiCoins = m_yoshi->getCoins(); //Number of coins/stars yoshi has at end of game
        
      //Yoshi and peach equal in stars
        if(peachStars==yoshiStars){
            // if also equal in coins, random winner chosen
            if(peachCoins==yoshiCoins){
                int winner = randInt(1,2);
                setFinalScore(peachStars, peachCoins);
                return (winner==1) ? GWSTATUS_PEACH_WON : GWSTATUS_YOSHI_WON;
            }
         
            if (peachCoins > yoshiCoins)//winner decided by who has more coins
            {
                setFinalScore(peachStars, peachCoins);
                return GWSTATUS_PEACH_WON;
            }
            else {
                setFinalScore(yoshiStars, yoshiCoins);
                return GWSTATUS_YOSHI_WON;
            }
        }
        else{
            if(peachStars > yoshiStars) //winner decided by who has more stars
            {
                setFinalScore(peachStars, peachCoins);
                return GWSTATUS_PEACH_WON;
            }
            else {
                setFinalScore(yoshiStars, yoshiCoins);
                return GWSTATUS_YOSHI_WON;
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::EmptySquare(int x, int y) //Check to see if square is empty
{
    if(bd.getContentsOf(x, y) == Board::empty) return true;
    return false;
}

Player* StudentWorld::pPeach()
{
    return m_peach;
}

Player* StudentWorld::pYoshi()
{
    return m_yoshi;
}

int StudentWorld::getBBalance() const{
    return m_bankAccount;
}

void StudentWorld::setBBalance(int amount)
{
    m_bankAccount = amount;
}

void StudentWorld::ChangeSquareToDrop(int x, int y)
{
    for(int i=0;i<m_actors.size();i++) //If actor is directly below bowser
    {
        if(!m_actors[i]->isPlayerEnemy() && m_actors[i]->getX()/16 == x && m_actors[i]->getY()/16 == y)
        {
            m_actors[i]->setDead();
            break;
        }
    }
    m_actors.push_back(new DroppingSquare(this,IID_DROPPING_SQUARE, x*SPRITE_WIDTH,y*SPRITE_HEIGHT)); //Replaces dead square with a dropping square
    playSound(SOUND_DROPPING_SQUARE_CREATED);
}

Actor* StudentWorld::contactVortex(int x, int y)
{
    Actor* oneActor = nullptr;
    for(int i=0;i<m_actors.size();i++)
    {
        if(m_actors[i]->impactableActor())  //Looking for actors that are impactable
        {
            if(abs(m_actors[i]->getX() - x) < SPRITE_WIDTH && abs(m_actors[i]->getY() - y) < SPRITE_HEIGHT) //If impactable actor made contact with vortex then, return overlapping vortex
            {
                oneActor = m_actors[i];
                break;
            }
        }
    }
    return oneActor;
}

void StudentWorld::pushVortex(int x, int y, int startDirection){
    m_actors.push_back(new Vortex(this, IID_VORTEX, x, y, startDirection));
    playSound(SOUND_PLAYER_FIRE);
}


void StudentWorld::cleanUp()
{
    //Need to delete all players
    delete m_peach;
    delete m_yoshi;
    while(m_actors.size()>0)
    {
        Actor* nextDeleted = m_actors[0];
        m_actors.erase(m_actors.begin());
        delete nextDeleted;
    }
}

