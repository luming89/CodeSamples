/***************************************************************************************
  Board.java
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
public class Board {

	private int[] takenList = null; // an array to figure out which numbers
										// have been taken
	private int lastMove = -1; // the last move made

	public Board(int n) {
		takenList = new int[n + 1];
	}

	// A move on the board made by a player
	public void move(int num, int player) {
		if (num == 0) {
			System.out.println("Wrong move");
			System.exit(0);
		}

		lastMove = num;
		takenList[num] = player;

	}

	public int getLastMove() {
		return this.lastMove;
	}

	public int[] getTakenListCopy() {
		return this.takenList.clone();
	}
}
