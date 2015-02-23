import java.util.ArrayList;
import java.util.LinkedList;
import java.io.*;

/**
 * Depth-First Search (DFS)
 * 
 * You should fill the search() method of this class.
 */
public class DepthFirstSearcher extends Searcher 
{

	/**
	 * Calls the parent class constructor.
	 * 
	 * @see Searcher
	 * @param maze initial maze.
	 */
	public DepthFirstSearcher(Maze maze) {
		super(maze);
	}

	private final boolean DEBUG = false;

	/**
	 * Main depth first search algorithm.
	 * 
	 * @return true if the search finds a solution, false otherwise.
	 */
	public boolean search() 
	{
		// FILL THIS METHOD

		// CLOSED list is a 2D Boolean array that indicates if a state associated with a given position in the maze has already been expanded.
		boolean[][] closed = new boolean[maze.getNoOfRows()][maze.getNoOfCols()];

		// ...

		// Stack implementing the Frontier list
		LinkedList<State> stack = new LinkedList<State>();
		State root = new State(maze.getPlayerSquare(),null,0,0,0);
		stack.push(root);
		//noOfNodesExpanded = 1;
		while (!stack.isEmpty()) 
		{
			// TODO return true if find a solution

			// TODO maintain the cost, noOfNodesExpanded
			// TODO update the maze if a solution found

			// use stack.pop() to pop the stack.

			// use stack.push(...) to elements to stack
			State top = stack.pop();
			noOfNodesExpanded+=1;
			//closed[top.getX()][top.getY()]=true;
			if(DEBUG)
			{

				maze.setOneSquare(top.getSquare(), '.'); 
				IO.printOutput(maze, getCost(),	getNoOfNodesExpanded());
				maze.setOneSquare(top.getSquare(), ' '); 
			}
			
			if(top.isGoal(maze))
			{
				cost = top.getDepth();
				State backtracking = top.getParent();
				while(backtracking!=null)
				{
					maze.setOneSquare(backtracking.getSquare(), '.'); 
					backtracking = backtracking.getParent();
				}
				maze.setOneSquare(maze.getPlayerSquare() , 'H'); 				maze.setOneSquare(maze.getGoalSquare(), 'C');
				return true;
			}
			ArrayList<State> succ = top.getSuccessors(closed, maze);
		
			for(int i = succ.size() - 1; i >= 0; i--)
			{
			  State tmp = top;
			  boolean flag = true;
			  while(tmp.getParent()!=null)
			  {
			    tmp = tmp.getParent();
			    if(succ.get(i).getSquare().X==tmp.getSquare().X&&succ.get(i).getSquare().Y==tmp.getSquare().Y)
			    {
			      flag = false;
			    }
			  }
			  if(flag) stack.push(succ.get(i));
			}
		}

		// TODO return false if no solution
		return false;
	}
}
