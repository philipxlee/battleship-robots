#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
//added
#include <stack>
#include <vector>
using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class HumanPlayer : public Player {
public:
    HumanPlayer (string nm, const Game & g);
    virtual ~HumanPlayer();
    virtual bool isHuman() const;
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
};

HumanPlayer::HumanPlayer(string nm, const Game &g) : Player(nm, g) {
}

HumanPlayer::~HumanPlayer() {
}

bool HumanPlayer::isHuman() const {
    return true; // returns true because HumanPlayer is indeed a human
}

bool HumanPlayer::placeShips(Board& b) {
    
    bool valid = false;
    string direction;
    
    for (int i = 0; i < game().nShips(); i++) {
        cout << Player::name() << " there are " << game().nShips()-i << " ships not placed." << endl;
        b.display(false);
        
        //Keep asking the human to enter a valid direction, -> loop?
        while (valid == false) {
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
         
            cin >> direction;
//            cin.ignore(10000,'\n'); // why isn't the else statement taking into account just an enter?? tf
            if (direction == "v" || direction == "h") {
                valid = true;
            }
//            else if (direction == "\n") {
//                cout << "Direction must be h or v." << endl;
//            }
            else {
                cout << "Direction must be h or v." << endl;
            }
        } // while loop closing
        
        //Direction implementation
        Direction dir;
        if (direction == "h") {
            dir = HORIZONTAL;
        }
        else {
            dir = VERTICAL;
        }
        valid = false;
        while (valid == false) {
            cout << "Enter row and column of ";
            if (dir == VERTICAL) {
                cout << "topmost cell (e.g. 3 5): ";
            }
            
            if (dir == HORIZONTAL) {
                cout << "leftmost cell (e.g. 3 5): ";
            }
            
            int r = 0, c = 0;
            cin >> r >> c;
            cin.clear(); // prevent errors later
            if (b.placeShip(Point(r,c), i, dir) == false) {
                cout << "This ship cannot be placed there." << endl;
            }
            else {
                valid = true;
            }
        } // while loop closing
        valid = false;
    }
    return true;
}

Point HumanPlayer::recommendAttack() {
    Point recommendedPoint;
    int r = -1, c = -1;
    int greenLight = 0; // once it hits 1 then go!
    while (greenLight == 0) {
        cout << "Enter row and column of leftmost cell (e.g. 3 5): ";
        cin >> r >> c;
        if (r > game().rows() || r < 0 || c > game().cols() || c < 0) {
            // This is bad -> how to check? Maybe just leave it be and prompt for another??
            cout << "You must enter two integers.";
        }
        else {
            greenLight++;
        }
    }
    return Point(r, c);
}


void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {
    //Should not need to do anything else
}

void HumanPlayer::recordAttackByOpponent(Point p) {
    //Should not need to do anything else
}




//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.


// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

class MediocrePlayer : public Player {
public:
  // First it must call the block fucntion --> REMEMBER TO UNBLOCK BEFORE RETURN
    MediocrePlayer(string nm, const Game & g);
    virtual ~MediocrePlayer();
//    virtual bool isHuman() const; --> should not be needed
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    
    // Added
    bool placeShipsRecursion(Board& b, Point coord, int shipId, int shipsNum, Direction dir, vector<Point> pointsAdded, vector<Direction>dirVec, bool backTrack);
    void designAttackPoints(Point coord);
    Point designedShot();
    
private:
    bool designedPoints;
    char m_boardHistory[MAXROWS][MAXCOLS];
    int calcOrRandom; // player can either randomly fire or shoot by design (cuz mediocre)
    Point m_firedCoord; // stores where the hit was at to help with history of the board
    vector<Point> m_pointsFree; // stores where on the board can you shoot validly (no blocks etc)
    vector<Point> m_pointsOfDesignedAttack; //stores where on the board the player can shoot on purpose (to make sure mediocre player is at least somewhat good)
};

// RECOMMENDED BY TA: use auto to take in any int (rely on C++)
void deletePoint(Point coord, vector<Point>& vec)
{
    for ( auto q = vec.begin(); q != vec.end(); )
    {
        if (q->r == coord.r && q->c == coord.c)
            q = vec.erase(q);
        else
            q++;
    }
}

// REMEMBER TO INTIALIZE OTHER PRIVATES!!!!
MediocrePlayer::MediocrePlayer(string nm, const Game & g) : Player(nm,g), designedPoints(false), calcOrRandom(1), m_firedCoord(0,0), m_pointsFree({}), m_pointsOfDesignedAttack({})  {
    // initialize the board history -> RECALL PROJECT 1 FOR HISTORY
    for (int r = 0; r < game().rows(); r++) {
        for (int c = 0; c < game().cols(); c++) {
            m_pointsFree.push_back(Point(r,c)); // store each point as somewhere you can shoot
            m_boardHistory[r][c] = '.';
        }
    }
}

MediocrePlayer::~MediocrePlayer() {
    
}

//Design the auxillary function first
bool MediocrePlayer::placeShipsRecursion(Board &b, Point coord, int shipId, int shipsNum, Direction dir, vector<Point> pointsAddedVec, vector<Direction> dirVec, bool backTrack) {

    // CHECK: if all ships are placed, then return true
    if (shipsNum == 0) {
        return true;
    }

    if (game().cols()-1 < coord.c) {
        coord.c = 0;
        coord.r++;
    }

    if (coord.c == 0 && game().rows()-1 < coord.r) {
        backTrack = true;
    }

    if (backTrack && pointsAddedVec.size() == 0) {
        return false;
    }

    // However, if there is no backtracking
    bool canPlace;
    if (!backTrack) {
        canPlace = b.placeShip(coord, shipId, dir);
        if (canPlace) {
            // Vector stores the point to place the ships and the directions
            dirVec.push_back(dir);
            pointsAddedVec.push_back(coord);

            //RECUR SION HERE?
            // Simplifying step: move along the vector and decrease the avaliable shifts
            return placeShipsRecursion(b, Point(0,0), shipId + 1, shipsNum - 1, HORIZONTAL, pointsAddedVec, dirVec, backTrack);
        }
        else {
            if (dir == HORIZONTAL) {
                dir = VERTICAL;
                return placeShipsRecursion(b, coord, shipId, shipsNum, dir, pointsAddedVec, dirVec, backTrack);
            }
            else {
                return placeShipsRecursion(b, Point(coord.r,coord.c + 1), shipId, shipsNum, HORIZONTAL, pointsAddedVec, dirVec, backTrack);
            }
        }
    }

    // Backtracking
    pointsAddedVec.pop_back(); // remove ship
    dirVec.pop_back(); // remove direction
    // unplacing the ship:
    canPlace = b.unplaceShip(Point(pointsAddedVec.back().r, pointsAddedVec.back().c), shipId - 1 , dirVec.back());

    backTrack = false; // stop backtracking for next iterate
    if (dirVec.back() == HORIZONTAL) {
        return placeShipsRecursion(b, Point(pointsAddedVec.back().r, pointsAddedVec.back().c), shipId - 1, shipsNum + 1, VERTICAL, pointsAddedVec, dirVec, backTrack);
    }
    return placeShipsRecursion(b, Point(pointsAddedVec.back().r, (pointsAddedVec.back().c) + 1), shipId - 1, shipsNum + 1, HORIZONTAL, pointsAddedVec, dirVec, backTrack);

}

bool MediocrePlayer::placeShips(Board& b) {
   // CALL BLOCK FIRST
    bool canPlace = false;

    // CONDITION: it must at least try backtrackign 50 times or placing the ship 50 times before saying you cant place ships -> While loop perhaps or for loop? for loop for simplicity
        for (int i = 0; i < 50; i++) {
            if (!canPlace) {
            // CALL BLOCK
            b.block();

            //CALL THE RECURSION HERE --> DOUBLE CHECK!!!
            canPlace = placeShipsRecursion(b, Point(0,0), 0, game().nShips(), HORIZONTAL, {}, {}, false);

            //UNBLOCK
            b.unblock();
            }
        }
    return canPlace; // false if 50 tries dont work
}

Point MediocrePlayer::designedShot() {
   
    if (designedPoints)
            designAttackPoints(m_firedCoord);
        int i = randInt(m_pointsOfDesignedAttack.size()); // KEY MISTAKE FIXED HAHA
        Point coord(m_pointsOfDesignedAttack[i].r, m_pointsOfDesignedAttack[i].c);
    //Remove the point from vector
        deletePoint(coord, m_pointsOfDesignedAttack);
        if (m_pointsOfDesignedAttack.empty())
            calcOrRandom = 1;
        return coord;
}

void MediocrePlayer::designAttackPoints(Point coord) {
    // reset the vector before adding new things into it
      m_pointsOfDesignedAttack.clear();
    
    
 // if the point is okay, then vector it
      for (int i = 1; i < 5; i++)
      {
          if (coord.c+i <= game().cols()-1 && m_boardHistory[coord.r][coord.c+i] == '.') {
              m_pointsOfDesignedAttack.push_back(Point(coord.r, coord.c+i));
          }
          
          else if (coord.r+i <= game().rows()-1 && m_boardHistory[coord.r+i][coord.c] == '.') {
              m_pointsOfDesignedAttack.push_back(Point(coord.r+i, coord.c));
          }
          
          else if (coord.r-i >= 0 && m_boardHistory[coord.r-i][coord.c] == '.') {
              m_pointsOfDesignedAttack.push_back(Point(coord.r-i, coord.c));
          }
          
          else if (coord.c-i >= 0 && m_boardHistory[coord.r][coord.c-i] == '.') {
              m_pointsOfDesignedAttack.push_back(Point(coord.r, coord.c-i));
          }
      }
  designedPoints = false;
}

Point MediocrePlayer::recommendAttack() {
    if (m_pointsFree.empty()) {
        cerr << "error";
    }

    if (calcOrRandom == 1) {
        // Remember: 1 = random, so call Rand Int
        //
//        Point coord(m_pointsFree[randInt(m_pointsFree.size())].r, m_pointsFree[randInt(m_pointsFree.size())].c); -->> KEY MISTAKE FOUND LOL jesus this took years of debugging
        // REMEMBER TO SET THE POINT OF RAND INT FIRST OTHERWISE IT's JUST CALLING A RANDOM INT EVERYTIME WITHIN THIS SQUARE BRACKETS
        int i = randInt(m_pointsFree.size());
        Point coord(m_pointsFree[i].r, m_pointsFree[i].c);

        //Remove the point from vector
        deletePoint(coord, m_pointsFree);

        return coord;
    }
    else {
        Point coord = designedShot();
        deletePoint(coord, m_pointsFree); // call delete - ta
        
        return coord;
    }
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {
   // mark the board
    if (!shotHit) {
        m_boardHistory[p.r][p.c] = 'o';
    }
    else {
        m_boardHistory[p.r][p.c] = 'X';
    }
 
    if (calcOrRandom == 1) {
        if (!shipDestroyed && shotHit) {
            calcOrRandom = 2;
            designedPoints = true;
            m_firedCoord = p;
        }
    }
    else {
        if (shipDestroyed && shotHit) {
            calcOrRandom = 1;
        }
    }
} // closing void

void MediocrePlayer::recordAttackByOpponent(Point p) {
    // dont do anything
}



//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class GoodPlayer : public Player {
public:
    GoodPlayer(string nm, const Game& g);
    virtual ~GoodPlayer();
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    
    // plan: maybe use stacks (imitate the solving maze recursion or maybe just loops) to see if the ship is there or not after a hit?
    // will probably need a history board like Mediocre Player and vector that stores the points.
    // double check nachenbburg slides -> reminder
    
    //Added
    void designAttackPoints(Point coord);
    
  private:
    //Added
    char m_boardHistory[MAXROWS][MAXCOLS];
    vector<Point> m_pointsFree;
    stack<Point> m_pointsOfDesignedAttack;
    int calcOrRandom; // like before -> changes states required in spec
};

GoodPlayer::GoodPlayer(string nm, const Game& g) : Player (nm, g), calcOrRandom(1) { // set the state --> SO IMPORTANT I ALMOST FORGOT
    for (int r = 0; r < game().rows(); r++) {
        for (int c = 0; c < game().cols(); c++) {
            m_boardHistory[r][c] = '.';
            m_pointsFree.push_back(Point(r,c));
        }
    }
}

GoodPlayer::~GoodPlayer() {
    
}

void GoodPlayer::designAttackPoints(Point coord) {
  
  
    // IMPLEMENT MAZE LIKE ALGORITHM
    // CHECK CAREY NACHENBERGS SLIDES

    // If NORTH is valid, add it to the stack
    // check for out of scope coordinates too
   if (coord.r - 1 >= 0 && m_boardHistory[coord.r - 1][coord.c] == '.') {
       m_boardHistory[coord.r - 1][coord.c] = '!';
       m_pointsOfDesignedAttack.push(Point(coord.r - 1, coord.c));
   }

    // If EAST is valid, add it to the stack
    if (coord.c + 1 <= game().cols() - 1 && m_boardHistory[coord.r][coord.c + 1] == '.') {
        m_boardHistory[coord.r][coord.c+1] = '!';
        m_pointsOfDesignedAttack.push(Point(coord.r, coord.c + 1));
    }

  // If SOUTH is valid, add it to the stack
   if (coord.r + 1 <= game().rows() - 1 && m_boardHistory[coord.r + 1][coord.c] == '.') {
       m_boardHistory[coord.r + 1][coord.c] = '!';
       m_pointsOfDesignedAttack.push(Point(coord.r + 1, coord.c));
   }

   // If WEST is valid, add it to the stack
   if (coord.c - 1 >= 0 && m_boardHistory[coord.r][coord.c - 1] == '.') {
       m_boardHistory[coord.r][coord.c - 1] = '!';
       m_pointsOfDesignedAttack.push(Point(coord.r, coord.c - 1));
   }
}

bool GoodPlayer::placeShips(Board& b) {
    bool valid;
    int shipId = 0;
    int shipNums = game().nShips(); // double check if this needs to be outside the loop

    while (shipNums > 0)
       {
           int i = randInt(m_pointsFree.size());
           Point coord(m_pointsFree[i].r, m_pointsFree[i].c);
           valid = b.placeShip(coord, shipId, HORIZONTAL);
           if (!valid)
               valid = b.placeShip(coord, shipId, VERTICAL);
           if (valid)
           {
               deletePoint(coord, m_pointsFree);
               shipNums--;
               shipId++;
           }
       }

    // What next ->? once placed then attack? -> check this later
    // TA: first clear the points of the vector so that it can be reinitiated -> DO THIS WEDNESDAY
    m_pointsFree.clear();
    for (int r = 0; r < game().rows(); r++) {
        for (int c = 0; c < game().cols(); c++) {
            m_pointsFree.push_back(Point(r,c));
        }
    }
    return true; // should be return true since it worked
   
}

Point GoodPlayer::recommendAttack() {
   
    
 // if state is 2, then calculate the point for the attack
  if (calcOrRandom == 2) {
       Point coord;
      if (!m_pointsOfDesignedAttack.empty()) {
          coord = m_pointsOfDesignedAttack.top();
      }
       m_pointsOfDesignedAttack.pop();
       deletePoint(coord, m_pointsFree); // delete the point where attacked
       return coord;
   }

//if state is normal, randomly select the point
   int i = randInt(m_pointsFree.size()); // implicit conversion -> ignore according to TA (doesn't do anything harmful anyways)
   Point coord(m_pointsFree[i].r, m_pointsFree[i].c);
   // Remove the selected point from points remaining
   deletePoint(coord, m_pointsFree);
   return coord;
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    
    // if the shot is valid and it does hit
    if (shotHit) {
        designAttackPoints(p);
        m_boardHistory[p.r][p.c] = 'X'; // update board with hit
    }

    if (!shotHit){
        m_boardHistory[p.r][p.c] = 'o';
    }

    if (calcOrRandom == 2) {// second state is calculated shot (NOT 1 <--)
        if (m_pointsOfDesignedAttack.empty()) {
            calcOrRandom = 1; // set the state back to normal if there isn't any attacks that have been calculated avaliable/any avalaible attacks
        }
    }
    else if (calcOrRandom == 1) {
        if (shotHit) {
            calcOrRandom = 2; // if the random shot hit something, then use the stack algorithm (the maze algo) to shoot and fire at possible points since a battleship is north/south/west/east etc.
        }
    }
    
}

void GoodPlayer::recordAttackByOpponent(Point p) {
    // shouldn't need to do anything
}

//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
