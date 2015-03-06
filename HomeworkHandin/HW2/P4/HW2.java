/***************************************************************************************
  HW2.java
  Do not modify this file!
  ---------
  Licensing Information:  You are free to use or extend this project for
  educational purposes provided that (1) you do not distribute or publish
  solutions, (2) you retain this notice, and (3) you provide clear
  attribution to UW-Madison
 
  Attribution Information: The Take Stone Games was developed at UW-Madison.
  The initial project was developed by Jerry(jerryzhu@cs.wisc.edu) and his TAs.
  Current version with depthLimit and SBE was developed by Fengan Li(fengan@cs.wisc.edu)
  and Chuck Dyer(dyer@cs.wisc.edu)
  
*****************************************************************************************/
import java.util.*;

public class HW2 {

	public static void main(String args[]) {
		int n = Integer.parseInt(args[0]);
		int mode = Integer.parseInt(args[1]);
		int depthLimit = Integer.parseInt(args[2]);
		if (n < 5) {
			System.out.println("Value of n is too small to play the game");
			return;
		}

		if (mode != 1 && mode != 2 && mode != 3) {
			System.out.println("Invalid mode");
			return;
		}

		// Creating the board with n elements
		Board board = new Board(n);

		// Creating two players: 1 and 2
		PlayerImpl player1 = new PlayerImpl(1, n);
		PlayerImpl player2 = new PlayerImpl(2, n);

		Scanner scn = new Scanner(System.in);

		// Variable that determines the winner
		int winner = 0;

		// The loop where the game is played

		while (true)
		{
			if (mode == 1 || mode == 3) {
				// Player 1's move
				int lastMove = board.getLastMove();
				int[] takenList = board.getTakenListCopy();
				int mvPl1 = player1.move(lastMove, takenList, depthLimit);
				// Player 1 looses
				if (mvPl1 == -1) {
					winner = 2;
					break;
				}
				System.out.println("Player 1: " + mvPl1);
				// Player 1's move on the board

				board.move(mvPl1, 1);
			}

			if (mode == 2) {
				// Player 1's move
				int lastMove = board.getLastMove();
				int[] takenList = board.getTakenListCopy();
				ArrayList<Integer> succ = player1.generateSuccessors(lastMove,
						takenList);
				if (succ.isEmpty()) {
					winner = 2;
					break;
				}

				System.out.print("Player 1: ");
				int mvPl1 = scn.nextInt();

				while (succ.indexOf(mvPl1) == -1) {
					System.out.println("Invalid move");
					System.out.print("Player 1: ");
					mvPl1 = scn.nextInt();
				}

				// Player 1's move on the board
				board.move(mvPl1, 1);

			}

			if (mode == 3) {
				// Player 2's move
				int lastMove = board.getLastMove();
				int[] takenList = board.getTakenListCopy();
				ArrayList<Integer> succ = player2.generateSuccessors(lastMove,
						takenList);
				if (succ.isEmpty()) {
					winner = 1;
					break;
				}

				System.out.print("Player 2: ");
				int mvPl2 = scn.nextInt();

				while (succ.indexOf(mvPl2) == -1) {
					System.out.println("Invalid move");
					System.out.print("Player 2: ");
					mvPl2 = scn.nextInt();
				}

				// Player 2's move on the board
				board.move(mvPl2, 2);

			}

			if (mode == 1 || mode == 2) {
				// Player 2's move
				int lastMove = board.getLastMove();
				int[] takenList = board.getTakenListCopy();
				int mvPl2 = player2.move(lastMove, takenList, depthLimit);
				// Player 2 loses
				if (mvPl2 == -1) {
					winner = 1;
					break;
				}
				System.out.println("Player 2: " + mvPl2);

				// Player 2's move on the board
				board.move(mvPl2, 2);
			}

		}

		System.out.println("Winner: Player " + winner);
	}
}
