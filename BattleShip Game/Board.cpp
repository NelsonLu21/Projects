#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

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
    const Game& m_game;
    char m_board[10][10];
    int m_nRows, m_nCols;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    m_nCols = g.cols();
    m_nRows = g.rows();
    for (int i = 0; i < m_nRows; i++){
        for (int j = 0; j < m_nCols; j++){
            m_board[i][j]='.';
        }
    }
}

void BoardImpl::clear()
{
    for (int i = 0; i < m_nRows; i++){
        for (int j = 0; j < m_nCols; j++){
            m_board[i][j]='.';
        }
    }
}

void BoardImpl::block()
{
    int half = m_nCols * m_nRows / 2;
    for (int i = 0; i < half; i++){
        int r = randInt(m_nRows);
        int c = randInt(m_nCols);
        while (m_board[r][c] == 'X'){
            r = randInt(m_nRows);
            c = randInt(m_nCols);
        }
        m_board[r][c] = 'X';
    }
    
}

void BoardImpl::unblock()
{
    for (int i = 0; i < m_nRows; i++){
        for (int j = 0; j < m_nCols; j++){
            if (m_board[i][j]=='X'){
                m_board[i][j]='.';
            }
        }
    }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId < 0 || shipId > m_game.nShips()) return false;
    if (dir == HORIZONTAL && topOrLeft.c + m_game.shipLength(shipId) > m_game.cols()) return false;
    else if (dir == VERTICAL && topOrLeft.r + m_game.shipLength(shipId) > m_game.rows()) return false;
    for (int i = 0; i < m_game.shipLength(shipId); i++){
        if (dir == HORIZONTAL){
            
            if (m_board[topOrLeft.r][topOrLeft.c+i] != '.') return false;
        }
        else{
            
            if (m_board[topOrLeft.r+i][topOrLeft.c] != '.') return false;
        }
    }
    
    for (int i = 0; i < m_game.shipLength(shipId); i++){
        if (dir == HORIZONTAL){
            m_board[topOrLeft.r][topOrLeft.c+i] = m_game.shipSymbol(shipId);
        }
        else{
            m_board[topOrLeft.r+i][topOrLeft.c] = m_game.shipSymbol(shipId);
        }
    }
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId < 0 || shipId >= m_game.nShips()) return false;
    for (int i = 0; i < m_game.shipLength(shipId); i++){
        if (dir == HORIZONTAL){
            if (m_board[topOrLeft.r][topOrLeft.c+i] != '.') return false;
        }
        else{
            if (m_board[topOrLeft.r+i][topOrLeft.c] != '.') return false;
        }
    }
    for (int i = 0; i < m_game.shipLength(shipId); i++){
        if (dir == HORIZONTAL){
            if (m_board[topOrLeft.r][topOrLeft.c+i] != m_game.shipSymbol(shipId)) return false;
        }
        else{
            if (m_board[topOrLeft.r+i][topOrLeft.c] != m_game.shipSymbol(shipId)) return false;
        }
    }
    for (int i = 0; i < m_game.shipLength(shipId); i++){
        if (dir == HORIZONTAL){
            m_board[topOrLeft.r][topOrLeft.c+i] = '.';
        }
        else{
            m_board[topOrLeft.r+i][topOrLeft.c] = '.';
        }
    }
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    cout << "  ";
    for (int i = 0; i < m_nCols; i++){
        cout << i;
    }
    cout << endl;
    
    if (shotsOnly)
    {
        for (int i = 0; i < m_nRows; i++)
        {
            cout << i << " ";
            for (int j = 0; j < m_nCols; j++)
            {
                if (m_board[i][j] == 'o' || m_board[i][j] == 'X')
                {
                    cout << m_board[i][j] ;
                }
                else
                {
                    cout << '.' ;
                }
            }
            cout << endl;
        }
    }
    
    else
    {
        for (int i = 0; i < m_nRows; i++){
            cout << i << " ";
            for (int j = 0; j < m_nCols; j++){
                cout << m_board[i][j] ;
            }
            cout << endl;
        }
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    shotHit = false;
    shipDestroyed = false;
    shipId = -1;
    if (p.r < 0 || p.c < 0 || p.r >= m_nRows || p.c >= m_nCols || m_board[p.r][p.c] == 'X' || m_board[p.r][p.c] == 'o'){
        return false;
    }
    if (m_board[p.r][p.c] == '.'){
        m_board[p.r][p.c] = 'o';
        shotHit = false;
    }
    else{
        shotHit = true;
        bool isLast = true;
        for (int i = 0; i<m_game.nShips();i++){
            if (m_game.shipSymbol(i) == m_board[p.r][p.c]){
                shipId = i;
            }
        }
        //if the symbol at p is the last symbol, the ship is considered to be destroyed
        for (int i = 0; i < m_nRows; i++){
            for (int j = 0; j < m_nCols; j++){
                //if the symbol at p is found anywhere else except for p, the segment is not the ship's last segment
                if ((i!=p.r||j!=p.c) && (m_board[i][j] == m_board[p.r][p.c])) isLast = false;
            }
        }
        
        if (isLast){
            shipDestroyed = true;
        }
        else{
            shipDestroyed = false;
        }
        
        m_board[p.r][p.c]='X';
    }
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    for (int i = 0; i < m_nRows; i++){
        for (int j = 0; j < m_nCols; j++){
            //if any symbol other than these detected, assume not all ships destroyed
            if (m_board[i][j] != 'X' && m_board[i][j] != 'o' && m_board[i][j] != '.'){
                return false;
            }
        }
    }
    return true;
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
