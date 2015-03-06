/***************************************************************************************
  Player.java
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

public interface Player {
	public int move(int lastMove, int[] takenList, int depthLimit);

	public ArrayList<Integer> generateSuccessors(int lastMove, int[] takenList);

	public double max_value(GameState s, int depthLimit);

	public double min_value(GameState s, int depthLimit);

	public double stateEvaluator(GameState s);
	

}
