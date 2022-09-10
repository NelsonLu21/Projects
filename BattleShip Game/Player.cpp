#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <stack>

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

class HumanPlayer : public Player{
public:
    HumanPlayer(string nm, const Game& g);
    virtual ~HumanPlayer();
    virtual bool isHuman() const;
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);

};

HumanPlayer::HumanPlayer(string nm, const Game& g) : Player(nm,g){}

HumanPlayer::~HumanPlayer(){}

bool HumanPlayer::isHuman() const {return true;}
bool HumanPlayer::placeShips(Board& b){
    for (int i = 0; i < game().nShips(); i++){
        cout << name()<<" must place " << game().nShips()<<" ships."<<endl;
        b.display(false);
        cout << "Enter h or v for direction of "<< game().shipName(i)<<" (length " << game().shipLength(i)<< "):";
        char dir;
        while (!((cin >> dir)&& (dir == 'h'||dir == 'v'))){
            cout << "Direction must be h or v.";
        }

        int r, c;
        bool placed = false;
        do{
            cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
            getLineWithTwoIntegers(r, c);
            if (dir == 'h') placed = b.placeShip(Point(r,c), i, HORIZONTAL);
            else placed = b.placeShip(Point(r,c), i, VERTICAL);
            if (placed == false){
                cout << "The ship can not be placed there."<<endl;
            }
        } while (placed == false);
        
    }
    return true;
}
Point HumanPlayer::recommendAttack() {
    int r,c;
    cout << "Enter the row and column to attack (e.g., 3 5): ";
    getLineWithTwoIntegers(r, c);
    return Point(r,c);
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    
}

void HumanPlayer::recordAttackByOpponent(Point p){}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player{
public:
    MediocrePlayer(string nm, const Game& g);
    virtual ~MediocrePlayer();
    virtual bool isHuman() const { return false; }
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    bool placeAShip(Board& b, int shipId);
    
private:
    Point prePt;
    bool preHit;
    bool preDes;
    bool preValid;
    int preID;
    vector<Point> preAtt;
    Point shotMadeStateOne;
    int attState;
    
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g) : Player(nm,g), prePt(), preHit(false), preDes(false), preID(-1), attState(1){}
MediocrePlayer ::~MediocrePlayer(){}

bool MediocrePlayer::placeAShip(Board& b, int shipId){
    //base case: if the shipId passed as param is greater than largest shipId, all ships are placed.
    if (shipId > game().nShips()-1) return true;
    
    //otherwise, try to place ship on any position in any coord on the board
    
    for (int i = 0; i < game().rows(); i++){
        for (int j = 0; j < game().cols(); j++){
            //if a ship could be placed, check if the rest of the ships could also be placed
            if (b.placeShip(Point(i,j), shipId, HORIZONTAL)) {
                if (placeAShip(b, shipId+1)) return true;
                //if with current placement, the rest of the ships could not be placed, unplace this ship and try a different position using for-loop
                else b.unplaceShip(Point(i,j), shipId, HORIZONTAL);
            }
            
            //trying vertical placement at (i,j) if horizontally could not be placed
            if (b.placeShip(Point(i,j), shipId, VERTICAL)) {
                if (placeAShip(b, shipId+1)) return true;
                else b.unplaceShip(Point(i,j), shipId, VERTICAL);
            }
        }
    }
    return false;
}

bool MediocrePlayer::placeShips(Board& b){
    b.clear();
    b.block();
    if (placeAShip(b,0)){
        //if returned true, all ships are placed successfully
        b.unblock();
        return true;
    }
    else  cerr<<"can't place ships"<<endl;
    return false;
}

Point MediocrePlayer::recommendAttack(){
    if (attState==1&&preHit==true) attState=2;
    //in any circumstance, if a ship is destroyed, go to state 1
     if (preDes == true) attState = 1;
    //return random point in state 1
    if (attState  == 1){
        int r,c;
        bool alrAtt = false;
        do {
            alrAtt = false;
            r = randInt(game().rows());
            c = randInt(game().cols());
            if (!preAtt.empty()){
                for (int i = 0; i < preAtt.size(); i++){
                    if (preAtt.at(i).r == r && preAtt.at(i).c == c){
                        alrAtt = true;
                    }
                }
            }
        } while (alrAtt);
        return Point(r,c);
    }
    
    //return random point in a set of points close to a last made shot
    else{
        int len = 5;
        int sign;
        int r,c;
        bool alrAtt = false;
        do {
            alrAtt = false;
            if (randInt(2) == 0) sign = -1;  else sign = 1;
            if (randInt(2) == 0){
                r = shotMadeStateOne.r+sign*randInt(len);
                if (r<0) r = 0;
                if (r>=game().rows()) r=game().rows()-1;
                c = shotMadeStateOne.c;
            }
            else {
                r = shotMadeStateOne.r;
                c = shotMadeStateOne.c+sign*randInt(len);
                if (c<0) c = 0;
                if (c>=game().rows()) c=game().rows()-1;
            }
            if (!preAtt.empty()){
                for (int i = 0; i < preAtt.size(); i++){
                    if (preAtt.at(i).r == r && preAtt.at(i).c == c){
                        alrAtt = true;
                    }
                }
            }
        } while (alrAtt);
        
        return Point(r,c);
    }
    
}

//stores attack result into mediocre player's member variables
void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,bool shipDestroyed, int shipId){
    prePt = p;
    preHit = shotHit;
    preDes =  shipDestroyed;
    preID = shipId;
    preValid = validShot;
    preAtt.push_back(p);
    if(attState==1&&shotHit==true){
        shotMadeStateOne=p;
        attState=2;
    }
}

void MediocrePlayer::recordAttackByOpponent(Point p){}


//*********************************************************************
//  GoodPlayer
//*********************************************************************


class GoodPlayer : public Player{
public:
    GoodPlayer(string nm, const Game& g);
    virtual ~GoodPlayer();
    virtual bool isHuman() const;
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    bool placeAShip(Board& b, int shipId);
    bool checkValidPt(Point p);
    Point makeAGuess();
    
private:
    Point prePt;
    bool preHit;
    bool preDes;
    bool preValid;
    int preID;
    int attState;
    bool shipLiesHor;
    bool shipLiesVer;
    Point guessShotMade;
    vector<Point> preAtt;
    stack<Point> state2targets;
    stack<Point> state3targets;
    stack<Point> state4targets;
    vector<Point> attackedInState3;
    stack<Point> state5targets;
    bool state3top;
    bool state3bot;
};

GoodPlayer::GoodPlayer(string nm, const Game& g) : Player(nm,g) {
    preHit = false;
    preDes = false;
    preValid = false;
    preID = -1;
    attState = 1;
    shipLiesHor=false;
    shipLiesVer=false;
    state3top = true;
    state3bot = true;
    
}

GoodPlayer::~GoodPlayer(){}

bool GoodPlayer::isHuman() const {return false;}

bool GoodPlayer::placeAShip(Board& b, int shipId){
    //base case: if the shipId passed as param is greater than largest shipId, all ships are placed.
    if (shipId > game().nShips()-1) return true;
    
    //otherwise, try to place ship on any position in any coord on the board
    
    for (int i = 0; i < game().rows(); i++){
        for (int j = 0; j < game().cols(); j++){
            //if a ship could be placed, check if the rest of the ships could also be placed
            if (b.placeShip(Point(i,j), shipId, HORIZONTAL)) {
                if (placeAShip(b, shipId+1)) return true;
                //if with current placement, the rest of the ships could not be placed, unplace this ship and try a different position using for-loop
                else b.unplaceShip(Point(i,j), shipId, HORIZONTAL);
            }
            
            //trying vertical placement at (i,j) if horizontally could not be placed
            if (b.placeShip(Point(i,j), shipId, VERTICAL)) {
                if (placeAShip(b, shipId+1)) return true;
                else b.unplaceShip(Point(i,j), shipId, VERTICAL);
            }
        }
    }
    return false;
}

bool GoodPlayer::placeShips(Board& b){
    b.clear();
    b.block();
    if (placeAShip(b,0)){
        b.unblock();
        return true;
    }

    return false;
}

bool GoodPlayer::checkValidPt(Point p){
    if (p.r>=game().rows()||p.c>=game().rows()||p.r<0||p.c<0){
        return false;
    }
    
    if (!preAtt.empty()){
        for (int i = 0; i < preAtt.size(); i++){
            if (p.r == preAtt.at(i).r && p.c == preAtt.at(i).c){
                return false;
            }
        }
    }
    
    return true;
}

Point GoodPlayer::makeAGuess(){
    int minShipLength = -1;
    for (int i = 0; i < game().nShips(); i++){
        if (minShipLength < game().shipLength(i)) minShipLength = game().shipLength(i);
    }
    

    if (minShipLength > 1){
        int r,c;
        int counter = 0;
        do{
            counter ++;
            if (randInt(2)==0){
                //if the row is odd, col must be even
                r = 2*randInt(game().rows()/2)+1;
                c = 2*randInt(game().cols()/2);
            }
            else {
                //if the row is even, col must be odd
                r = 2*randInt(game().rows()/2);
                c = 2*randInt(game().cols()/2)+1;
            }
        } while (!checkValidPt(Point(r,c))&& counter < 100000);

        if (counter > 100000){
            int a,b;
            do{
                a = randInt(game().rows());
                b = randInt(game().cols());
            } while (!checkValidPt(Point(a,b)));
            return Point(a,b);
        }
        return Point(r,c);
    }
    
    else {
        int r,c;
        do{
            r = randInt(game().rows());
            c = randInt(game().cols());
        } while (!checkValidPt(Point(r,c)));
        return Point(r,c);
    }
}

Point GoodPlayer::recommendAttack(){
    if (attState == 1){
        return makeAGuess();
    }
    
    else if (attState == 2){
        //if a guess shot hit and ship not destroyed, recommend every horiz/vert adjacent point
        while (!state2targets.empty()){
            if (checkValidPt(state2targets.top())){
                Point p = state2targets.top();
                state2targets.pop();
                return p;
            }
            else state2targets.pop();
        }
        
        //return makeAGuess();
    }
    
    else if (attState == 3){
        if  (!state3targets.empty()) {
            Point p;
            bool success = false;
            do{
                p = state3targets.top();
                state3targets.pop();
                success = checkValidPt(p);
            } while (!state3targets.empty()&&!success);
            if (!success) return makeAGuess();
            return p;
        }
    }
    
    else if (attState == 4){
        if (!state4targets.empty()){
            Point p = makeAGuess();
            bool success = false;
            do {
                p = state4targets.top();
                state4targets.pop();
                success = checkValidPt(p);
            } while (!state4targets.empty() && !success);
            if (!success) return makeAGuess();
            return p;
        }
        else {
            attState = 1;
            return makeAGuess();
        }
    }
    
    else if (attState == 5){
        if (!state5targets.empty()){
            Point p = makeAGuess();
            bool success = false;
            do {
                p = state5targets.top();
                state5targets.pop();
                success = checkValidPt(p);
            } while (!state5targets.empty() && !success);
            if (!success) return makeAGuess();
            return p;
        }
        else attState = 1;
        return makeAGuess();
    }
    return makeAGuess();
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    //if a ship is destroyed, return to state 1
    if (shipDestroyed){
        //if ship destroyed after attacking along line, but more attacks are made than the length of the ship destroyed
        if (attState == 3){
            attackedInState3.push_back(p);
            if (attackedInState3.size()>game().shipLength(shipId)){
                attState = 5;
                if (shipLiesVer){
                    int rMin = 10;
                    int rMax = -1;
                    for (int i = 0; i < attackedInState3.size(); i++){
                        if (rMin>attackedInState3.at(i).r){
                            rMin = attackedInState3.at(i).r;
                        }
                        
                        if (rMax < attackedInState3.at(i).r){
                            rMax = attackedInState3.at(i).r;
                        }
                    }
                    state5targets.push(Point(rMin,attackedInState3.front().c+1));
                    state5targets.push(Point(rMin,attackedInState3.front().c-1));
                    state5targets.push(Point(rMin-1,attackedInState3.front().c));
                    
                    state5targets.push(Point(rMax,attackedInState3.front().c+1));
                    state5targets.push(Point(rMax,attackedInState3.front().c-1));
                    state5targets.push(Point(rMax+1,attackedInState3.front().c));
                }
                
                if (shipLiesHor){
                    int cMin = 10;
                    int cMax = -1;
                    for (int i = 0; i < attackedInState3.size(); i++){
                        if (cMin>attackedInState3.at(i).c){
                            cMin = attackedInState3.at(i).c;
                        }
                        
                        if (cMax < attackedInState3.at(i).c){
                            cMax = attackedInState3.at(i).c;
                        }
                    }
                    state5targets.push(Point(attackedInState3.front().r+1,cMin));
                    state5targets.push(Point(attackedInState3.front().r-1,cMin));
                    state5targets.push(Point(attackedInState3.front().r,cMin-1));
                    
                    state5targets.push(Point(attackedInState3.front().r+1,cMax));
                    state5targets.push(Point(attackedInState3.front().r-1,cMax));
                    state5targets.push(Point(attackedInState3.front().r,cMax+1));
                }
                
               
                
                state5targets.push(Point(attackedInState3.back().r+1,attackedInState3.back().c));
                state5targets.push(Point(attackedInState3.back().r-1,attackedInState3.back().c));
                state5targets.push(Point(attackedInState3.back().r,attackedInState3.back().c+1));
                state5targets.push(Point(attackedInState3.back().r,attackedInState3.back().c-1));
            }
            else attState = 1;
        }
        else if (attState == 4 || attState == 5){}
        else attState = 1;

    }
    
    if (!shipDestroyed && shotHit && attState==1){
        //if a guess shot hit target, enter attack state 2
        while(!state2targets.empty()){
            state2targets.pop();
        }
        state2targets.push(Point(p.r,p.c+1));
        state2targets.push(Point(p.r,p.c-1));
        state2targets.push(Point(p.r+1,p.c));
        state2targets.push(Point(p.r-1,p.c));
        guessShotMade = p;
        attState = 2;
    }
    
    else if (shotHit && attState==2){
        //if after initial shot hit, another shot is hit
        //determine whether second shot lies horizontal or vertical
        //assume that that is the orientation of ship
        while (!attackedInState3.empty()){
            attackedInState3.pop_back();
        }
        shipLiesHor = false;
        shipLiesVer = false;
        attackedInState3.push_back(guessShotMade);
        attackedInState3.push_back(p);
        if (p.r == guessShotMade.r) shipLiesHor = true;
        else if (p.c == guessShotMade.c) shipLiesVer = true;
        attState = 3;
        state3top=true;
        state3bot=true;

        if (shipLiesHor) {
            if (p.c>guessShotMade.c){
                if (checkValidPt(Point(p.r,p.c+1)))
                    state3targets.push(Point(p.r,p.c+1));
                else state3bot = false;
                
                if (checkValidPt(Point(p.r,guessShotMade.c-1)))
                    state3targets.push(Point(p.r,guessShotMade.c-1));
                else state3top = false;
            }
            else {
                if (checkValidPt(Point(p.r,p.c-1)))
                    state3targets.push(Point(p.r,p.c-1));
                else state3top = false;
                
                if (checkValidPt(Point(p.r,guessShotMade.c+1)))
                    state3targets.push(Point(p.r,guessShotMade.c+1));
                else state3bot = false;
            }
        }
        else if (shipLiesVer) {
            if (p.r>guessShotMade.r){
                if (checkValidPt(Point(p.r+1,p.c)))
                state3targets.push(Point(p.r+1,p.c));
                else state3bot = false;
                if (checkValidPt(Point(guessShotMade.r-1,p.c)))
                state3targets.push(Point(guessShotMade.r-1,p.c));
                else state3top = false;
            }
            else {
                if (checkValidPt(Point(p.r-1,p.c)))
                    state3targets.push(Point(p.r-1,p.c));
                else state3top = false;
                
                if (checkValidPt(Point(guessShotMade.r+1,p.c)))
                state3targets.push(Point(guessShotMade.r+1,p.c));
                else state3bot = false;
            }
        }
    }
    
    else if (attState == 3 && shotHit && !shipDestroyed){
        attackedInState3.push_back(p);
        if (shipLiesHor && p.c>guessShotMade.c){
            if (checkValidPt(Point (p.r,p.c+1)))
            {
                state3targets.push(Point (p.r,p.c+1));
                state3bot = true;
            }
            else state3bot=false;
        }
        if (shipLiesHor && p.c<guessShotMade.c){
            if (checkValidPt(Point (p.r,p.c-1)))
            {
                state3targets.push(Point (p.r,p.c-1));
                state3top = true;
            }
            else state3top=false;
        }
        if (shipLiesVer && p.r>guessShotMade.r){
            if (checkValidPt(Point (p.r+1,p.c)))
            {
                state3targets.push(Point (p.r+1,p.c));
                state3bot = true;
            }
            else state3bot=false;
        }
        if (shipLiesVer && p.r<guessShotMade.r){
            if (checkValidPt(Point (p.r-1,p.c)))
            {
                state3targets.push(Point (p.r-1,p.c));
                state3top = true;
            }
            else state3top = false;
        }
        if (!state3top && !state3bot){
            attState = 4;
            for (int i =  0; i< attackedInState3.size(); i++){
                if (shipLiesVer){
                    state4targets.push(Point(attackedInState3.at(i).r,attackedInState3.at(i).c+1));
                    state4targets.push(Point(attackedInState3.at(i).r,attackedInState3.at(i).c-1));
                }
                else{
                    state4targets.push(Point(attackedInState3.at(i).r+1,attackedInState3.at(i).c));
                    state4targets.push(Point(attackedInState3.at(i).r-1,attackedInState3.at(i).c));
                }
            }
        }
    }
    
    else if (attState == 3 && !shotHit){
        if (shipLiesHor && p.c>guessShotMade.c){
            state3bot = false;
        }
        else if (shipLiesHor && p.c<guessShotMade.c){
            state3top = false;
        }
        if (shipLiesVer && p.r>guessShotMade.r){
            state3bot = false;
        }
        else if (shipLiesVer && p.r<guessShotMade.r){
            state3top = false;
        }
        
        //if every point in the direction has been attacked (bounded by two empty shots) and ship not destroyed, enter state 4, in which we search for ships adjacent to points
        if (!state3top && !state3bot){
            attState = 4;
            for (int i =  0; i< attackedInState3.size(); i++){
                if (shipLiesHor){
                    state4targets.push(Point(attackedInState3.at(i).r+1,attackedInState3.at(i).c));
                    state4targets.push(Point(attackedInState3.at(i).r-1,attackedInState3.at(i).c));
                }
                else{
                    state4targets.push(Point(attackedInState3.at(i).r,attackedInState3.at(i).c+1));
                    state4targets.push(Point(attackedInState3.at(i).r,attackedInState3.at(i).c-1));
                }
            }
        }
    }
    
    else if (attState == 4 && shotHit){
        state4targets.push(Point(p.r,p.c+1));
        state4targets.push(Point(p.r,p.c-1));
        state4targets.push(Point(p.r+1,p.c));
        state4targets.push(Point(p.r-1,p.c));
    }
    
    else if (attState == 5 && shotHit){
        state5targets.push(Point(p.r,p.c+1));
        state5targets.push(Point(p.r,p.c-1));
        state5targets.push(Point(p.r+1,p.c));
        state5targets.push(Point(p.r-1,p.c));
    }
    
    prePt = p;
    preHit = shotHit;
    preDes =  shipDestroyed;
    preID = shipId;
    preValid = validShot;
    preAtt.push_back(p);
}

void GoodPlayer::recordAttackByOpponent(Point p){}
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
