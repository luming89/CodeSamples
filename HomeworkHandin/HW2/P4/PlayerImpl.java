/***************************************************************************************
  PlayerImpl.java
  Implement five functions in this file.
  ---------
  Licensing Information:  You are free to use or extend these projects for
  educational purposes provided that (1) you do not distribute or publish
  solutions, (2) you retain this notice, and (3) you provide clear
  attribution to UW-Madison.
 
  Attribution Information: The Take Stone Games was developed at UW-Madison.
  The initial project was developed by Jerry(jerryzhu@cs.wisc.edu) and his TAs.
  Current version with depthLimit and SBE was developed by Fengan Li(fengan@cs.wisc.edu)
  and Chuck Dyer(dyer@cs.wisc.edu)
  
*****************************************************************************************/

import java.util.*;

public class PlayerImpl implements Player 
{
	private final static boolean DEBUG = false;//true;//
	// Identifies the player
	private int name = 0;
	int n = 0;
	

	// Constructor
	public PlayerImpl(int name, int n) {
		this.name = name;//0;
		this.n = n;
	}

	// Function to find possible successors
	@Override
	public ArrayList<Integer> generateSuccessors(int lastMove, int[] takenList) 
	{
		// TODO Add your code here
		ArrayList<Integer> res  = new ArrayList<Integer>();
		/*if(DEBUG)
			System.out.print("I'm player "+name+", last move is "+lastMove);*/
		if(lastMove==-1) // initial move
		{
			for(int i = 1;i<n/2+n%2;i++)
			{
				if(i%2==1)
					res.add(i);
			}
		}
		else // non-initial move
		{
			for(int i = 1;i<=n;i++)
			{
				if(takenList[i]==0&&(i%lastMove==0||lastMove%i==0))
					res.add(i);
			}
		}
		/*if(DEBUG)
		{
			System.out.print(", successors are ");
			for(int i = 0;i<res.size();i++)
				System.out.print(res.get(i)+", ");
			System.out.println();
		}*/
		return res;
	}

	// The max value function
	@Override
	public double max_value(GameState s, int depthLimit) 
	{
		// TODO Add your code here
		/*if(DEBUG)
			System.out.println("In max_value: Gamestate(lastMove:"+s.lastMove+")");*/
		double res = -Double.MAX_VALUE;
		ArrayList<Integer> succ = generateSuccessors(s.lastMove,s.takenList);
		if(succ.size()==0)
			s.leaf = true;
		else 
			s.leaf = false;
		if(s.leaf)
		{
			res = -1.0;
			s.bestMove = -1;
		}
		else
		{
			if(depthLimit==0)
			{
				res = stateEvaluator(s);
			}
				
			else
			{
				for(int i=succ.size()-1;i>=0;i--)
				{
					int lastMove = succ.get(i);
					int[] takenList = s.takenList.clone(); // I mean copy it
					takenList[lastMove] = name; // taken
					double tmp = min_value(new GameState(takenList,lastMove),depthLimit>0? depthLimit-1: -1);
					if(tmp>res)
					{
						res = tmp;
						s.bestMove = succ.get(i);
						if(res==1) break;
					}
				}
			}
			

		}
		if(DEBUG)
			System.out.println("Gamestate(lastMove:"+s.lastMove+")'s max value is "+res);
		return res;
		//return 0;
	}

	// The min value function
	@Override
	public double min_value(GameState s, int depthLimit)
	{
		// TODO Add your code here
		/*if(DEBUG)
			System.out.println("In min_value: Gamestate(lastMove:"+s.lastMove+")");*/
		double res = Double.MAX_VALUE;
		ArrayList<Integer> succ = generateSuccessors(s.lastMove,s.takenList);
		if(succ.size()==0)
			s.leaf = true;
		if(s.leaf)
		{
			res = 1.0;
			s.bestMove = -1;
		}
		else
		{
			if(depthLimit==0)
				res = -stateEvaluator(s);
			else
			{
				
				for(int i=succ.size()-1;i>=0;i--)
				{
					int lastMove = succ.get(i);
					int[] takenList = s.takenList.clone(); // I mean copy it
					takenList[lastMove] = name; // taken
					double tmp = max_value(new GameState(takenList,lastMove),depthLimit>0? depthLimit-1:-1);
					if(tmp<res)
					{
						res = tmp;
						s.bestMove = succ.get(i);
						if(res == -1) break;
					}
				}
			}
			

		}
		if(DEBUG)
			System.out.println("Gamestate(lastMove:"+s.lastMove+")'s min value is "+res);
		return res;
	}
	
	// Function to find the next best move
	@Override
	public int move(int lastMove, int[] takenList, int depthLimit) 
	{
		// TODO Add your code here
		GameState s = new GameState(takenList,lastMove);
		max_value(s, depthLimit);
		if(DEBUG)
			System.out.println("bestMove is "+s.bestMove);
		return s.bestMove;

	}
	
	// The static board evaluator function
	@Override
	public double stateEvaluator(GameState s)
	{
		// TODO Add your code here
		if(s.takenList[1]==0)
		{
			return 0;
		}
		else if(s.lastMove==1)
		{
			int numLegalMoves = generateSuccessors(s.lastMove,s.takenList).size();
			if(numLegalMoves%2==1)
			{
				return 0.5;
			}
			else
			{
				return -0.5;
			}
		}
		else if(isPrime(s.lastMove))
		{
			ArrayList<Integer> succ = generateSuccessors(s.lastMove,s.takenList);
			int count = 0;
			for(int i=0;i<succ.size();i++)
			{
				if(succ.get(i)%s.lastMove==0)
					count++;
			}
			if(count%2==1)
				return 0.7;
			else
				return -0.7;
		}
		else // guaranteed to be composite
		{
			int prime=-1; // initialize to an invalid value
			for(int i=s.lastMove-1;i>1;i--)
			{
				if(isPrime(i)&&s.lastMove%i==0)
				{
					prime = i;
					break;
				}
			}
			ArrayList<Integer> succ = generateSuccessors(s.lastMove,s.takenList);
			int count = 0;
			for(int i=0;i<succ.size();i++)
			{
				if(succ.get(i)%prime==0)
					count++;
			}
			if(count%2==1)
				return 0.6;
			else
				return -0.6;
		}
		//return 0;
	}

	private boolean isPrime(long n)
	{
		if(n < 2) return false;
	    if(n == 2 || n == 3) return true;
	    if(n%2 == 0 || n%3 == 0) return false;
	    long sqrtN = (long)Math.sqrt(n)+1;
	    for(long i = 6L; i <= sqrtN; i += 6) {
	        if(n%(i-1) == 0 || n%(i+1) == 0) return false;
	    }
	    return true;
	}

	/*private static boolean isPrime(int num) 
	{
	    if (num % 2 == 0) return false;
	    for (int i = 3; i * i <= num; i += 2)
	        if (num % i == 0) return false;
	    return true;
	}*/
}
