
#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector> // added

using namespace std;

// Create a struct for a ship
struct Ship {
    Ship(string shipName, int shipID, int shipLength, char shipSymbol) : ship_name(shipName), ship_id(shipID), ship_length(shipLength), ship_symbol(shipSymbol) {

        //Check for validity in ship length
        if (ship_length > 0 /*&& ship_length < MAXROWS && ship_length < MAXCOLS*/) {
            validLength = true;
        }
        else {
            validLength = false;
        }

    }

    string ship_name;
    int ship_id;
    int ship_length;
    char ship_symbol;
    bool validLength;
    bool getValidLength() const {
        return validLength;
    }
};


class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);

    // Added
    private:
    vector<Ship *> m_ships; // Creates a pointer to each ship
    int m_rows;
    int m_cols;

};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols) : m_rows(nRows), m_cols(nCols) // added
{
//     make sure doesnt exceed rows/cols
    if (nRows > MAXROWS || nCols > MAXCOLS) {
        exit (1);
    }

}

int GameImpl::rows() const
{
    return m_rows; // added
}

int GameImpl::cols() const
{
    return m_cols; // added
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    // Creates new ship
  //  Ship(string shipName, int shipID, int shipLength, char shipSymbol)
        Ship * added_ship = new Ship(name, m_ships.size(), length, symbol); // call vector's size
    
        if (added_ship->getValidLength() == true) {
            m_ships.push_back(added_ship); // add the ship to the vector
//            cerr << "added ship! (GameImpl)" << endl;
            return true;  // This compiles but may not be correct
        }
    // CHECK
    return false;
}

int GameImpl::nShips() const
{
    // uses vector's size function -> vector maintains the count of the ship
    return m_ships.size(); // loses integer precision -> doesn't matter?
}

int GameImpl::shipLength(int shipId) const
{
    return m_ships[shipId]->ship_length;
}

char GameImpl::shipSymbol(int shipId) const
{
    return m_ships[shipId]->ship_symbol;
}

string GameImpl::shipName(int shipId) const
{
    return m_ships[shipId]->ship_name;
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{

    // Variable declarations/simplifications to help later (make more coherent)
    int p1shipNums = p1->game().nShips();
    int p2shipNums = p2->game().nShips();
    
    bool gotShot=true, gotHit = true, gotDestroyed=true;
    int shipId = 0;
    
    if (p1->placeShips(b1) == false || p2->placeShips(b2) == false) {
        return nullptr;
    }
    

    while(b1.allShipsDestroyed()==false && b2.allShipsDestroyed() == false)
    {
    
        //First player's turn
        cout << p1->name() << "'s turn. Board for " << p2->name() << ": " << endl;
        b2.display(p1->isHuman());
        
        //HAVE TO CREATE ATTACK COORD OR ELSE IF YOU CALL IT EVERYTIME, IT BECOMES RANDOM
        Point pointOfAttack = p1->recommendAttack();
    
        
        if(b2.attack(pointOfAttack, gotHit, gotDestroyed, shipId)) {
            if(!gotHit) {
                cout << p1->name() << " attacked (" << pointOfAttack.r << "," << pointOfAttack.c << ")" << " and missed, resulting in:" << endl;
            }
            else {
                if(gotDestroyed){ // What about if the boat is entirely SUNK? ->> REMEMBER TO CHECK!!!
                
                    p2shipNums--; // decrement ship count for later
                    cout << p1->name() << " attacked (" << pointOfAttack.r << "," << pointOfAttack.c << ")" <<" and destroyed the " << p2->game().shipName(shipId) << ", resulting in:" << endl;
                    
                    if(p2shipNums == 0){
                        b2.display(p1->isHuman());
                        break;
                    }
                }
                else {
                    cout << p1->name() << " attacked (" << pointOfAttack.r << "," << pointOfAttack.c << ")" << " and hit something, resulting in:" << endl;
                }
            }
            
            //RECORD ATTACKS NOW  after the ATTACK
            p1->recordAttackResult(pointOfAttack, gotShot, gotHit, gotDestroyed, shipId);
            p2->recordAttackByOpponent(pointOfAttack);
        }
        else { // condition: what about if you shoot at a point where you already shot before / hit / missed?
            cout << p1->name() << " wasted a shot at (" << pointOfAttack.r << "," << pointOfAttack.c << ")" << endl;
        }
        
        b2.display(p1->isHuman());
        if(p1->isHuman()) {
            if(shouldPause) {
                waitForEnter(); // call the function for the COUT and IGNORE
            }
        }
        else {
            if(shouldPause) {
                waitForEnter();
            }
        }
        
        cout << endl; // double check if this is needed -> REMEMBER THIS !!!!!!! <---------
        cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
        b1.display(p2->isHuman());
        
        //SET THE POINT AGAIN to make sure that the attack point remains the same
        Point pointOfAttack2 = p2->recommendAttack();
        if(b1.attack(pointOfAttack2, gotHit, gotDestroyed, shipId )) {
            if(!gotHit) {
                cout << p2->name() << " attacked (" << pointOfAttack2.r << "," << pointOfAttack.c << ")" <<" and missed, resulting in:" << endl;
            }
            else {
                if(gotDestroyed) {
                    p1shipNums--;
                    cout << p2->name() << " attacked (" << pointOfAttack2.r << "," << pointOfAttack2.c << ")" << " and destroyed the " << p1->game().shipName(shipId) << ", resulting in:" << endl;
                    if(p1shipNums==0) {
                        b1.display(p2->isHuman());
                        break;
                    }
                }
                else {
                    cout << p2->name() << " attacked (" << pointOfAttack2.r << "," << pointOfAttack2.c << ")" << " and hit something, resulting in:" << endl;
                }
            }
            //Recording again
            p1->recordAttackByOpponent(pointOfAttack2);
            p2->recordAttackResult(pointOfAttack2, gotShot, gotHit, gotDestroyed, shipId);
        }
        else {
            cout << p2->name() << " wasted a shot at (" << pointOfAttack2.r << "," << pointOfAttack2.c << ")" << endl;
        }
        b1.display(p2->isHuman());
        if (p2->isHuman()) {
            if(shouldPause)
            {
                waitForEnter(); // two pasuses? <-- check
            }
        }
        else {
            if(shouldPause)
            {
                waitForEnter();
            }
        }
 
    }
    
// Deciding on the winner now
    Player * winner = nullptr;
      // Checking for Wins Now
      if (p1shipNums == 0) {
          winner = p2;
          cout << p2->name() << " wins!" << endl;
//          cout << "Here is where " << p2->name() << "'s ships were:" << endl;
//          b2.display(p1->isHuman());
          // Am I suppose to display the board after a win?
          // Feel like Smallberg's example .exe sometimes displays it but sometimes it doesn't
          // maybe don't include just to be safe? SO WEIRD??
          // How to display the board placements without the shots???
          
      }
      if (p2shipNums == 0 ) {
          winner = p1;
          cout << p1->name() << " wins!" << endl;
//          cout << "Here is where " << p1->name() << "'s ships were:" << endl;
//          b1.display(p2->isHuman());
      }
      return winner;
} // closing

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

