/**************************************************************************/
/* Author: Luming Zhang                                                   */
/* Date 02/20/2015                                                        */
/**************************************************************************/ 

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>
#include <unordered_set>

#define DEBUG 0 // 1 for critial info, 3 for detailed info
#define LEADING_OFS 10
#define SURPRISE_THRESHOLD 5 // 5
#define SELF_DEFENDING_THRESHOLD 5 // 5
#define CANCELING_THRESHOLD 5 // 5
#define SELF_GUIDING_THRESHOLD 1 // Set to 1 to explore more oppotunity

#define GENERATE_WALL_OFS 7 // 7
#define BFS//ASTAR//
#define TIMING do{double elapsed_secs = double(clock() - beginTiming) / CLOCKS_PER_SEC; cerr<<"Elapsed time: "<<elapsed_secs*1000<<"ms in "<<__FUNCTION__<<" line: "<<__LINE__<<endl;} while(0);

#define VERSION 13/1133

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
    Player(const int id, const int wallsLeft, const Position& curr, const Position& dest, bool alive=false) 
    : id(id), wallsLeft(wallsLeft), curr(curr), dest(dest), alive(alive), ETA(-1) {}
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
        if(curr.x==dest.x)
            return true;
        else
            return false;
    else
        if(curr.y==dest.y)
            return true;
        else
            return false;
}

int calcManhattanDist(const Position& curr, const Position& dest)
{
    return abs(curr.x-dest.x)+abs(curr.y-dest.y);
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

vector<Position> getSuccessors(const Position& curr, const vector<SmallWall>& walls, bool heuristic = false, int id = -1)
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
                if(id<=1&&i%2==1)
                    next.priority++;
                else if(id==2&&i%2==0)
                    next.priority++;
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
        vector<Position> succ = getSuccessors(tmp->pos,walls,heuristic,player.id);
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

vector<BigWall> generateWalls(int myid=0)
{
    vector<BigWall> res;
    if(myid==1)
    {
        for(int i = 8;i>=0;i--)
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
    }
    else
    {
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
            return false;
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
    int opNextETAMax = opPrevETA;
    int myPrevETA = players[myid].ETA;
    int myNextETA = -1;
    int benefit = 0;
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
                if(players[ip].alive&&tmpETA==0)
                    legalFlag = false;
                if(players[ip].alive&&ip==myid)
                    myNextETA = tmpETA;
            }
            if(legalFlag)
            {
                vector<Position> tmpPath = pathFinding(p,tmpWalls);
                int opNextETA = tmpPath.size();
                int tmpBenefit = opNextETA-(myNextETA-myPrevETA)*2-opPrevETA;
                if(tmpBenefit>benefit)
                    benefit = tmpBenefit;
            }
            tmpWalls.pop_back();
            tmpWalls.pop_back();
        }
    }
    TIMING
    return benefit;
}

BigWall greedyWallAdjust(int order, const BigWall& toBeAdjust, const Player& p, const vector<BigWall>& originalWalls, const vector<Player>& players, int myid)
{
    BigWall res = toBeAdjust;
    bool flag = true;
    if(order<=1) // 2
    {
        switch(p.id)
        {
            case 0: if(res.pos.x-1>=p.curr.x&&res.pos.y==1&&res.ori=='V'&&p.curr.y==1&&order==0) // upper one
                        res.pos.y--;
                    else if(res.pos.x-1>=p.curr.x&&res.pos.y==6&&res.ori=='V'&&p.curr.y==7&&order==0) // lower one
                        res.pos.y++;
                    
                    else if(res.pos.x-1==p.curr.x&&res.pos.y==6&&res.ori=='V'&&p.curr.y==6&&order==0)
                        res.pos.y--;
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
    if(flag)
        return res;
    else
        return toBeAdjust;
}

BigWall placeWall(const vector<Player>& players, int myid, std::vector<SmallWall>& walls, 
    int& wallPlaceFlag, const std::vector<BigWall>& originalWalls, bool numPlayerChanged,
    int benefitThreshold, bool delayFlag, int opNumBuildPath, int& prevTarget, int& prevBenefit)
{

    int numAlive=0;
    int numOpWallsLeft=0;
    int numPlayers = players.size();
    int wallForWhom = -1;
    int numMaxWalls = -1;
    vector<int> prevETA(numPlayers,-1);
    vector<int> nextETA(numPlayers,-1);
    vector<vector<Position>> currPath(numPlayers,vector<Position>());
    vector<vector<Position>> nextPath(numPlayers,vector<Position>());
    if(numPlayers==2)
        numMaxWalls = 10;
    else
        numMaxWalls = 6;
    #if DEBUG
        cerr<<"prevTarget is "<<prevTarget<<endl;
        if(delayFlag==false)
            cerr<<"Opponent is building path for himself, don't delay anything!"<<endl;
    #endif
    for(int i = 0;i < numPlayers;i++)
    {
        if(players[i].alive)
        {
            prevETA[i]=players[i].ETA;
            numAlive++;

            if(i!=myid)
            {
                numOpWallsLeft+=players[i].wallsLeft;
            }
        }
    }
    if(numOpWallsLeft==0)
        delayFlag=true;
    for(int ip = 0;ip < numPlayers;ip++)
    {
        if(players[ip].alive)
            currPath[ip] = pathFinding(players[ip], walls, true); 
    }
    #if DEBUG>1
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
    wallsToBeChecked=generateWalls(myid);
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
            #if DEBUG
                cerr<<"TotalBenefit is "<<totalBenefit<<" for wall "<<i<<endl;
                cerr<<"wallForWhomLocal is "<<wallForWhomLocal<<endl;
            #endif
            if(totalBenefit>max)
            {
                max = totalBenefit;
                res = i;
                numSucc = getSuccessors(players[wallForWhomLocal].curr,tmpSmallWalls,false,wallForWhomLocal).size();
                wallForWhom = wallForWhomLocal;
                opNextETAMax = nextETA[wallForWhom];
            }
            else // totalBenefit == max
            {
                if(prevTarget!=-1&&wallForWhomLocal!=prevTarget)
                    continue;
                    
                int tmpNumSucc = getSuccessors(players[wallForWhomLocal].curr,tmpSmallWalls,false,wallForWhomLocal).size();
                if((prevTarget==-1||wallForWhomLocal==prevTarget)&&(tmpNumSucc>=2||players[wallForWhomLocal].ETA>3))
                {
                    max = totalBenefit;
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
        cerr<<"Best wall is "<<res<<", totalBenefit is: "<<max<<endl;
        cerr<<"Dangerous Wall "<<dangerousWall<<" caused my ETA increased by "<<myNextETAMax-prevETA[myid]<<endl;
    #endif
    if(res.ori=='?') 
        max=0;
    if(myNextETAMax - prevETA[myid]>SELF_DEFENDING_THRESHOLD&&numOpWallsLeft) // check dangerous wall
    {
        /********************* SELF-DEFENDING LOGIC **********************/
        vector<BigWall> wallsToBeCheckedForMe = generateWalls(players[myid].curr);
        vector<BigWall> originalPlusDangerousWall = originalWalls;
        originalPlusDangerousWall.push_back(dangerousWall);
        vector<BigWall> wallsCheckedForMe = checkWalls(wallsToBeCheckedForMe,originalPlusDangerousWall);
        int benefitFromCancellingMax=0;
        BigWall resFromCancelling;
        Player tmpMyself = players[myid];
        tmpMyself.curr = currPath[myid][0];
        if(pathFinding(tmpMyself, generatePathFindingWalls(originalPlusDangerousWall)).size()==prevETA[myid]-1)
        {
            wallPlaceFlag=0;
            return res;
        }
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
                int cancelETA = pathFinding(players[myid], wallsWithCancelling).size();
                if(cancelETA==0)
                    continue;
                
                int benefitFromCancelling = 0;
                for(int i = 0;i < numPlayers;i++)
                {
                    if(players[i].alive&&i==myid)
                        benefitFromCancelling += (myNextETAMax - cancelETA);
                    if(players[i].alive&&i!=myid)
                        benefitFromCancelling += (testETA[i]-prevETA[i]);
                }
                
                #if DEBUG>1
                    cerr<<"Wall "<<i<<" can cancel this dangerous wall"<<endl;
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
            #if DEBUG>1
                cerr<<"Place the cancelling wall "<<resFromCancelling<<", benefit is "<<benefitFromCancellingMax<<endl;
            #endif
            wallPlaceFlag = 2;
            return resFromCancelling;
        }
        else
        {
            #if DEBUG>1
                cerr<<"Failed to find a cancelling wall"<<endl;
            #endif
        }
    }
    /************************ SELF-GUIDING LOGIC **************************/
    if(myNextETAMax - prevETA[myid]>SELF_GUIDING_THRESHOLD&&numOpWallsLeft&&calcManhattanDist(players[myid].curr,dangerousWall.pos)<5&&(wallForWhom==-1||players[wallForWhom].ETA>=2))
    {
        vector<BigWall> originalPlusDangerousWall = originalWalls;
        originalPlusDangerousWall.push_back(dangerousWall);
        vector<BigWall> toBeChecked;
        toBeChecked.push_back(rotateWall(dangerousWall));
        if(dangerousWall.ori=='H')
        {
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x+1,dangerousWall.pos.y),'H'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x-1,dangerousWall.pos.y),'H'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x,dangerousWall.pos.y-1),'V'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x+2,dangerousWall.pos.y-1),'V'));
        }
        else // 'V'
        {
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x,dangerousWall.pos.y+1),'V'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x,dangerousWall.pos.y-1),'V'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x-1,dangerousWall.pos.y),'H'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x-1,dangerousWall.pos.y+2),'H'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x,dangerousWall.pos.y+1),'H'));
            toBeChecked.push_back(BigWall(Position(dangerousWall.pos.x-2,dangerousWall.pos.y+1),'H'));
        }
        #if DEBUG
            cerr<<"A slightly dangerous wall "<<dangerousWall<<" detected"<<endl;
        #endif
        for(auto it : toBeChecked)
        {
            #if DEBUG
                cerr<<"Try wall "<<it<<endl;
            #endif
            if(it==res)
            {
                wallPlaceFlag = 3;
                return it;
            }
            if(checkWallInBound(it)&&checkSingleWall(it, originalWalls))
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
                if(testSuccess&&((testBenefit-4>max&&max<=2)||(noDangerAnymore&&testBenefit>3))) //
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
    TIMING
    /**********************************************************************/
    benefitThreshold = min(4,benefitThreshold);
    int threshold = 0;
    if(numMaxWalls!=players[myid].wallsLeft&&numPlayerChanged==false)
        threshold = benefitThreshold;
    if(players[myid].wallsLeft<=1&&players.size()==2)
        threshold++;
    if(numMaxWalls-players[myid].wallsLeft==0||numPlayerChanged||(numAlive==2&&prevETA[wallForWhom]<=2))
        threshold=1;
    if(numMaxWalls-players[myid].wallsLeft==1||numOpWallsLeft==0||prevETA[wallForWhom]<=2)
        threshold=min(threshold,2);
    if(opNumBuildPath>1) 
        threshold--;
    if(numAlive==3&&prevTarget!=-1&&wallForWhom!=prevTarget)
        threshold++;
    /**************************** DELAY WALL TEST ***************************/
    if(res.ori!='?')
    {
        #if DEBUG
            if(delayFlag)
                cerr<<"delayFlag is on"<<endl;
            else
                cerr<<"delayFlag is off"<<endl;
        #endif
        vector<Position> path = pathFinding(players[wallForWhom],walls);
        #if DEBUG
            cerr<<"Max is "<<max<<", threshold is "<<threshold<<endl;
            cerr<<"Best wall is "<<res<<", mainly for player #"<<wallForWhom<<endl;
            cerr<<"Player #"<<wallForWhom<<" current position"<<players[wallForWhom].curr<<endl;
            cerr<<"Player #"<<wallForWhom<<" predicted next position"<<path[0]<<endl;
        #endif
        if(max>=threshold)
        {
            if(delayFlag&&max-threshold<=5)
            {
                Player tmpPlayer = players[wallForWhom];
                tmpPlayer.curr = path[0];
                int benefitLeft = anyGoodWallLeft(tmpPlayer, originalWalls, 1,players, res, myid);
                #if DEBUG
                    cerr<<"BenefitLeft from anyGoodWall is "<<benefitLeft<<endl;
                #endif
                if(benefitLeft>=max||(benefitLeft==2&&max==3&&players[myid].wallsLeft>=2&&players[wallForWhom].ETA>4))
                {
                    wallPlaceFlag=0;
                }
                else // benefitLeft<max
                {
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
                        if(tmpPlayer.alive&&tmpPlayer.ETA>1)
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
                        if(numAlive==2&&numOpWallsLeft&&players[myid].wallsLeft-2>numOpWallsLeft) // set to >2 rather than >=2 to trap him deeper
                        {
                            wallPlaceFlag=1;
                            res = greedyWallAdjust(numMaxWalls-players[myid].wallsLeft,res,players[wallForWhom], originalWalls,players,myid);
                            #if DEBUG
                                cerr<<"Place wall "<<res<<" succeed after delay test, benefit is "<<max<<endl;
                            #endif
                        }
                        else
                        {
                            wallPlaceFlag=0;
                            #if DEBUG
                                cerr<<"Delay wall "<<res<<" to trap him deeper!"<<endl;
                            #endif
                        }
                        
                    }
                    else
                    {
                        wallPlaceFlag=1;
                        //if(delayFlag) // why this?
                        res = greedyWallAdjust(numMaxWalls-players[myid].wallsLeft,res,players[wallForWhom], originalWalls,players,myid);
                        #if DEBUG
                            cerr<<"Place wall "<<res<<" succeed after delay test, benefit is "<<max<<endl;
                        #endif
                    }
                }
            }
            else
            {
                wallPlaceFlag=1;
                res = greedyWallAdjust(numMaxWalls-players[myid].wallsLeft,res,players[wallForWhom], originalWalls,players,myid);
                #if DEBUG
                    cerr<<"Place wall "<<res<<" succeed, benefit is "<<max<<endl;
                #endif
                
            }
        }
        else
        {
            wallPlaceFlag=0;
            #if DEBUG
                cerr<<"Place wall "<<res<<" failed due to little benefit!"<<endl;
            #endif
        }
    }
    if(wallPlaceFlag==1)
    {
        prevTarget = wallForWhom;
        prevBenefit = max;
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
    int prevWallCount = 0;
    int myPrevETA=8;
    int prevWallPlace = -1;
    int opNumBuildPath = 0;
    bool stopThem = false;
    int prevTarget=-1;
    int prevBenefit=-1;
    int myPrevSucc = -1;
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
            }
            else 
            {
                players.push_back(Player(i, wallsLeft, curr, dest, false));
            }
        }
        #if DEBUG
            cerr<<"There are "<<numAlive<<" players alive in round "<<round<<"!"<<endl;
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
        for (int i = 0; i < playerCount; i++) 
        {
            switch(i)
            {
                case 0: if(players[0].alive&&(players[0].curr.x>=6||(players[0].curr.x>=5&&wallCount))) 
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
        int myCurrSucc = getSuccessors(players[id].curr,walls).size();
        if(prevWallPlace)
        {
            if(numAlive==2&&currWallCount>prevWallCount&&myCurrETA<=myPrevETA&&myCurrSucc==myPrevSucc) 
                opNumBuildPath+=2;
        }
        else
        {
            if(numAlive==2&&currWallCount>prevWallCount&&myCurrETA<myPrevETA&&myCurrSucc==myPrevSucc) 
                opNumBuildPath+=2;
        }
        if(opNumBuildPath||(playerCount==2&&players[id].wallsLeft-players[1-id].wallsLeft>1&&round<12))
            delayFlag = false;
        if(opNumBuildPath)
            stopThem = true;
        if(stopThem&&players[id].wallsLeft&&players[id].ETA>1)
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
            wall = placeWall(players,id,walls,placeWallFlag,originalWalls,numPlayerChanged,leading?SURPRISE_THRESHOLD:benefitThreshold,delayFlag,opNumBuildPath, prevTarget, prevBenefit);
            
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
        if(placeWallFlag)
        {
            originalWalls.push_back(wall);
            myPrevSucc = getSuccessors(players[id].curr,generatePathFindingWalls(originalWalls)).size();
        }
        else // I move 
        {
            myPrevSucc = getSuccessors(next, walls).size();
        }
        if(opNumBuildPath&&placeWallFlag)
            opNumBuildPath--;
    } // game loop
}