/***************************************************************************************
  GameState.java
  Do not modify this file!
  ---------
  Licensing Information:  You are free to use or extend these projects for
  educational purposes provided that (1) you do not distribute or publish
  solutions, (2) you retain this notice, and (3) you provide clear
  attribution to UW-Madison
 
  Attribution Information: The Take Stone Games was developed at UW-Madison.
  The initial project was developed by Jerry(jerryzhu@cs.wisc.edu) and his TAs.
  Current version with depthLimit and SBE was developed by Fengan Li(fengan@cs.wisc.edu)
  and Chuck Dyer(dyer@cs.wisc.edu)
  
*****************************************************************************************/
import java.util.*;

public class GameState {
	public boolean leaf = false;// true if this state is a leaf
	int[] takenList;// the takenlist for this state
	int lastMove; // the last move which led to this game state
	int bestMove;// best possible next move

	public GameState(int[] takenList, int lastMove) {
		this.takenList = takenList.clone();
		this.lastMove = lastMove;
	}

}
