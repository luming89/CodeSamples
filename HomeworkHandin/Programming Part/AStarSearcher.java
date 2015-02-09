import java.util.ArrayList;
import java.util.PriorityQueue;

/**
 * A* algorithm search
 * 
 * You should fill the search() method of this class.
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

		// FILL THIS METHOD

		// CLOSED list is a Boolean array that indicates if a state associated with a given position in the maze has already been expanded. 
		boolean[][] closed = new boolean[maze.getNoOfRows()][maze.getNoOfCols()];
		// ...

		// OPEN list (aka Frontier list)
		PriorityQueue<StateFValuePair> open = new PriorityQueue<StateFValuePair>();

		// TODO initialize the root state and add
		// to OPEN list
		// ...
		State root = new State(maze.getPlayerSquare(),null,0,0,0);
		int h = Math.abs(maze.getGoalSquare().X-root.getX())+Math.abs(maze.getGoalSquare().Y-root.getY());
		
		//State goal = new State(maze.getGoalSquare(),null,0,0);
		
		open.add(new StateFValuePair(root,h));
		ArrayList<StateFValuePair> expanded = new ArrayList<StateFValuePair>();
		while (!open.isEmpty()) 
		{
			// TODO return true if a solution has been found
			// TODO maintain the cost, noOfNodesExpanded,
			// TODO update the maze if a solution found

			// use open.poll() to extract the minimum stateFValuePair.
			// use open.add(...) to add stateFValue pairs
			StateFValuePair min = open.poll();
			expanded.add(min);	
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
			   boolean flag = true;
			   for(int j = 0; j< expanded.size();j++)
	                   {
			     if(succ.get(i).getSquare().X==expanded.get(j).getState().getSquare().X&&succ.get(i).getSquare().Y==expanded.get(j).getState().getSquare().Y)
			     {
				if(succ.get(i).getGValue()>=expanded.get(j).getState().getGValue())
				{
				   flag=false;
				}
				else
				{
				   expanded.remove(j);
				}
			     }
			   }
			   StateFValuePair[] frontiers = open.toArray(new StateFValuePair[0]);
			   for(int j = 0; j< frontiers.length;j++)
                           {
                             if(succ.get(i).getSquare().X==frontiers[j].getState().getSquare().X&&succ.get(i).getSquare().Y==frontiers[j].getState().getSquare().Y)
                             {
                                if(succ.get(i).getGValue()>=frontiers[j].getState().getGValue())
                                {
                                   flag=false;
                                }
                                else
                                {
                                   open.remove(frontiers[j]);
                                }
                             }
                           }
			   if(flag)
			   { 
				h = Math.abs(maze.getGoalSquare().X-succ.get(i).getX())+Math.abs(maze.getGoalSquare().Y-succ.get(i).getY());
				open.add(new StateFValuePair(succ.get(i),min.getState().getGValue()+h));
			   }
			}
		}
		return false;
	}

}
