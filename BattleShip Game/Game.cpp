#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class Ship{
public:
    Ship(int length, char symbol, string name, int ID);
    int getLen();
    char getSym();
    string getName();
    
private:
    int m_length;
    char m_symbol;
    string m_name;
    int m_ID;
};

Ship::Ship(int length, char symbol, string name, int ID) : m_length(length), m_symbol(symbol), m_name(name), m_ID(ID){}

int Ship::getLen(){
    return m_length;
}

char Ship::getSym(){
    return m_symbol;
}

string Ship::getName(){
    return m_name;
}

class GameImpl
{
public:
    GameImpl(int nRows, int nCols);
    ~GameImpl();
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
private:
    int m_nRows, m_nCols, m_nShips;
    Ship* m_ships[100];         //nShips() will always be less than 100
};



void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols) : m_nRows(nRows), m_nCols(nCols), m_nShips(0){}

GameImpl::~GameImpl(){
    for (int i = 0; i < m_nShips; i++){
        delete m_ships[i];
    }
}

int GameImpl::rows() const
{
    return m_nRows;
}

int GameImpl::cols() const
{
    return m_nCols;
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
    m_ships[m_nShips] = new Ship(length, symbol, name, m_nShips);
    m_nShips++;
    return true;
}

int GameImpl::nShips() const
{
    return m_nShips;
}

int GameImpl::shipLength(int shipId) const
{
    return m_ships[shipId] -> getLen();
}

char GameImpl::shipSymbol(int shipId) const
{
    return m_ships[shipId] -> getSym();
}

string GameImpl::shipName(int shipId) const
{
    return m_ships[shipId] -> getName();
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)  
{
    bool p1Placed = false;
    bool p2Placed = false;
    
    for(int i =  0; i<50; i++){
        if (p1 -> placeShips(b1)) {
            p1Placed = true;
            break;
        }
    }
    
    for (int i =  0; i<50; i++){
        if (p2 -> placeShips(b2)) {
            p2Placed=true;
            break;
        }
    }
    
    if  (p1Placed==false || p2Placed == false) return nullptr;
    
    while (true){
        bool isHit = false;
        bool isDes = false;
        int hitID;
        cout << p1->name()<<"'s turn. Board for "<<p2->name()<<endl;
        b2.display(p1 -> isHuman());        
        Point rec = p1->recommendAttack();
        if (b2.attack(rec, isHit, isDes, hitID)){p1->recordAttackResult(rec, true, isHit, isDes, hitID);}      
        else {
            cout << p1->name()<<" wasted a shot at ("<<rec.r<<','<<rec.c<<")."<<endl;
            p1->recordAttackResult(rec, false, false, false, -1);
        }
        if (isHit || isDes){
            cout << p1->name() << " attacked (" << rec.r << "," << rec.c <<") and hit something, resulting in: " << endl;
            b2.display(p1->isHuman());
            if(shouldPause){
                waitForEnter();
            }
        }
        else{
            cout<<p1->name() << " attacked (" << rec.r << "," << rec.c <<") and missed, resulting in: " << endl;
            b2.display(p1->isHuman());
            if(shouldPause){
                waitForEnter();
            }
        }
        if (b2.allShipsDestroyed()){
            cout << p1->name() << " wins!"<<endl;
            if (p2->isHuman()){
                cout << "Here is " << p1->name() << "'s board: " << endl;
                b1.display(false);
            }
            return p1;
        }
        
        isHit = false; isDes = false; hitID = -1;
        
        cout << p2->name()<<"'s turn. Board for "<<p1->name()<<endl;

        b1.display(p2->isHuman());
        rec = p2->recommendAttack();
        if(b1.attack(rec, isHit, isDes, hitID)){p2->recordAttackResult(rec, true, isHit, isDes, hitID);}
        else{
            cout << p2->name()<<" wasted a shot at ("<<rec.r<<','<<rec.c<<")."<<endl;
            p2->recordAttackResult(rec, false, false, false, -1);
        }
        
        if (isHit || isDes){
            cout << p2->name() << " attacked (" << rec.r << "," << rec.c <<") and hit something, resulting in: " << endl;
            b1.display(p2 -> isHuman());
            if(shouldPause){
                waitForEnter();
            }
        }
        
        else{
            cout<<p2->name() << " attacked (" << rec.r << "," << rec.c <<") and missed, resulting in: " << endl;
            b1.display(p2 -> isHuman());
            if(shouldPause){
                waitForEnter();
            }
        }
        
        if (b1.allShipsDestroyed()){
            cout << p2->name() << " wins!"<<endl;
            if (p1 -> isHuman()){
                cout << "Here is " << p2->name() << "'s board: " << endl;
                b2.display(false);
            }
            return p2;
        }
    }
    
    return nullptr;
}








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

