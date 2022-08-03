#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
//Added
#include <vector>
#include <map>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
      // TODO:  Decide what private members you need.  Here's one that's likely
      //        to be useful:
    const Game& m_game;

    //Added:
    char m_board[MAXROWS][MAXCOLS];
    map<int,int> m_shipsOnBoard;
/*   vector<int> aliveShips;
     vector<int> deadShips; */

};

BoardImpl::BoardImpl(const Game& g) : m_game(g), m_shipsOnBoard( {} )
{
//    m_shipsOnBoard = { };
    // call g.rows() not MAXROLS/MAXCOLS since a board can be constructed lower than 10x10

    for (int r = 0; r < g.rows(); r++) {
        for (int c = 0; c < g.cols(); c++) {
            m_board[r][c] = '.'; // initialize the board to have '.' for everything
        }
    }
}

void BoardImpl::clear()
{
    // This compiles, but may not be correct
    
    //VECTOR IMPLEMENTATION?
    // Kill off shipIDs evident in both vectors -> use STL's clear() function
//    aliveShips.clear();
//    deadShips.clear();

    // Reset the grid to water (which is '.')
    for (int r = 0; r < m_game.rows(); r++) { // cleared by default is "."
        for (int c = 0; m_game.cols(); c++) { // loop through to reset board with "."
            m_board[r][c] = '.';
        }
    }

}

void BoardImpl::block()
{
    // Block cells with 50% probability
    // Loop through 50% of spaces, pick a random point, validating if point is unblocked, then block
    int blocked = (m_game.rows() * m_game.cols()) / 2;
    int blockedcounter = 0;

    while (blockedcounter < blocked) {
        int randomRow = randInt(m_game.rows()); // choose random point within limit of grid
        int randomCol = randInt(m_game.cols());
        if (m_board[randomRow][randomCol] != 'X') { // if not blocked
            m_board[randomRow][randomCol] = 'X'; // then block
            blockedcounter++; // adjust counter to account for 50%
        }
    }
}

     // ===== SPEC CHANGED BELOW =====
//      // Block cells with 50% probability
//    for (int r = 0; r < m_game.rows(); r++) {
//        for (int c = 0; c < m_game.cols(); c++) {
//            if (randInt(2) == 0)
//            {
//                //added
//                m_board[r][c] = 'X'; // TODO:  Replace this with code to block cell (r,c)
//            }
//        }
//    }
//}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            // TODO:  Replace this with code to unblock cell (r,c) if blocked
            //added
            if (m_board[r][c] == 'X') {
                m_board[r][c] = '.';
            };
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    // TOP most segment if dir = VERTICAL
    // LEFT most segment if dir = HORIZONTAL

    // Conditions that will cause a return false
    // 1: ShipId is invalid
    if (shipId < 0 || shipId > m_game.nShips() - 1) { //shipID 0 indexed, nShips returns elements, so -1
        return false;
    }

    //2: The ship would be partly or outside the board
    if (topOrLeft.r < 0 || topOrLeft.c < 0 || topOrLeft.r > m_game.rows() - 1 || topOrLeft.c > m_game.cols() - 1) { // rows() and cols() not 0 indexed
        return false;
    }

    //3 & 4 skipped for now...

    //5: The ship with that ship ID has previously been placed on this Board and not yet been unplaced since its most recent placement.
    if (m_shipsOnBoard.find(shipId) != m_shipsOnBoard.end()) {
        return false;
    }

    //VECTOR IMPLEMENTATION?
 /*   for (vector<int>::iterator q = aliveShips.begin(); q != aliveShips.end(); q++) {
        if (*q == shipId) {
            cerr << "ship already exists! (board placeShip)";
            return false;
        }
    }*/
    
    // =====NOTE=====: topOrLeft.c is actually ROW and topOrLeft.r is actually COL
    // Horizontal placement: check for already-placed ship (3) & blocks (4)
    int length = m_game.shipLength(shipId);

    //HORIZONTAL DEALS WITH COLS
    if (dir == HORIZONTAL) {

        // Checked for initial placement of ship is not out of bounds previously
        // We need to now check if the ship length (rest of the ship) is within bounds
        if (topOrLeft.c + length > m_game.cols()) {
            return false;
        }
        // Now check if the whole ship length does not overlap another ship or blocked (condition 4 + 5)
        for (int i = 0; i < length; i++) {
            if (m_board[topOrLeft.r][topOrLeft.c + i] != '.') { // Move ALONG RIGHT to continue CHECKING
                return false;
            }
        }
        for (int i = 0; i < length; i++) {
//            else {
                // After checking for condition 4 + 5, place ship
                m_board[topOrLeft.r][topOrLeft.c + i] = shipId+'0'; // uses char symbol -> check
//                cerr << "aliveShips added horiz. successfully (Board placeships)" << endl;
//            }
        }
} // dir = horizontal ending bracket
    else {
    // VERTICAL placement: check for already-placed ship (3) & blocks (4)
    if (dir == VERTICAL) {

        // Checked for initial placement of ship is not out of bounds previously
        // We need to now check if the ship length (rest of the ship) is within bounds
        if (topOrLeft.r + length > m_game.rows()) {
            return false;
        }

        // Now check if the whole ship length does not overlap another ship or blocked (condition 4 + 5)
        for (int i = 0; i < length; i++) {
            if (m_board[topOrLeft.r + i][topOrLeft.c] != '.') { // Move DOWN to continue CHECKING
                return false;
            }
        }
//            else {
                // After checking for condition 4 + 5, place ship
            for (int i = 0; i < length; i++) {
                m_board[topOrLeft.r + i][topOrLeft.c] = shipId+'0'; // uses char symbol -> check
//                cerr << "aliveShips added vert. successfully (Board placeships)" << endl;
        //        aliveShips.push_back(i); // adds ship to vector that tracks alive ships
//            }
            }
        
        }
    }
    // Tracker
 /*   aliveShips.push_back(length); // each element in vector consists of the length of a ship */
    m_shipsOnBoard.insert(make_pair(shipId, m_game.shipLength(shipId)));
    return true;                  // e.g. shipId = 0 means element 1 has x length ship
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    // Conditions that will cause a return false
    // ShipId is invalid
    if (shipId < 0 || shipId > m_game.nShips() - 1) { //shipID 0 indexed, nShips returns elements, so -1
        return false;
    }

    // The ship would be partly or outside the board
    if (topOrLeft.r < 0 || topOrLeft.c < 0 || topOrLeft.r > m_game.rows() - 1 || topOrLeft.c > m_game.cols() - 1) { // rows() and cols() not 0 indexed
        return false;
    }

    // Only able to remove if the shipId is indeed already in the vector
  /*  for (vector<int>::iterator q = aliveShips.begin(); q != aliveShips.end(); q++) {
        if (*q != shipId) {
            cerr << "ship already exists! (board placeShip)";
            return false;
        }
    }*/
    if (m_shipsOnBoard.find(shipId) == m_shipsOnBoard.end()) {
        return false;
    }

    // Bound checking
        // Is the whole ship that needs to be removed longer than the actual cols and rows? Should be no if program works.
    if (dir == HORIZONTAL && topOrLeft.c + m_game.shipLength(shipId) > m_game.cols()) {
        return false;
    }
    if (dir == VERTICAL && topOrLeft.r + m_game.shipLength(shipId) > m_game.rows()) {
        return false;
    }

    // Actual unplacing of ships
    int length = m_game.shipLength(shipId);

    // Horizontal
    if (dir == HORIZONTAL) {
        for (int i = 0; i < length; i++) {
            if (m_board[topOrLeft.r][topOrLeft.c + i]-48 != shipId) { // ascii conversion for symbol
                return false;
            }
            else {
                // Remove ship by placing water again
                m_board[topOrLeft.r][topOrLeft.c + i] = '.';
            }
        }
    }

    if (dir == VERTICAL) {
        for (int i = 0; i < length; i++) {
            if (m_board[topOrLeft.r + 1][topOrLeft.c]-48 != shipId) {
                return false;
            }
            else {
                // Remove ship by placing water again
                m_board[topOrLeft.r + 1][topOrLeft.c] = '.';
            }
        }
    }


    //Tracker: can't simply pop the back
 /*   for (vector<int>::iterator q = aliveShips.begin(); q != aliveShips.end(); q++) {
        int iteratorIndex = 0;
        if (q[iteratorIndex] == shipId) {
            aliveShips.erase(aliveShips.begin() + iteratorIndex);
        }
        iteratorIndex++;
    }*/
    m_shipsOnBoard.erase(shipId);
//    cerr << "Successfully removed ship (Board, unplaceShip)" << endl;
    return true; // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const
{
//    cerr << "Display CALLED" << endl;
    // First line:
    cout << "  ";
    int i = 0;
    while (i < m_game.cols()) {
        cout << i;
        i++;
    }
    cout << endl;

    // Create rest of the board
    for (int r = 0; r < m_game.rows(); r++) {
        cout << r << " "; // create indentation
        for (int c = 0; c < m_game.cols(); c++) {
            if (m_board[r][c] == 'o' || m_board[r][c] == 'X' || m_board[r][c]== '.') {
//                cerr << "is display even called??" << endl;
                cout << m_board[r][c];
            }
            else {
                if (shotsOnly/* == true*/) {
                    cout << '.';
                }
                else {
                    cout << m_game.shipSymbol(m_board[r][c]-48);
                }
            }
//            cout << endl;
        }
        cout << endl;
    }


    // This compiles, but may not be correct
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    // If attack is invalid (point outside of grid)
    if (p.r > m_game.rows() || p.c > m_game.cols() || p.r < 0 || p.c < 0 ) {
        return false;
    }
    // Attack
    // Condition 1: if the player fires and misses (hitting the ocean)
    if (m_board[p.r][p.c] == '.') {
        m_board[p.r][p.c] = 'o';
        shipDestroyed = false;
        shotHit = false;
    }
    // Condition 2: if the ship is already shot
    else if (m_board[p.r][p.c] == 'X' || m_board[p.r][p.c] == 'o') { // DONT CHANGE NOTHING IF X OR O
        return false;
    }
    else {
       /* shipDestroyed = true;*/
        // Set shipID
        shipId = m_board[p.r][p.c]-48;
        shotHit = true;

        // Mark hit
        m_board[p.r][p.c] = 'X'; // mark as hit
        m_shipsOnBoard[shipId]--; // added 4 map

        //added 4 map
        if (m_shipsOnBoard[shipId] == 0) {
            shipDestroyed = true;
            m_shipsOnBoard.erase(shipId);
        }

        // Tracker
    /*    deadShips.push_back(shipId); // another vector: keeps track of ships that are destroyed */
        
    }
    return true; // This compiles, but may not be correct
}

bool BoardImpl::allShipsDestroyed() const
{
    // For vector implementation -> SCRAP if MAP is more useful?
    /*bool winOrLose = false;
    if (aliveShips.size() == deadShips.size()) {
        winOrLose = true;
    }
    return winOrLose;*/
    return m_shipsOnBoard.empty(); // if the map has nothing, it means there is no ship, thus a winner
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
