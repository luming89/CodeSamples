import java.util.ArrayList;

/**
 * A state in the search represented by the (x,y) coordinates of the square and
 * the parent. In other words a (square,parent) pair where square is a Square,
 * parent is a State.
 * 
 * You should fill the getSuccessors(...) method of this class.
 * 
 */
public class State {

	private Square square;
	private State parent;

	// Maintain the gValue (the distance from start)
	// You may not need it for the DFS but you will
	// definitely need it for AStar
	private int gValue;

	// States are nodes in the search tree, therefore each has a depth.
	private int depth;
	private int direction; // 0 for U, 1 for R, 2 for D, 3 for L
	/**
	 * @param square
	 *            current square
	 * @param parent
	 *            parent state
	 * @param gValue
	 *            total distance from start
	 */
	public State(Square square, State parent, int gValue, int depth, int d) 
	{
		this.square = square;
		this.parent = parent;
		this.gValue = gValue;
		this.depth = depth;
		this.direction = d;
	}

	/**
	 * @param visited
	 *            closed[i][j] is true if (i,j) is already expanded
	 * @param maze
	 *            initial maze to get find the neighbors
	 * @return all the successors of the current state
	 */
	public ArrayList<State> getSuccessors(boolean[][] closed, Maze maze) 
	{
		// FILL THIS METHOD
		if(maze == null)
		{
     		   throw new IllegalArgumentException("Maze cannot be null");
    		}
		// TODO check all four neighbors (up, right, down, left)
		ArrayList<State> res = new ArrayList<State>();
		int rows = maze.getNoOfRows();
		int cols = maze.getNoOfCols();
		int X = this.square.X;
		int Y = this.square.Y;
		if(X>0) // has up
		{
			if(closed[X-1][Y]==false&&maze.getSquareValue(X-1,Y)!='%')
			{
				res.add(new State(new Square(X-1,Y),this,this.gValue+1,this.depth+1,0)); // up unvisited
			}
		}
		if(Y<cols-1) // has right
		{
			if(closed[X][Y+1]==false&&maze.getSquareValue(X,Y+1)!='%')
			{
				res.add(new State(new Square(X,Y+1),this,this.gValue+1,this.depth+1,1));// right unvisited
			}
		}
		if(X<rows-1) // has down
		{
			if(closed[X+1][Y]==false&&maze.getSquareValue(X+1,Y)!='%')
			{
				res.add(new State(new Square(X+1,Y),this,this.gValue+1,this.depth+1,2));// down unvisited
			}
		}
		if(Y>0) // has left
		{
			if(closed[X][Y-1]==false&&maze.getSquareValue(X,Y-1)!='%')
			{
				res.add(new State(new Square(X,Y-1),this,this.gValue+1,this.depth+1,3));// left unvisited
			}
		}
		return res;
		// TODO return all unvisited neighbors

		// TODO remember that each successor's depth and gValue are
		// +1 of this object.
		//return null;
	}

	/**
	 * @return x coordinate of the current state
	 */
	public int getX() {
		return square.X;
	}

	/**
	 * @return y coordinate of the current state
	 */
	public int getY() {
		return square.Y;
	}

	/**
	 * @param maze initial maze
	 * @return true is the current state is a goal state
	 */
	public boolean isGoal(Maze maze) {
		if (square.X == maze.getGoalSquare().X
				&& square.Y == maze.getGoalSquare().Y)
			return true;

		return false;
	}

	/**
	 * @return the current state's square representation
	 */
	public Square getSquare() {
		return square;
	}

	/**
	 * @return parent of the current state
	 */
	public State getParent() {
		return parent;
	}

	/**
	 * You may not need g() value in the DFS but you will need it in A-star
	 * search.
	 * 
	 * @return g() value of the current state
	 */
	public int getGValue() 
	{
		return gValue;
	}

	/**
	 * @return depth of the state (node)
	 */
	public int getDepth() {
		return depth;
	}
 
	/**
         * @return direction of the state (node)
         */
	public int getDirection()
	{
	  return direction;
	}
}
