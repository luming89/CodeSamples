import java.util.ArrayList;
import java.util.PriorityQueue;

/**
 * A* algorithm search for Artificial Intelligence Homework 1
 * Author: Luming Zhang
 * Email: luming@cs.wisc.edu 
 */
public class AStarSearcher extends Searcher {

	/**
	 * Calls the parent class constructor.
	 * 
	 * @see Searcher
	 * @param maze initial maze.
	 */
	public AStarSearcher(Maze maze) {
		super(maze);
	}

	/**
	 * Main a-star search algorithm.
	 * 
	 * @return true if the search finds a solution, false otherwise.
	 */
	public boolean search() {

		// CLOSED list is a Boolean array that indicates if a state associated with a given position in the maze has already been expanded. 
		boolean[][] closed = new boolean[maze.getNoOfRows()][maze.getNoOfCols()];

		// OPEN list (aka Frontier list)
		PriorityQueue<StateFValuePair> open = new PriorityQueue<StateFValuePair>();

		State root = new State(maze.getPlayerSquare(),null,0,0);
		int h = Math.abs(maze.getGoalSquare().X-root.getX())+Math.abs(maze.getGoalSquare().Y-root.getY());
		
		
		open.add(new StateFValuePair(root,h));

		while (!open.isEmpty()) 
		{

			// use open.poll() to extract the minimum stateFValuePair.
			// use open.add(...) to add stateFValue pairs
			StateFValuePair min = open.poll();
			noOfNodesExpanded+=1;
			closed[min.getState().getX()][min.getState().getY()]=true;
			if(min.getState().isGoal(maze))
			{
				cost = min.getState().getDepth();
				State backtracking = min.getState().getParent();
				while(backtracking!=null)
				{
					maze.setOneSquare(backtracking.getSquare(), '.'); 
					backtracking = backtracking.getParent();
				}
				maze.setOneSquare(maze.getPlayerSquare() , 'H'); 
				return true;
			}
			ArrayList<State> succ = min.getState().getSuccessors(closed, maze);

			for(int i = succ.size() - 1; i >= 0; i--)
			{
				h = Math.abs(maze.getGoalSquare().X-succ.get(i).getX())+Math.abs(maze.getGoalSquare().Y-succ.get(i).getY());
				open.add(new StateFValuePair(succ.get(i),min.getState().getGValue()+h));

			}
		}

		return false;
	}

}
