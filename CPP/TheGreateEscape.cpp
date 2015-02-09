#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#define DEBUG 1

#define BFS//ASTAR//

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
    #ifdef BFS /*************** BFS **************/
    cerr<<"Using BFS!"<<endl;
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
            
            cerr<<"Find a path in BFS, gValue is "<<tmp->gValue<<", shortest path length is "<<h<<endl;
            if(tmp->gValue<minPath)
            {
                opt = tmp;
                minPath = tmp->gValue;
                if(minPath==h)
                {
                    break;
                }
                   
            }
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
    while(opt->prev->prev)
    {
        //err<<"Prev is "<<tmp->prev<<endl;
        res.push_back(opt->pos);
        opt=opt->prev;
        cerr<<"Pos of opt is "<<opt->pos.x<<", "<<opt->pos.y<<endl;
    }
    res.push_back(opt->pos);
    
    #else /*************** ASTAR **************/
    cerr<<"Using ASTAR!"<<endl;
    auto lambda = [](const State* s1, const State* s2){return s1->hValue>s2->hValue;};
    priority_queue<State*,vector<State*>,decltype(lambda)> pq(lambda);
    #if DEBUG==10
        cerr<<"Going to find a path for player #"<<player.id<<endl;
        cerr<<"Start point: "<<player.curr.x<<", "<<player.curr.y<<endl;
        cerr<<"Destination point: "<<player.dest.x<<", "<<player.dest.y<<endl;
    #endif
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

Wall placeWall(const vector<Player>& players, int opid, int myid, std::vector<Wall>& walls, bool& wallPlaceFlag, const std::vector<Wall>& originalWalls)
{
    #if DEBUG
        cerr<<"Try to place a wall for player #"<<players[opid].id<<endl;
    #endif
    Wall res= Wall(Position(),'?');
    vector<Wall> wallsToBeChecked;
    vector<Wall> wallsChecked;
    int opPrevETA = players[opid].ETA;
    int myPrevETA = players[myid].ETA;
    vector<Position> path = pathFinding(players[opid],walls);
    
    if(opid==0)
    {
        if(path[0].y<8)
            wallsToBeChecked.push_back(Wall(Position(path[0].x,path[0].y),'V'));   
        if(path[0].y>0)
            wallsToBeChecked.push_back(Wall(Position(path[0].x,path[0].y-1),'V'));
        //if(players[opid].curr.x<8)
            //wallsToBeChecked.push_back(Wall(Position(players[opid].curr.x,players[opid].curr.y),'H'));   
        //if(players[opid].curr.y<8)
            //wallsToBeChecked.push_back(Wall(Position(players[opid].curr.x,players[opid].curr.y+1),'H'));
    }
    else if(opid==1)
    {
        if(players[opid].curr.y<8)
            wallsToBeChecked.push_back(Wall(Position(players[opid].curr.x,players[opid].curr.y),'V'));   
        if(players[opid].curr.y>0)
            wallsToBeChecked.push_back(Wall(Position(players[opid].curr.x,players[opid].curr.y-1),'V'));
        if(players[opid].curr.x<8)
            wallsToBeChecked.push_back(Wall(Position(players[opid].curr.x,players[opid].curr.y),'H'));   
        if(players[opid].curr.y<8)
            wallsToBeChecked.push_back(Wall(Position(players[opid].curr.x,players[opid].curr.y+1),'H'));
    }
    else
    {
        if(path[0].x<8)
            wallsToBeChecked.push_back(Wall(Position(path[0].x,path[0].y),'H'));
        if(path[0].x>0)
            wallsToBeChecked.push_back(Wall(Position(path[0].x-1,path[0].y),'H'));
    }
    cerr<<"There are "<<wallsToBeChecked.size()<<" walls to be checked"<<endl;

    for(auto i:wallsToBeChecked) // ckeck potential walls
    {
        bool flag = true;
        for(auto j:originalWalls)
        {
            if(abs(i.pos.x-j.pos.x)<=1&&i.pos.y==j.pos.y&&i.ori==j.ori&&i.ori=='H'||
               i.pos.x==j.pos.x&&abs(i.pos.y-j.pos.y)<=1&&i.ori==j.ori&&i.ori=='V'||
               i.pos.x-j.pos.x==-1&&i.pos.y-j.pos.y==1&&i.ori=='V'&&j.ori=='H'||
               i.pos.x-j.pos.x==1&&i.pos.y-j.pos.y==-1&&i.ori=='H'&&j.ori=='V'
                )
            {
                flag=false;
                cerr<<"Place wall("<<i.pos.x<<", "<<i.pos.y<<") failed due to conflict!"<<endl;
            }       
        }
        if(flag)
            wallsChecked.push_back(i);
    }
    cerr<<"There are "<<wallsChecked.size()<<" valid walls to be tested"<<endl;

    int max=0;
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
        #if DEBUG
            cerr<<"ETA increased "<<opNextETA-opPrevETA<<" for player #"<<opid<<endl;
            cerr<<"ETA increased "<<myNextETA-myPrevETA<<" for myself"<<endl;
        #endif
        
        if(opNextETA-opPrevETA>max&&myNextETA<=myPrevETA&&myNextETA)
        {
            max = opNextETA-opPrevETA;
            res = i;
        }
    }
    if(max>=2)
    {
        wallPlaceFlag=true;
        cerr<<"Place wall succeed, benefit is "<<max<<endl;
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
    
    // game loop
    while (1) 
    {
        
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
            #if DEBUG
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
                if(i==id)
                    cerr<<"ETA for player #"<<i<<"(myself): "<<players[i].ETA<<endl;
                else
                    cerr<<"ETA for player #"<<i<<" : "<<players[i].ETA<<endl;
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
            if(players[i].alive)
            {
                cerr<<"i is "<<i<<", playerCount is "<<playerCount<<endl;
                if(i==id)
                    cerr<<"My ETA is "<<players[id].ETA<<endl;
                else
                    cerr<<"player #"<<i<<" ETA is "<<players[i].ETA<<endl;
                if(players[i].ETA<players[id].ETA&&players[id].wallsLeft)
                {
                    cerr<<"Player #"<<i<<" is ahead of me!"<<endl;
                    wall = placeWall(players,i,id,walls,placeWallFlag,originalWalls);
                    if(placeWallFlag == true)
                        break;
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
            cout << res <<" Have fun!"<< endl; // action: LEFT, RIGHT, UP, DOWN or "putX putY putOrientation" to place a wall
    }
}