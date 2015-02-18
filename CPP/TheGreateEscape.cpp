/**************************************************************************/
/* Author: Luming Zhang                                                   */
/* Date 02/15/2015                                                        */
/**************************************************************************/ 

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>
#include <unordered_set>


#define DEBUG 1
#define LEADING_OFS 10
#define SURPRISE_THRESHOLD 5 // 5
#define SELF_DEFENDING_THRESHOLD 5 // 5
#define CANCELING_THRESHOLD 5 // 5
#define SELF_GUIDING_THRESHOLD 1 // Set to 1 to explore more oppotunity

#define GENERATE_WALL_OFS 7 // 7
#define BFS//ASTAR//
#define TIMING do{double elapsed_secs = double(clock() - beginTiming) / CLOCKS_PER_SEC; cerr<<"Elapsed time: "<<elapsed_secs*1000<<"ms in "<<__FUNCTION__<<" line: "<<__LINE__<<endl;} while(0);

#define VERSION 56/1006

using namespace std;

clock_t beginTiming; 
/*********************************** CLASS ************************************/
class Position
{
public:
    int x;
    int y;
    int priority;
    Position() : x(0), y(0), priority(-1) {}
    Position(int x, int y): x(x), y(y), priority(-1) {}
    friend ostream& operator<<(ostream& os, const Position& p)
    {
        os<<"("<<p.x<<","<<p.y<<")";
        return os;
    }
    bool operator==(const Position& other) const
    {
        if(this->x==other.x&&this->y==other.y)
            return true;
        else
            return false;
    }
    
};

class PositionHasher
{
public:
  size_t operator()(const Position& p) const
  {
    return hash<int>()(p.x)^hash<int>()(p.y);
  }
};

class State
{
public:
    Position pos;
    int gValue;
    int hValue;
    State* prev;
    State(const Position& s, int h, State* prev = NULL, int gValue=0) : pos(s), hValue(h), prev(prev), gValue(gValue) {}
};

class Player
{
public:
    int id;
    int wallsLeft;
    Position curr;
    Position dest;
    bool alive;
    int ETA;
    bool isMe;
    Player(const int id, const int wallsLeft, const Position& curr, const Position& dest, bool alive=false) 
    : id(id), wallsLeft(wallsLeft), curr(curr), dest(dest), alive(alive), ETA(-1), isMe(false) {}
};

class Wall
{
public: 
    Position pos;
    char ori;
    int priority;
    friend ostream& operator<<(ostream& os, const Wall& w)
    {
        os<<"("<<w.pos.x<<","<<w.pos.y<<","<<w.ori<<","<<w.priority<<")";
        return os;
    }
protected:
    Wall() : pos(Position(-1,-1)), ori('?'), priority (-1) {} // invalid wall
    Wall(const Position& s, const char o) : pos(s), ori(o), priority (-1){}
};

class BigWall : public Wall
{
public:
    BigWall() : Wall() {} // invalid wall
    BigWall(const Position& s, const char o) : Wall(s,o) {}
    bool operator==(const BigWall& other) const
    {
        if(this->pos.x==other.pos.x&&this->pos.y==other.pos.y&&this->ori==other.ori)
            return true;
        else
            return false;
    }
    bool operator!=(const BigWall& other) const
    {
        if(this->pos.x!=other.pos.x||this->pos.y!=other.pos.y||this->ori!=other.ori)
            return true;
        else
            return false;
    }
};

class SmallWall : public Wall
{
public:
    SmallWall() : Wall() {} // invalid wall
    SmallWall(const Position& s, const char o) : Wall(s,o) {}
};

/********************************** UTILITIES ******************************/
bool checkDest(const Position& curr, const Position& dest, const int id)
{
    if(id<2)
    {
        if(curr.x==dest.x)
            return true;
        else
            return false;
    }
    else
    {    
        if(curr.y==dest.y)
            return true;
        else
            return false;
    }
}

int wallAhead(const Position& pos, const vector<SmallWall>& walls, const int direction)
{
    int res=10;
    int tmp=-1;
    for(auto w : walls)
    {
        switch(direction)
        {
            case 0: if(w.pos.x>pos.x&&w.ori=='V'&&w.pos.y==pos.y) // right
                    {
                        tmp = w.pos.x-pos.x;
                        if(tmp<res)
                            res=tmp;
                    }
                    break;
            case 1: if(w.pos.y>pos.y&&w.ori=='H'&&w.pos.x==pos.x) // down
                    {
                        tmp = w.pos.y-pos.y;
                        if(tmp<res)
                            res=tmp;
                    }
                    break;
            case 2: if(w.pos.x<=pos.x&&w.ori=='V'&&w.pos.y==pos.y) // left
                    {
                        tmp = pos.x-w.pos.x+1;
                        if(tmp<res)
                            res=tmp;
                    }
                    break;
            case 3: if(w.pos.y<=pos.y&&w.ori=='H'&&w.pos.x==pos.x) // up
                    {
                        tmp = pos.y-w.pos.y+1;
                        if(tmp<res)
                            res=tmp;
                    }
                    break;
            default:cerr<<"Something wrong with direction!"<<endl;
                    break;
        }
    }
    
    return res;
}

vector<Position> getSuccessors(const Position& curr, const vector<SmallWall>& walls, bool heuristic = false)
{
    int dir[4][2] = {{1,0},{0,1},{-1,0},{0,-1}}; // right, down, left, up
    vector<Position> succ;
    auto lambda = [](const Position p1, const Position p2){return p1.priority<p2.priority;};
    priority_queue<Position,vector<Position>,decltype(lambda)> pq(lambda);
    
    for(int i=0;i<4;i++)
    {
        Position next;
        bool isSucc = true;
        next.x = curr.x+dir[i][0];
        next.y = curr.y+dir[i][1];
        if(next.x<0||next.x>8||next.y<0||next.y>8) // check out of bound
            continue;
        for(auto it : walls)
        {
            if((next.x==it.pos.x&&next.y==it.pos.y&&next.x>curr.x&&it.ori=='V')||// check go to right
                (next.x==it.pos.x&&next.y==it.pos.y&&next.y>curr.y&&it.ori=='H')||// check go to up
                (curr.x==it.pos.x&&curr.y==it.pos.y&&next.x<curr.x&&it.ori=='V')||// check go to left
                (curr.x==it.pos.x&&curr.y==it.pos.y&&next.y<curr.y&&it.ori=='H')) // check go to up
            {
                isSucc = false;
                break;
            }
            
        }
        if(isSucc)
        {
            if(heuristic)
            {
                next.priority = wallAhead(next, walls, i);
                pq.push(next);
            }
            else
                succ.push_back(next);
        }
    }
    if(heuristic)
    {
        while(pq.empty()==false)
        {
            succ.push_back(pq.top());
            pq.pop();
        }
    }
    return succ;
}

vector<Position> pathFinding(const Player& player, const std::vector<SmallWall>& walls, bool heuristic = false)
{
    vector<Position> res;
    vector<vector<bool> > closed(9,vector<bool>(9,false));
    vector<State*> garbage;
    
    /********************************* BFS ***********************************/
    State* opt=NULL;
    queue<State*> q;
    q.push(new State(player.curr,0));
    while(q.empty()==false)
    {
        State* tmp = q.front();
        garbage.push_back(tmp);
        q.pop();
        if(checkDest(tmp->pos, player.dest, player.id))
        {  
            while(tmp->prev)
            {
                res.push_back(tmp->pos);
                tmp=tmp->prev;
            }
            break;
    
        }
        vector<Position> succ = getSuccessors(tmp->pos,walls,heuristic);
        for(auto it : succ)
        {
            if(closed[it.x][it.y]==false)
            {
                closed[it.x][it.y]=true;
                q.push(new State(it,0,tmp,tmp->gValue+1));
            }
        }
    }
    for(auto it:garbage) delete it;
    std::reverse(res.begin(),res.end());
    return res;
}  

BigWall rotateWall(const BigWall& w)
{
    if(w.ori=='H')
        return BigWall(Position(w.pos.x+1,w.pos.y-1),'V');
    else
        return BigWall(Position(w.pos.x-1,w.pos.y+1),'H');
    
}

bool checkWallInBound(const BigWall& w)
{
    if(w.ori=='V')
    {
        if(w.pos.x<=0||w.pos.x>8||w.pos.y<0||w.pos.y>=8)
            return false;
    }
    else // 'H'
    {
        if(w.pos.x<0||w.pos.x>=8||w.pos.y<=0||w.pos.y>8)
            return false;
    }
    return true;
}

vector<BigWall> generateWalls(const Position& pos, int extension = 7)
{
    vector<BigWall> res;
    for(int i = -extension;i<=extension;i++)
    {
        for(int j = -extension;j<=extension;j++)
        {
            BigWall tmp;
            tmp = BigWall(Position(pos.x+i,pos.y+j),'V');
            if(checkWallInBound(tmp))
                res.push_back(tmp);
            tmp = BigWall(Position(pos.x+i,pos.y+j),'H');
            if(checkWallInBound(tmp))
                res.push_back(tmp);
        }
    }
    return res;
}

vector<BigWall> generateWalls()
{
    vector<BigWall> res;
    for(int i = 0;i<=8;i++)
    {
        for(int j = 0;j<=8;j++)
        {
            BigWall tmp;
            tmp = BigWall(Position(i,j),'V');
            if(checkWallInBound(tmp))
                res.push_back(tmp);
            tmp = BigWall(Position(i,j),'H');
            if(checkWallInBound(tmp))
                res.push_back(tmp);
        }
    }
    return res;
}

vector<BigWall> checkWalls(const vector<BigWall>& wallsToBeChecked, const vector<BigWall>& originalWalls)
{
    vector<BigWall> wallsChecked;
    for(auto i:wallsToBeChecked) // ckeck conflict of potential walls
    {
        bool flag = true;
        for(auto j:originalWalls)
        {
            if(abs(i.pos.x-j.pos.x)<=1&&i.pos.y==j.pos.y&&i.ori==j.ori&&i.ori=='H'||
               i.pos.x==j.pos.x&&abs(i.pos.y-j.pos.y)<=1&&i.ori==j.ori&&i.ori=='V'||
               i.pos.x-j.pos.x==1&&i.pos.y-j.pos.y==-1&&i.ori=='V'&&j.ori=='H'||
               i.pos.x-j.pos.x==-1&&i.pos.y-j.pos.y==1&&i.ori=='H'&&j.ori=='V'
                )
            {
                flag=false;
            }   
        }
        if(flag)
            wallsChecked.push_back(i);
    }
    return wallsChecked;
}

bool checkSingleWall(const BigWall& wallToBeChecked, const vector<BigWall>& originalWalls)
{
    for(auto i:originalWalls)
    {
        if(abs(wallToBeChecked.pos.x-i.pos.x)<=1&&wallToBeChecked.pos.y==i.pos.y&&wallToBeChecked.ori==i.ori&&wallToBeChecked.ori=='H'||
           wallToBeChecked.pos.x==i.pos.x&&abs(wallToBeChecked.pos.y-i.pos.y)<=1&&wallToBeChecked.ori==i.ori&&wallToBeChecked.ori=='V'||
           wallToBeChecked.pos.x-i.pos.x==1&&wallToBeChecked.pos.y-i.pos.y==-1&&wallToBeChecked.ori=='V'&&i.ori=='H'||
           wallToBeChecked.pos.x-i.pos.x==-1&&wallToBeChecked.pos.y-i.pos.y==1&&wallToBeChecked.ori=='H'&&i.ori=='V'
            )
        {
            return false;
        }   
    }
    return true;
}

vector<SmallWall> generatePathFindingWalls(const vector<BigWall>& originalWalls)
{
    vector<SmallWall> res;
    for(auto i : originalWalls)
    {
        int wallX = i.pos.x;
        int wallY = i.pos.y;
        char wallOri = i.ori;
        res.push_back(SmallWall(Position(wallX,wallY), wallOri));
        if(i.ori=='V')
            res.push_back(SmallWall(Position(wallX,wallY+1), wallOri));
        else
            res.push_back(SmallWall(Position(wallX+1,wallY), wallOri));
    }
    return res;
}

bool noDangerChecking(const Player& player, const std::vector<BigWall>& originalWallsPlusTestWall, int prevETA)
{
    vector<BigWall> wallsToBeChecked;
    vector<BigWall> wallsChecked;
    vector<SmallWall> wallsForPathFinding;
    /********************* generate potential walls **************************/
    wallsToBeChecked=generateWalls();
    /*************************************************************************/
    /********************* check conflict of walls ***************************/
    wallsChecked = checkWalls(wallsToBeChecked, originalWallsPlusTestWall);
    wallsForPathFinding = generatePathFindingWalls(originalWallsPlusTestWall);
    for(auto i : wallsChecked)
    {
        if(i.ori=='V')
        {
            wallsForPathFinding.push_back(SmallWall(Position(i.pos.x,i.pos.y),'V'));
            wallsForPathFinding.push_back(SmallWall(Position(i.pos.x,i.pos.y+1),'V'));
        }
        else
        {
            wallsForPathFinding.push_back(SmallWall(Position(i.pos.x,i.pos.y),'H'));
            wallsForPathFinding.push_back(SmallWall(Position(i.pos.x+1,i.pos.y),'H'));
        }
        if(pathFinding(player,wallsForPathFinding).size()>prevETA)
            return false;
        wallsForPathFinding.pop_back();
        wallsForPathFinding.pop_back();
    }
    return true;
    
}

int anyGoodWallLeft(const Player& p, const vector<BigWall>& originalWallsPlusTestWall, int extension, const vector<Player>& players, const BigWall& except, const int myid)
{
    vector<BigWall> wallsToBeChecked;
    vector<BigWall> wallsChecked;
    vector<SmallWall> wallsForPathFinding;
    //int opPrevETA = p.ETA;
    /********************* generate potential walls **************************/
    wallsToBeChecked=generateWalls(p.curr, extension);
    /*************************************************************************/
    /********************* check conflict of walls ***************************/
    wallsChecked = checkWalls(wallsToBeChecked, originalWallsPlusTestWall);
    /*************************************************************************/
    /********************* generate pathFinding walls ************************/
    wallsForPathFinding = generatePathFindingWalls(originalWallsPlusTestWall);
    /*************************************************************************/
    vector<SmallWall> tmpWalls = wallsForPathFinding; 
    int opPrevETA = pathFinding(p,wallsForPathFinding).size();
    int nextETAMax = opPrevETA;
    int myPrevETA = players[myid].ETA;
    int myNextETA = -1;
    for(auto i : wallsChecked)
    {
        if(i!=except)
        {
            bool legalFlag = true;
        
            if(i.ori=='V')
            {
                tmpWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y),'V'));
                tmpWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y+1),'V'));
            }
            else
            {
                tmpWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y),'H'));
                tmpWalls.push_back(SmallWall(Position(i.pos.x+1,i.pos.y),'H'));
            }
            for(int ip = 0;ip<players.size();ip++)
            {
                int tmpETA = pathFinding(players[ip],tmpWalls).size();
                if(players[ip].alive&&ip!=p.id&&tmpETA==0)
                    legalFlag = false;
                if(players[ip].alive&&ip==myid)
                    myNextETA = tmpETA;
            }
            if(legalFlag)
            {
                vector<Position> tmpPath = pathFinding(p,tmpWalls);
                int nextETA = tmpPath.size();
                int tmpBenefit = nextETA-(myNextETA-myPrevETA);
                if(tmpBenefit>nextETAMax)
                {
                    nextETAMax = tmpBenefit;
                    //cerr<<"Good wall left found "<<i<<" with benefit "<<nextETAMax - prevETA<<endl;
                }
            }
            tmpWalls.pop_back();
            tmpWalls.pop_back();
        }
        
    }
    TIMING
    return nextETAMax - opPrevETA;
}

BigWall greedyWallAdjust(int order, const BigWall& toBeAdjust, const Player& p, const vector<BigWall>& originalWalls, const vector<Player>& players, int myid)
{
    BigWall res = toBeAdjust;
    bool flag = true;
    if(order<=2)
    {
        switch(p.id)
        {
            case 0: if(res.pos.x-1==p.curr.x&&res.pos.y==1&&res.ori=='V'&&p.curr.y==1&&order==0)
                        res.pos.y--;
                    else if(res.pos.x-1==p.curr.x&&res.pos.y==6&&res.ori=='V'&&p.curr.y==7&&order==0)
                        res.pos.y++;
                    else if(res.pos.x+1==p.curr.x&&res.pos.y<=p.curr.y&&res.ori=='H'&&p.curr.y>=7&&res.pos.x==6&&order==1) // lower one
                    {
                        res.pos.x+=2;
                        res.pos.y-=2;
                        res.ori='V';
                    }
                    else if(res.pos.x+1==p.curr.x&&res.pos.y==2&&res.ori=='H'&&p.curr.y<=1&&res.pos.x==6&&order==1) // higher one
                    {
                        res.pos.x+=2;
                        res.ori='V';
                    }
                    else if(res.pos.x+1==p.curr.x&&res.pos.y==5&&res.ori=='H'&&p.curr.y<=5&&res.pos.x==6&&order==2) // lower one
                    {
                        res.pos.x+=2;
                        res.pos.y-=2;
                        res.ori='V';
                    }
                    break;
            case 1: if(res.pos.x==p.curr.x&&res.pos.y==1&&res.ori=='V'&&p.curr.y==1&&order==0)
                    {
                        res.pos.y--;
                    }
                    else if(res.pos.x==p.curr.x&&res.pos.y==6&&res.ori=='V'&&p.curr.y==7&&order==0)
                    {
                        res.pos.y++;
                    }
                    else if(res.pos.x==p.curr.x&&res.pos.y==2&&res.ori=='H'&&p.curr.y==1&&order==1)
                        res.ori='V';
                    else if(res.pos.x==p.curr.x&&res.pos.y==7&&res.ori=='H'&&p.curr.y==7&&order==1)
                    {
                        res.pos.y-=2;
                        res.ori='V';

                    }
                    else if(res.pos.x==p.curr.x&&res.pos.y==4&&res.ori=='H'&&p.curr.y==3&&order==2)
                        res.ori='V';
                    break;
            case 2: if(res.pos.x==1&&res.pos.y-1==p.curr.y&&res.ori=='H'&&p.curr.x==1&&order==0)
                    {
                        res.pos.x--;
                    }
                    else if(res.pos.x==6&&res.pos.y-1==p.curr.y&&res.ori=='H'&&p.curr.x==7&&order==0)
                    {
                        res.pos.x++;
                    }
                    else if(res.pos.x==2&&res.pos.y+1==p.curr.y&&res.ori=='V'&&p.curr.x==1&&order==1) // left one
                    {
                        res.ori = 'H';
                        res.pos.y+=2;

                    }
                    else if(res.pos.x==7&&res.pos.y+1==p.curr.y&&res.ori=='V'&&p.curr.x==7&&order==1)
                    {
                        res.ori = 'H';
                        res.pos.y+=2;
                        res.pos.x-=2;

                    }
                    break;
        }
        if(checkSingleWall(res,originalWalls))
        {
            vector<BigWall> originalWallsPlusAdjustedWall = originalWalls;
            originalWallsPlusAdjustedWall.push_back(res);
            vector<SmallWall> pathFindingWalls = generatePathFindingWalls(originalWallsPlusAdjustedWall);
            for(auto it : players)
            {
                if(it.alive&&pathFinding(it,pathFindingWalls).size()==0)
                {
                    flag = false;
                    break;
                }
            }
        }
        else
            flag = false;
    }
    TIMING
    if(flag)
        return res;
    else
        return toBeAdjust;
}



BigWall placeWall(const vector<Player>& players, int myid, std::vector<SmallWall>& walls, 
    int& wallPlaceFlag, const std::vector<BigWall>& originalWalls, bool numPlayerChanged,
    int benefitThreshold, int numWallsLeft, int numMaxWalls, bool delayFlag)
{
    int numAlive=0;
    int numOpWallsLeft=0;
    int numPlayers = players.size();
    int wallForWhom = -1;
    vector<int> prevETA(numPlayers,-1);
    vector<int> nextETA(numPlayers,-1);
    vector<vector<Position>> currPath(numPlayers,vector<Position>());
    vector<vector<Position>> nextPath(numPlayers,vector<Position>());
    int minETA = numeric_limits<int>::max();
    int opMinETA = -1; // id
    if(delayFlag==false)
        cerr<<"Opponent is building path for himself, don't delay anything!"<<endl;

    for(int i = 0;i < numPlayers;i++)
    {
        if(players[i].alive)
        {
            prevETA[i]=players[i].ETA;
            numAlive++;
            if(i!=myid&&players[i].ETA<minETA)
            {
                minETA = players[i].ETA;
                opMinETA = i;
            }
            if(i!=myid)
            {
                numOpWallsLeft+=players[i].wallsLeft;
            }
        }
    }
    for(int ip = 0;ip < numPlayers;ip++)
    {
        if(players[ip].alive)
            currPath[ip] = pathFinding(players[ip], walls, true); 
    }
    #if DEBUG
        cerr<<"BenefitThreshold in placeWall is "<<benefitThreshold<<endl;
    #endif
    BigWall res;
    vector<BigWall> wallsToBeChecked;
    vector<BigWall> wallsChecked;
    int opNextETAMax=-1;
    int myNextETAMax=-1;
    BigWall dangerousWall;
    int max=1; // if 0, useless wall is also updated
    int benefitLeftMax=0;
    int manhattanDist=numeric_limits<int>::max();
    int numSucc = 0;
    /********************* generate potential walls ***************************/
    wallsToBeChecked=generateWalls();
    /**************************************************************************/
    /********************* check conflict of walls ****************************/
    wallsChecked = checkWalls(wallsToBeChecked, originalWalls);
    /**************************************************************************/
    /************************* make the decision ******************************/
    for(auto i:wallsChecked) // try walls
    {
        vector<SmallWall> tmpSmallWalls = walls; 
        if(i.ori=='V')
        {
            tmpSmallWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y),'V'));
            tmpSmallWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y+1),'V'));
        }
        else
        {
            tmpSmallWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y),'H'));
            tmpSmallWalls.push_back(SmallWall(Position(i.pos.x+1,i.pos.y),'H'));
        }
        int totalBenefit = 0;
        int illegalWall = false;
        int wallForWhomLocal = -1;
        int compareETA = 0;
        for(int ip = 0;ip < numPlayers;ip++)
        {
            if(players[ip].alive)
            {
                nextPath[ip] = pathFinding(players[ip], tmpSmallWalls);
                nextETA[ip] = nextPath[ip].size();
                if(nextETA[ip]==0)
                    illegalWall = true;
            }
            if(players[ip].alive&&ip!=myid)
            {
                totalBenefit=totalBenefit+(nextETA[ip]-prevETA[ip]);
                if(nextETA[ip]-prevETA[ip]>compareETA)
                {
                    compareETA = nextETA[ip]-prevETA[ip];
                    wallForWhomLocal = ip;
                }
            }
        }
        if(illegalWall)
            continue;
        int myETAInc = nextETA[myid] - prevETA[myid];
        totalBenefit = totalBenefit -myETAInc*2;
        if(totalBenefit>=max)
        {
            #if DEBUG>2
                cerr<<"TotalBenefit is "<<totalBenefit<<" for wall "<<i<<endl;
                cerr<<"wallForWhomLocal is "<<wallForWhomLocal<<", opMinETA is #"<<opMinETA<<endl;
            #endif
            if(totalBenefit==max&&wallForWhomLocal!=opMinETA&&max==1)
            {
                #if DEBUG
                    cerr<<"Skipping "<<i<<endl;
                #endif
                continue;
            }
            if(totalBenefit>max)
            {
                max = totalBenefit;
                res = i;
                numSucc = getSuccessors(players[wallForWhom].curr,tmpSmallWalls).size();
                wallForWhom = wallForWhomLocal;
                opNextETAMax = nextETA[wallForWhom];
                
            }
            else // totalBenefit == max
            {
                /*vector<BigWall> originalPlusTestWall = originalWalls;
                originalPlusTestWall.push_back(i);
                Player tmpPlayer=players[wallForWhomLocal];
                vector<Position> path = pathFinding(players[wallForWhomLocal],walls);
                tmpPlayer.curr = path[0];
                int benefitLeft = anyGoodWallLeft(players[wallForWhomLocal],originalPlusTestWall,2,players,i,myid);
                cerr<<"benefitLeft is "<<benefitLeft<<" for wall "<<i<<endl;
                if(benefitLeft==0)
                    continue;*/
                int tmpNumSucc = getSuccessors(players[wallForWhomLocal].curr,tmpSmallWalls,false).size();
                if(wallForWhomLocal==opMinETA&&tmpNumSucc>=2)
                {
                    max = totalBenefit;
                    cerr<<"Res tie resolved to "<<i<<endl;
                    res = i;
                    numSucc = tmpNumSucc;
                    wallForWhom = wallForWhomLocal;
                    opNextETAMax = nextETA[wallForWhom];
                }
                
                
            }
        }
        /*********************** Dangerous Wall Detection *********************/
        if(nextETA[myid]>myNextETAMax) 
        {
            myNextETAMax = nextETA[myid];
            dangerousWall = i;
        }
        /**********************************************************************/
    } // wall check loop completes
    TIMING
    #if DEBUG
        cerr<<"opNextETAMax after wall check loop is "<<opNextETAMax<<endl;
        cerr<<"Best wall is "<<res<<", totalBenefit is: "<<max<<endl;
        cerr<<"myNextETAMax(myPrevETA) is "<<myNextETAMax<<"("<<prevETA[myid]<<")"<<endl;
        cerr<<"Wall "<<dangerousWall<<" caused this myNextETAMax"<<endl;
    #endif
    if(myNextETAMax - prevETA[myid]>SELF_DEFENDING_THRESHOLD&&numOpWallsLeft) // check dangerous wall
    {
        #if DEBUG
            cerr<<"Find a dangerous wall for myself "<<dangerousWall<<endl;
            cerr<<"This wall may increase my ETA by "<<myNextETAMax - prevETA[myid]<<endl;
        #endif
        /********************* SELF-DEFENDING LOGIC **********************/
        vector<BigWall> wallsToBeCheckedForMe = generateWalls(players[myid].curr);
        vector<BigWall> originalPlusDangerousWall = originalWalls;
        originalPlusDangerousWall.push_back(dangerousWall);
        vector<BigWall> wallsCheckedForMe = checkWalls(wallsToBeCheckedForMe,originalPlusDangerousWall);
        int benefitFromCancellingMax=0;
        BigWall resFromCancelling;
        for(auto i:wallsCheckedForMe)
        {   
            vector<SmallWall> tmpWalls = walls; 
            vector<SmallWall> tmpWallsWithoutDangerousWall = walls;
            if(dangerousWall.ori=='V')
            {
                tmpWalls.push_back(SmallWall(Position(dangerousWall.pos.x,dangerousWall.pos.y),'V'));
                tmpWalls.push_back(SmallWall(Position(dangerousWall.pos.x,dangerousWall.pos.y+1),'V'));
            }
            else
            {
                tmpWalls.push_back(SmallWall(Position(dangerousWall.pos.x,dangerousWall.pos.y),'H'));
                tmpWalls.push_back(SmallWall(Position(dangerousWall.pos.x+1,dangerousWall.pos.y),'H'));
            }
            if(i.ori=='V')
            {
                tmpWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y),'V'));
                tmpWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y+1),'V'));
                tmpWallsWithoutDangerousWall.push_back(SmallWall(Position(i.pos.x,i.pos.y),'V'));
                tmpWallsWithoutDangerousWall.push_back(SmallWall(Position(i.pos.x,i.pos.y+1),'V'));
            }
            else
            {
                tmpWalls.push_back(SmallWall(Position(i.pos.x,i.pos.y),'H'));
                tmpWalls.push_back(SmallWall(Position(i.pos.x+1,i.pos.y),'H'));
                tmpWallsWithoutDangerousWall.push_back(SmallWall(Position(i.pos.x,i.pos.y),'H'));
                tmpWallsWithoutDangerousWall.push_back(SmallWall(Position(i.pos.x+1,i.pos.y),'H'));
            }
            vector<int> testETA(numPlayers,-1);
            
            bool cancellingSuccess = true;
            for(int i = 0;i < numPlayers;i++)
            {
                if(players[i].alive&&i==myid)
                {
                    testETA[i] = pathFinding(players[i], tmpWalls).size();
                }
                if(players[i].alive&&i!=myid)
                {
                    testETA[i] = pathFinding(players[i], tmpWallsWithoutDangerousWall).size();
                }
                
                if(players[i].alive&&i==myid&&testETA[myid])
                    cancellingSuccess = false;
                if(players[i].alive&&i!=myid&&testETA[i]==0)
                    cancellingSuccess = false;
            }
            if(cancellingSuccess)
            {
                // check the cancelling alone can block me or not
                vector<SmallWall> wallsWithCancelling = walls;
                if(i.ori=='V')
                {
                    wallsWithCancelling.push_back(SmallWall(Position(i.pos.x,i.pos.y),'V'));
                    wallsWithCancelling.push_back(SmallWall(Position(i.pos.x,i.pos.y+1),'V'));
                }
                else
                {
                    wallsWithCancelling.push_back(SmallWall(Position(i.pos.x,i.pos.y),'H'));
                    wallsWithCancelling.push_back(SmallWall(Position(i.pos.x+1,i.pos.y),'H'));
                }
                if(pathFinding(players[myid], wallsWithCancelling).size()==0)
                {
                    #if DEBUG
                        cerr<<"Cancelling wall "<<i<<" alone will block me, abort"<<endl;
                    #endif
                    continue;
                }
                #if DEBUG
                    cerr<<"Wall "<<i<<" can eliminate dangerous wall "<<dangerousWall<<endl;
                #endif
                int benefitFromCancelling = 0;
                for(int i = 0;i < numPlayers;i++)
                {
                    if(players[i].alive&&i==myid)
                        benefitFromCancelling += (myNextETAMax - prevETA[myid]);
                    if(players[i].alive&&i!=myid)
                        benefitFromCancelling += (testETA[i]-prevETA[i]);
                }
                #if DEBUG
                    cerr<<"benefitFromCancelling is "<<benefitFromCancelling<<endl;
                #endif
                if(benefitFromCancelling>benefitFromCancellingMax)
                {
                    benefitFromCancellingMax = benefitFromCancelling;
                    resFromCancelling = i;
                }
                
            }
        } // loop for check cancelling wall
        if(benefitFromCancellingMax>CANCELING_THRESHOLD)
        {
            #if DEBUG
                cerr<<"Place the cancelling wall, benefit is "<<benefitFromCancellingMax<<endl;
            #endif
            wallPlaceFlag = 2;
            return resFromCancelling;
        }
        else
        {
            #if DEBUG
                cerr<<"Failed to find a cancelling wall"<<endl;
            #endif
        }
    }
    /************************ SELF-GUIDING LOGIC **************************/
    if(myNextETAMax - prevETA[myid]>SELF_GUIDING_THRESHOLD&&numOpWallsLeft&&players[wallForWhom].ETA>=2)
    {
        vector<BigWall> originalPlusDangerousWall = originalWalls;
        originalPlusDangerousWall.push_back(dangerousWall);
        vector<BigWall> toBeChecked;
        toBeChecked.push_back(rotateWall(dangerousWall));
        if(dangerousWall.ori=='H')
        {
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x+1,dangerousWall.pos.y),'H'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x-1,dangerousWall.pos.y),'H'));
        }
        else
        {
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x,dangerousWall.pos.y+1),'V'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x,dangerousWall.pos.y-1),'V'));
        }
        #if DEBUG
            cerr<<"A slightly dangerous wall "<<dangerousWall<<" detected"<<endl;
        #endif
        for(auto it : toBeChecked)
        {
            #if DEBUG
                cerr<<"Try "<<it<<" to cancel it"<<endl;
            #endif
            if(checkSingleWall(it, originalWalls))
            {
                #if DEBUG
                    cerr<<"Wall "<<it<<" passed check"<<endl;
                #endif
                vector<BigWall> testBigWalls = originalWalls;
                testBigWalls.push_back(it);
                vector<SmallWall> testSmallWalls = generatePathFindingWalls(testBigWalls);
                vector<int> testETA(numPlayers,-1);
                bool testSuccess = true;
                int testBenefit = 0;
                for(int i = 0;i < numPlayers;i++)
                {
                    if(players[i].alive)
                    {
                        testETA[i] = pathFinding(players[i], testSmallWalls).size();
                        if(testETA[i]==0)
                            testSuccess = false;
                    }
                    if(players[i].alive&&i==myid&&testETA[myid]>prevETA[myid])
                        testSuccess = false;
                    if(players[i].alive&&i!=myid&&testETA[i]!=0)
                        testBenefit+=( testETA[i]-prevETA[i]);
                }
                
                testBenefit+=(myNextETAMax-prevETA[myid]);
                bool noDangerAnymore = noDangerChecking(players[myid], testBigWalls, prevETA[myid]);
                #if DEBUG
                    cerr<<"Self-guiding benefit "<<testBenefit<<", while max is "<<max<<endl;
                    if(noDangerAnymore)
                        cerr<<"noDangerAnymore"<<endl;
                    else
                        cerr<<"Still have dangerous wall for me"<<endl;
                #endif
                TIMING
                
                if(testSuccess/*&&testBenefit>max*/&&noDangerAnymore)
                {
                    #if DEBUG
                        cerr<<"Self-guiding is more beneficial than delay them"<<endl;
                    #endif
                    wallPlaceFlag = 3;
                    TIMING
                    return it;
                }
                else
                {
                    #if DEBUG
                        cerr<<"No good self-guiding wall detected"<<endl;
                    #endif
                }
            }
        }
        
            
        
    }
    /*else
    {
        #if DEBUG
            cerr<<"No dangerous wall detected"<<endl;
        #endif
    }*/
    TIMING
    /**********************************************************************/
    benefitThreshold = min(4,benefitThreshold);
    int threshold = 0;
    if(numMaxWalls!=players[myid].wallsLeft&&numPlayerChanged==false)
        threshold = benefitThreshold;
    if(players[myid].wallsLeft<=1)
        threshold++;
    if(numMaxWalls-players[myid].wallsLeft==0||numPlayerChanged||(numAlive==2&&prevETA[wallForWhom]<=2))
        threshold=1;
    if(numMaxWalls-players[myid].wallsLeft==1||numOpWallsLeft==0||prevETA[wallForWhom]<=2)
        threshold=min(threshold,2);
    if(delayFlag==false)
        threshold--;
    /**************************** DELAY WALL TEST ***************************/
    if(res.ori!='?')
    {
        vector<Position> path = pathFinding(players[wallForWhom],walls);
        
        #if DEBUG
            cerr<<"Max is "<<max<<", threshold is "<<threshold<<endl;
            cerr<<"Best wall is "<<res<<", mainly for player #"<<wallForWhom<<endl;
            cerr<<"Player #"<<wallForWhom<<" current position"<<players[wallForWhom].curr<<endl;
            cerr<<"Player #"<<wallForWhom<<" predicted next position"<<path[0]<<endl;
        #endif
        if(max>=threshold)
        {
            if(delayFlag&&max-threshold<=4)
            {
                Player tmpPlayer = players[wallForWhom];
                tmpPlayer.curr = path[0];
                int benefitLeft = anyGoodWallLeft(tmpPlayer, originalWalls, 1,players, res, myid);
                cerr<<"BenefitLeft from anyGoodWall is "<<benefitLeft<<endl;
                if(benefitLeft>=max)
                {
                    wallPlaceFlag=0;
                    #if DEBUG
                        cerr<<"Still have good wall, delay "<<res<<endl;
                    #endif
                }
                else
                {
                    #if DEBUG
                        cerr<<"No good wall left, don't delay "<<res<<endl;
                    #endif
                    std::vector<SmallWall> tmpWalls = walls;
                
                    if(res.ori=='V')
                    {
                        tmpWalls.push_back(SmallWall(res.pos,'V'));
                        tmpWalls.push_back(SmallWall(Position(res.pos.x,res.pos.y+1),'V'));
                    }
                    else
                    {
                        tmpWalls.push_back(SmallWall(res.pos,'H'));
                        tmpWalls.push_back(SmallWall(Position(res.pos.x+1,res.pos.y),'H'));
                    }
                    int tmpTotalBenefit=0;
                    bool tmpSuccess = true;
                    for(int ip = 0;ip < numPlayers;ip++)
                    {
                        Player tmpPlayer = players[ip];
                        int tmpETA = -1;
                        if(tmpPlayer.alive)
                        {
                            tmpPlayer.curr = currPath[ip][0];
                            tmpETA = pathFinding(tmpPlayer, tmpWalls).size();
                            if(ip!=myid)
                            {
                                if(tmpETA)
                                    tmpTotalBenefit = tmpTotalBenefit + tmpETA + 1 - prevETA[ip];
                                else
                                    tmpSuccess = false;
                            }
                        }
                    }
    
                    #if DEBUG
                        cerr<<"If the wall is placed next round, newBenefit(curBenefit) would be "<<tmpTotalBenefit<<"("<<max<<")"<<endl;
                    #endif
                    if(tmpTotalBenefit>=max&&tmpSuccess)
                    {
                        wallPlaceFlag=0;
                        #if DEBUG
                            cerr<<"Delay wall "<<res<<" to trap him deeper!"<<endl;
                        #endif
                    }
                    else
                    {
                        wallPlaceFlag=1;
                        if(delayFlag)
                            res = greedyWallAdjust(numMaxWalls-numWallsLeft,res,players[wallForWhom], originalWalls,players,myid);
                        #if DEBUG
                            cerr<<"Place wall "<<res<<" succeed after delay test, benefit is "<<max<<endl;
                        #endif
                    }
                }
            }
            else
            {
                wallPlaceFlag=1;
                res = greedyWallAdjust(numMaxWalls-numWallsLeft,res,players[wallForWhom], originalWalls,players,myid);
                #if DEBUG
                    cerr<<"Place wall "<<res<<" succeed, benefit is "<<max<<endl;
                #endif
            }
        }
        else
        {
            wallPlaceFlag=0;
            cerr<<"Place wall "<<res<<" failed due to little benefit!"<<endl;
        }
    }
    
    TIMING
    return res;
}

int main()
{
    int w; // width of the board
    int h; // height of the board
    int playerCount; // number of players (2,3, or 4)
    int id; // id of my player (0 = 1st player, 1 = 2nd player, ...)
    cin >> w >> h >> playerCount >> id; cin.ignore();
    string res = "ERROR";
    int round=0;
    int prevNumAlive=playerCount;
    bool prevPlaceWallFlag=false;
    int benefitThreshold=1;
    int numMaxWalls;
    int prevWallCount = 0;
    int myPrevETA=-1;
    int prevWallPlace = -1;
    //bool delayFlag = true;
    
    bool stopThem = false;
    switch(playerCount)
    {
        case 2: numMaxWalls = 10;
                break;
        case 3: numMaxWalls = 6;
                break;
        default:cerr<<"Something wrong with playerCount"<<endl;
                break;
    }
    // game loop
    while (1) 
    {
        int currWallCount = 0;
        int myCurrETA = -1;
        beginTiming = clock();
        round++;
        Position curr;
        Position dest;
        std::vector<Player> players;
        std::vector<vector<Position> > paths;
        int numAlive=0;
        vector<BigWall> originalWalls;
        vector<SmallWall> walls;

        for (int i = 0; i < playerCount; i++) 
        {
            int x; // x-coordinate of the player
            int y; // y-coordinate of the player
            int wallsLeft; // number of walls available for the player
            cin >> x >> y >> wallsLeft; cin.ignore();

            if(wallsLeft!=-1) // alive
            {
                numAlive++;
                curr.x = x;
                curr.y = y;
                dest.x = x;
                dest.y = y;
                switch(i)
                {
                    case 0: dest.x = w-1; break;
                    case 1: dest.x = 0;   break;
                    case 2: dest.y = h-1; break;
                    case 3: dest.y = 0;   break;
                    default:cerr<< "ERROR in determine destination"<<endl; break;
                }
                players.push_back(Player(i, wallsLeft, curr, dest, true));
                if(i==id)
                    players[i].isMe = true;
            }
            else 
            {
                players.push_back(Player(i, wallsLeft, curr, dest, false));
            }
        }
        #if DEBUG
            cerr<<"There are "<<numAlive<<" players alive!"<<endl;
        #endif
        int wallCount; // number of walls on the board
        cin >> wallCount; cin.ignore();
        for (int i = 0; i < wallCount; i++) 
        {
            int wallX; // x-coordinate of the wall
            int wallY; // y-coordinate of the wall
            string wallOrientation; // wall orientation ('H' or 'V')
            cin >> wallX >> wallY >> wallOrientation; cin.ignore();
            originalWalls.push_back(BigWall(Position(wallX,wallY),wallOrientation[0]));
        }
        currWallCount = wallCount;
        walls = generatePathFindingWalls(originalWalls);
        #if DEBUG==10
            for(auto it : walls)
                cerr<<"Wall: "<<it.pos.x<<", "<<it.pos.y<<endl;
        #endif
        for (int i = 0; i < playerCount; i++) 
        {
            switch(i)
            {
                case 0: if(players[0].alive&&(players[0].curr.x>=7||(players[0].curr.x>=5&&wallCount))) 
                            stopThem=true;
                        break;
                case 1: if(players[1].alive&&(players[1].curr.x<=1||(players[1].curr.x<=3&&wallCount)))
                            stopThem=true;
                        break;
                case 2: if(players[2].alive&&(players[2].curr.y==7||(players[2].curr.y>=5&&wallCount))) 
                            stopThem=true;
                        break;
                default:cerr<<"Something wrong with loop variable i"<<endl;
                        break;
                
            }
        }
        if(wallCount>=2)
            stopThem = true;
        
        for (int i = 0; i < playerCount ; i++) 
        {
            if(players[i].alive)
            {
                if(i==id)
                {
                    paths.push_back(pathFinding(players[i],walls,true));
                    myCurrETA = paths[id].size();
                }
                else
                {
                    paths.push_back(pathFinding(players[i],walls,false));
                }
                players[i].ETA = paths[i].size();
            }
            else
            {
                paths.push_back(vector<Position>());
                players[i].ETA = -1;
            }
            #if DEBUG
                if(players[i].alive)
                {
                    if(i==id&&players[i].alive)
                        cerr<<"ETA for player #"<<i<<"(myself): "<<players[i].ETA<<endl;
                    else
                        cerr<<"ETA for player #"<<i<<" : "<<players[i].ETA<<endl;
                }
            #endif
        }
        TIMING
        int placeWallFlag = 0;
        BigWall wall;
        bool delayFlag = true;
        cerr<<"currWallCount: "<<currWallCount<<", prevWallCount: "<<prevWallCount<<endl;
        cerr<<"myCurrETA: "<<myCurrETA<<", myPrevETA: "<<myPrevETA<<endl;
        if(prevWallPlace)
        {
            if(numAlive==2&&currWallCount>prevWallCount&&myCurrETA<=myPrevETA) 
                delayFlag = false;
        }
        else
        {
            if(numAlive==2&&currWallCount>prevWallCount&&myCurrETA<myPrevETA) 
                delayFlag = false;
        }
        if(stopThem&&players[id].wallsLeft)
        {
            bool numPlayerChanged = false;
            bool leading = true;
            for (int i = 0; i < playerCount ; i++) 
            {
                if(players[i].alive&&i!=id&&players[i].ETA<=players[id].ETA+LEADING_OFS)
                {
                    leading = false;
                    break;
                }
            }
            if(numAlive!=prevNumAlive)
            {
                numPlayerChanged =true; 
                prevNumAlive=numAlive;
            }
            if(leading)
            {
                #if DEBUG
                    cerr<<"I'm leading, try to find a surprise wall for them"<<endl;
                #endif    
                wall = placeWall(players,id,walls,placeWallFlag,originalWalls,numPlayerChanged,SURPRISE_THRESHOLD,players[id].wallsLeft,numMaxWalls,delayFlag);
            }
            else
            {
                #if DEBUG
                    cerr<<"I'm not leading, try to stop them!"<<endl;
                #endif
                wall = placeWall(players,id,walls,placeWallFlag,originalWalls,numPlayerChanged,benefitThreshold,players[id].wallsLeft,numMaxWalls,delayFlag);
            }
            if(placeWallFlag==1)
            {
                benefitThreshold++;
            }
            else
            {
                benefitThreshold--;
                if(benefitThreshold==0)
                    benefitThreshold = 1;
                else if(benefitThreshold<2) 
                    benefitThreshold = 2;
            }
        }

        Position next =  paths[id][0];
        #if DEBUG
            cerr<<"My curr is "<<players[id].curr.x<<", "<<players[id].curr.y<<endl;
            cerr<<"My next is "<<next.x<<", "<<next.y<<endl;
        #endif
        if(next.x-players[id].curr.x)
        {
            if(next.x-players[id].curr.x>0)
                res = "RIGHT";
            else
                res = "LEFT";
        }
        else // x unchanged
        {
            if(next.y-players[id].curr.y>0)
                res = "DOWN";
            else
                res = "UP";
        }
        double elapsed_secs = double(clock() - beginTiming) / CLOCKS_PER_SEC;
        int ms = elapsed_secs*1000;
        vector<string> greetings({" Asgard"," Muahahaha!"," This is self defense!"," This is self guiding!"});
        switch(placeWallFlag)
        {
            case 0: cout << res <<" "<<ms<<"ms"<<endl;//greetings[placeWallFlag]<< endl; // action: LEFT, RIGHT, UP, DOWN or "putX putY putOrientation" to place a wall
                    break;
            case 1: cout << wall.pos.x<<" "<<wall.pos.y<<" "<<wall.ori<<" "<<ms<<"ms"<<endl;//greetings[placeWallFlag]<< endl;
                    break;
            case 2: cout << wall.pos.x<<" "<<wall.pos.y<<" "<<wall.ori<<greetings[2]<< endl;
                    break;
            case 3: cout << wall.pos.x<<" "<<wall.pos.y<<" "<<wall.ori<<greetings[3]<< endl;
                    break;
        }
        if(placeWallFlag)
            currWallCount++;
        prevWallCount = currWallCount;
        myPrevETA = myCurrETA;
        prevWallPlace = placeWallFlag;
    } // game loop
}