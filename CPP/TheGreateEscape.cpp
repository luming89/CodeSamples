#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#define DEBUG 1

#define BFS//ASTAR//

#define VERSION 1.5

using namespace std;

class Position
{
public:
    int x;
    int y;
    Position() : x(0), y(0){}
    Position(int x, int y): x(x), y(y){}
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
    Player(const int id, const int wallsLeft, const Position& curr, const Position& dest, bool alive=true) 
    : id(id), wallsLeft(wallsLeft), curr(curr), dest(dest), alive(alive), ETA(-1)
    {}
};

class Wall
{
public: 
    Position pos;
    char ori;
    bool whole;
    Wall() : pos(Position(-1,-1)), ori('?'), whole(true) {} // invalid wall
    Wall(const Position& s, const char o, const bool whole=true) : pos(s), ori(o), whole(whole){}
};

void printPath(const vector<Position>& path)
{
    for(auto it:path)
        cerr<<"Path: "<<it.x<<", "<<it.y<<endl;
        
}

int calcH(const Position& curr, const Position& dest, const int id)
{
    if(id<2)
        return abs(curr.x-dest.x);
    else
        return abs(curr.y-dest.y);
}

int calcManhattanDist(const Position& curr, const Position& dest)
{
    return abs(curr.x-dest.x)+abs(curr.y-dest.y);
}

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

vector<Position> getSuccessors(const Position& curr, const vector<Wall>& walls)
{
    int dir[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    vector<Position> succ;
    
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
            succ.push_back(next);
    }
    return succ;
}

vector<Position> pathFinding(const Player& player, const std::vector<Wall>& walls)
{
    vector<Position> res;
    vector<vector<bool> > closed(9,vector<bool>(9,false));
    vector<State*> garbage;
    #if DEBUG
        //cerr<<"Going to find a path for player #"<<player.id<<endl;
        //cerr<<"Start point: "<<player.curr.x<<", "<<player.curr.y<<endl;
        //cerr<<"Destination point: "<<player.dest.x<<", "<<player.dest.y<<endl;
    #endif

    #ifdef BFS /*************** BFS **************/
    //cerr<<"Using BFS!"<<endl;
    State* opt=NULL;
    int minPath=std::numeric_limits<int>::max();
    queue<State*> q;
    q.push(new State(player.curr,0));
    int h = calcH(player.curr,player.dest,player.id);
    while(q.empty()==false)
    {
        State* tmp = q.front();
        garbage.push_back(tmp);
        q.pop();
        if(checkDest(tmp->pos, player.dest, player.id))
        {  
            while(tmp->prev)
            {
                //err<<"Prev is "<<tmp->prev<<endl;
                res.push_back(tmp->pos);
                tmp=tmp->prev;
                //cerr<<"Pos of tmp is "<<tmp->pos.x<<", "<<tmp->pos.y<<endl;
            }
            break;
    
        }
        vector<Position> succ = getSuccessors(tmp->pos,walls);
        for(auto it : succ)
        {
            if(closed[it.x][it.y]==false)
            {
                closed[it.x][it.y]=true;
                q.push(new State(it,0,tmp,tmp->gValue+1));
            }
        }
    }
    
    
    #else /*************** ASTAR **************/
    //cerr<<"Using ASTAR!"<<endl;
    auto lambda = [](const State* s1, const State* s2){return s1->hValue>s2->hValue;};
    priority_queue<State*,vector<State*>,decltype(lambda)> pq(lambda);
    
    pq.push(new State(player.curr,calcH(player.curr,player.dest,player.id)));
    closed[player.curr.x][player.curr.y]=true;
    while(pq.empty()==false) // find a path
    {
        State* tmp = pq.top();
        garbage.push_back(tmp);
        pq.pop();
        //cerr<<"Pos "<<tmp->pos.x<<", "<<tmp->pos.y<<" is popped"<<endl;
        if(checkDest(tmp->pos, player.dest, player.id))
        {  
            //cerr<<"Found a path to destination for player #"<<player.id<<"!"<<endl;
            while(tmp->prev->prev)
            {
                //err<<"Prev is "<<tmp->prev<<endl;
                res.push_back(tmp->pos);
                tmp=tmp->prev;
                //cerr<<"Pos of tmp is "<<tmp->pos.x<<", "<<tmp->pos.y<<endl;
            }
            res.push_back(tmp->pos);
            break;
        }
        vector<Position> succ = getSuccessors(tmp->pos,walls);
        for(auto it : succ)
        {
            int h = calcH(it,player.dest,player.id);
            //cerr<<"Position "<<
            if(closed[it.x][it.y]==false)
            {
                pq.push(new State(it,h,tmp));
                closed[it.x][it.y]=true;
            }
        }
    }
    #endif // if ALG
    for(auto it:garbage) delete it;
    std::reverse(res.begin(),res.end());
    return res;
}  

bool checkWallInBound(const Wall& w)
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

Wall placeWall(const vector<Player>& players, int opid, int myid, std::vector<Wall>& walls, 
    bool& wallPlaceFlag, const std::vector<Wall>& originalWalls, int numAlive,
    bool numPlayerChanged,int benefitThreshold)
{
    cerr<<"BenefitThreshold in placeWall is "<<benefitThreshold<<endl;
    #if DEBUG
        cerr<<"Try to place a wall for player #"<<players[opid].id<<endl;
    #endif
    Wall res= Wall(Position(),'?');
    vector<Wall> wallsToBeChecked;
    vector<Wall> wallsChecked;
    int opPrevETA = players[opid].ETA;
    int myPrevETA = players[myid].ETA;
    
    vector<Position> path = pathFinding(players[opid],walls);
    for(int i = -4;i<=4;i++)
    {
        for(int j = -4;j<=4;j++)
        {
            Wall tmp;
            tmp = Wall(Position(players[opid].curr.x+i,players[opid].curr.y+j),'V');
            if(checkWallInBound(tmp))
                wallsToBeChecked.push_back(tmp);
            tmp = Wall(Position(players[opid].curr.x+i,players[opid].curr.y+j),'H');
            if(checkWallInBound(tmp))
                wallsToBeChecked.push_back(tmp);
        }
    }
    
    cerr<<"There are "<<wallsToBeChecked.size()<<" walls to be checked"<<endl;

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
                //cerr<<"Place wall("<<i.pos.x<<", "<<i.pos.y<<", "<<i.ori<<") failed due to conflict!"<<endl;
            }   

        }
        if(flag)
        {
            //cerr<<"Wall("<<i.pos.x<<", "<<i.pos.y<<", "<<i.ori<<") is valid!"<<endl;
            wallsChecked.push_back(i);
        }
            
    }
    //cerr<<"There are "<<wallsChecked.size()<<" valid walls to be tested"<<endl;
    int opNextETAMax;
    int max=0;
    int manhattanDist=std::numeric_limits<int>::max();
    for(auto i:wallsChecked)
    {
        vector<Wall> tmp = walls; 
        if(i.ori=='V')
        {
            tmp.push_back(Wall(Position(i.pos.x,i.pos.y),'V',false));
            tmp.push_back(Wall(Position(i.pos.x,i.pos.y+1),'V',false));
        }
        else
        {
            tmp.push_back(Wall(Position(i.pos.x,i.pos.y),'H',false));
            tmp.push_back(Wall(Position(i.pos.x+1,i.pos.y),'H',false));
        }
        int opNextETA = pathFinding(players[opid], tmp).size();
        int myNextETA = pathFinding(players[myid], tmp).size();
        #if DEBUG==10
            cerr<<"ETA increased "<<opNextETA-opPrevETA<<" for player #"<<opid<<endl;
            cerr<<"ETA increased "<<myNextETA-myPrevETA<<" for myself"<<endl;
        #endif
        if(numAlive==3)
        {
            int tmpManhattanDist = calcManhattanDist(i.pos,players[opid].curr);
            if(opNextETA-opPrevETA>=max&&myNextETA<=myPrevETA&&myNextETA&&opNextETA)
            {
                if(opNextETA-opPrevETA>max)
                {
                    max = opNextETA-opPrevETA;
                    res = i;
                    opNextETAMax = opNextETA;
                    manhattanDist = tmpManhattanDist;
                }
                else  //
                {
                    if(tmpManhattanDist<manhattanDist)
                    {
                        max = opNextETA-opPrevETA;
                        res = i;
                        opNextETAMax = opNextETA;
                        manhattanDist = tmpManhattanDist;
                    }
                    
                }
                
            }
        }
        else
        {
            int tmpManhattanDist = calcManhattanDist(i.pos,players[opid].curr);
            if(opNextETA-opPrevETA-(myNextETA-myPrevETA)>=max&&myNextETA&&opNextETA)
            {
                max = opNextETA-opPrevETA-(myNextETA-myPrevETA);
                res = i;
                opNextETAMax = opNextETA;
                manhattanDist = tmpManhattanDist;
            }
        }
    }
    int wallsMax;
    if(players.size()==3)
    {
        wallsMax = 6;
    }
    else
    {
        wallsMax = 10;
    }
    benefitThreshold = min(2,benefitThreshold);
    int threshold = 0;
    if(wallsMax==players[myid].wallsLeft||numPlayerChanged)
        threshold = 1;
    else
        threshold = 1+benefitThreshold;
    cerr<<"Max is "<<max<<", threshold is "<<threshold<<endl;
    cerr<<"Player #"<<opid<<" current position("<<players[opid].curr.x<<", "<<players[opid].curr.y<<")"<<endl;
    cerr<<"Player #"<<opid<<" predicted next position("<<path[0].x<<", "<<path[0].y<<")"<<endl;
    if(max>=threshold)
    {
        //threshold++;
        Player tmpPlayer = players[opid];
        tmpPlayer.curr = path[0];
        std::vector<Wall> tmpWalls = walls;
        
        if(res.ori=='V')
        {
            tmpWalls.push_back(Wall(res.pos,'V',false));
            tmpWalls.push_back(Wall(Position(res.pos.x,res.pos.y+1),'V',false));
        }
        else
        {
            tmpWalls.push_back(Wall(res.pos,'H',false));
            tmpWalls.push_back(Wall(Position(res.pos.x+1,res.pos.y),'H',false));
        }
        
        vector<Position> newPath = pathFinding(tmpPlayer,tmpWalls);
        int tmpNextETA= newPath.size();
        cerr<<"If the wall is placed next round, his newETA(curETA) would be "<<tmpNextETA<<"("<<opNextETAMax<<")"<<endl;
        if(tmpNextETA>=opNextETAMax)
        {
            wallPlaceFlag=false;
            cerr<<"Delay wall ("<<res.pos.x<<", "<<res.pos.y<<", "<<res.ori<<") to trap him deeper!"<<endl;
        }
        else
        {
            wallPlaceFlag=true;
            cerr<<"Place wall ("<<res.pos.x<<", "<<res.pos.y<<", "<<res.ori<<") succeed, benefit is "<<max<<endl;
        }
        
        return res;
    }
    else
    {
        wallPlaceFlag=false;
        cerr<<"Place wall failed!"<<endl;
        return Wall(Position(),'?');
    }
}

int main()
{
    int w; // width of the board
    int h; // height of the board
    int playerCount; // number of players (2,3, or 4)
    int id; // id of my player (0 = 1st player, 1 = 2nd player, ...)
    cin >> w >> h >> playerCount >> id; cin.ignore();
    cerr << "My ID is: " << id << endl;
    string res = "ERROR";
    int round=0;
    int prevNumAlive=playerCount;
    bool prevPlaceWallFlag=false;
    int benefitThreshold=0;
    // game loop
    while (1) 
    {
        //cerr<<"Round "<<round<<" starts!"<<endl;
        round++;
        Position curr;
        Position dest;
        std::vector<Player> players;
        std::vector<vector<Position> > paths;
        int numAlive=0;
        
        vector<Wall> originalWalls;
        vector<Wall> walls;

        for (int i = 0; i < playerCount; i++) 
        {
            int x; // x-coordinate of the player
            int y; // y-coordinate of the player
            int wallsLeft; // number of walls available for the player
            cin >> x >> y >> wallsLeft; cin.ignore();
            #if DEBUG==10
                cerr<<"Got input: "<<x<<", "<<y<<", "<<wallsLeft<<endl;
            #endif
            if(wallsLeft!=-1)
            {
                numAlive++;
                //cerr<<"Player #"<<i<<" is alive!"<<endl;
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
                players.push_back(Player(i, wallsLeft, curr, dest));
            }
            else // ==-1
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
            originalWalls.push_back(Wall(Position(wallX,wallY),wallOrientation[0],true));
            walls.push_back(Wall(Position(wallX,wallY), wallOrientation[0],false));
            if(wallOrientation[0]=='V')
                walls.push_back(Wall(Position(wallX,wallY+1), wallOrientation[0],false));
            else
                walls.push_back(Wall(Position(wallX+1,wallY), wallOrientation[0],false));
        }
        #if DEBUG==10
            for(auto it : walls)
                cerr<<"Wall: "<<it.pos.x<<", "<<it.pos.y<<endl;
        #endif
        
        
        for (int i = 0; i < playerCount ; i++) 
        {
            if(players[i].alive)
            {
                paths.push_back(pathFinding(players[i],walls));
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
        #if DEBUG==10
            cerr<<"Path length is: "<<paths[id].size()<<endl;
            for(auto it : paths[id])
                cerr<<"Path: "<<it.x<<", "<<it.y<<endl;
        #endif
        //string alt;
        bool placeWallFlag = false;
        Wall wall;
        for (int i = 0; i < playerCount ; i++) 
        {
            if(players[i].alive&&i!=id&&round>6&&players[id].wallsLeft)
            {
               
                bool numPlayerChanged = false;
                if(players[i].ETA<=players[id].ETA) 
                {
                    if(players[i].ETA==players[id].ETA&&id==0) // if i'm id 0, don't slow down, just run!
                        continue;
                    cerr<<"Player #"<<i<<" is no behind of me, place a wall for him!"<<endl;
                    
                    if(numAlive!=prevNumAlive)
                    {
                        numPlayerChanged =true;
                        prevNumAlive=numAlive;
                    }
                    wall = placeWall(players,i,id,walls,placeWallFlag,originalWalls,numAlive,numPlayerChanged,benefitThreshold);
                    if(placeWallFlag == true)
                    {
                        //prevPlaceWallFlag = true;
                        benefitThreshold++;
                        break;
                    }
                    else
                    {
                        benefitThreshold=1;
                    }
                }
                else 
                {
                    cerr<<"Try to find a surprise wall!";
                    // Make a try to see if we can get surprise
                    wall = placeWall(players,i,id,walls,placeWallFlag,originalWalls,numAlive,numPlayerChanged,4);
                    if(placeWallFlag == true)
                    {
                        cerr<<"This wall is from surprise!!!"<<endl;
                        break;
                    }
                            
                }
            }
        }
        Position next =  paths[id][0];
        cerr<<"Curr is "<<players[id].curr.x<<", "<<players[id].curr.y<<endl;
        cerr<<"Next is "<<next.x<<", "<<next.y<<endl;
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
        
        if(placeWallFlag)
            cout << wall.pos.x<<" "<<wall.pos.y<<" "<<wall.ori<<" Stop there!" << endl;
        else
            cout << res <<" Have fun! By Asgard"<< endl; // action: LEFT, RIGHT, UP, DOWN or "putX putY putOrientation" to place a wall
    }
}