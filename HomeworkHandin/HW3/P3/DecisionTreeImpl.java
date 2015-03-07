import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.Queue;
import java.util.LinkedList;

/**
 * Fill in the implementation details of the class DecisionTree using this file.
 * Any methods or secondary classes that you want are fine but we will only
 * interact with those methods in the DecisionTree framework.
 * 
 * You must add code for the 5 methods specified below.
 * 
 * See DecisionTree for a description of default methods.
 */
public class DecisionTreeImpl extends DecisionTree {
	private static final boolean DEBUG = false;//true;//
	private DecTreeNode root;
	private List<String> labels; // ordered list of class labels
	private List<String> attributes; // ordered list of attributes
	private Map<String, List<String>> attributeValues; // map to ordered
														// discrete values taken
														// by attributes
	private Map<String, Integer> attributeOrders;

	/**
	 * Answers static questions about decision trees.
	 */
	DecisionTreeImpl() 
	{
		// no code necessary
		// this is void purposefully
	}

	/**
	 * Build a decision tree given only a training set.
	 * 
	 * @param train: the training set
	 */
	DecisionTreeImpl(DataSet train) {

		this.labels = train.labels;
		this.attributes = train.attributes;
		this.attributeValues = train.attributeValues;
		this.attributeOrders = new HashMap<String,Integer>();
		if(DEBUG)
		{
			for(int i = 0;i<labels.size();i++)
				System.out.println("Label "+i+" is "+labels.get(i));
			System.out.println("There are "+attributes.size()+" attributes in total");
			System.out.println("There are "+train.instances.size()+" instances in total");
		}
		for(int i = 0;i<attributes.size();i++)
		{
			attributeOrders.put(attributes.get(i),i);
		}
		root = buildTree(train.instances, attributes, -1, attributeValues, 0);
	}

	DecTreeNode buildTree(List<Instance> instances, List<String> attributes, int parentAttributeValue, Map<String, List<String>> attributeValues, int defaultValue)
	{
		DecTreeNode tree = null;
		if(DEBUG) 
		{
			System.out.println("*************************BuildTree begins***********************");
			System.out.println("Number of instances: "+instances.size());
		}
		if(instances.size()==0) // There is no instance
		{
			if(DEBUG) 
				System.out.println("There is no instances, set label to 0");
			tree = new DecTreeNode(defaultValue,-1,parentAttributeValue,true);
		}
		else // There is instance(s)
		{
			boolean same = true;
			int prevLabel= instances.get(0).label;
			for(int i = 1;i<instances.size();i++)
			{
				if(instances.get(i).label!=prevLabel)
				{
					same = false;
					break;
				}
			}
			if(same)
			{
				if(DEBUG) System.out.println("All instances have the same label "+prevLabel);
				tree = new DecTreeNode(prevLabel,-1,parentAttributeValue,true);
			}
			else // labels are not the same
			{
				if(DEBUG)
					System.out.println("DEBUG::: Labels are not the same");
				if(attributes.size()==0)
				{
					if(DEBUG)
						System.out.println("DEBUG::: No attributes left, use majority vote");
					tree = new DecTreeNode(majorityVote(instances),-1,parentAttributeValue,true);
				}
				else
				{
					if(DEBUG)
						System.out.println("DEBUG::: Still has "+attributes.size()+" attributes, choose best one");
					int q = bestAttribute(instances, attributes, attributeValues);

					tree = new DecTreeNode(majorityVote(instances),attributeOrders.get(attributes.get(q)),parentAttributeValue,false);
					List<String> attributesMinusQ = new ArrayList<String>(attributes);
					attributesMinusQ.remove(q);
					
					for(int i = 0;i < attributeValues.get(attributes.get(q)).size();i++)
					{

						if(DEBUG) 
						{
							System.out.println("Going to build subtree with attribute "+attributes.get(q)+", value "+attributeValues.get(attributes.get(q)).get(i));
							System.out.println("Size of attributesMinusQ is "+attributesMinusQ.size());
						}
						List<Instance> v_ex = new ArrayList<Instance>();
						int order = attributeOrders.get(attributes.get(q));
						for(int j = 0; j<instances.size();j++)
						{
							if(instances.get(j).attributes.get(order)==i)
								v_ex.add(instances.get(j));
						}
						DecTreeNode subtree = buildTree(v_ex,attributesMinusQ,i,attributeValues,majorityVote(instances));
						tree.addChild(subtree);
					}
				}
				
			}
		}
		return tree;
	}

	int majorityVote(List<Instance> instances)
	{
		if(DEBUG)
			System.out.println("In majorityVote");
		int[] counters = new int[labels.size()];
		int maxIndex = 0;
		int maxCount = 0;;
		for(int i=0;i<instances.size();i++)
			counters[instances.get(i).label]++;

		for(int i = 0;i<labels.size();i++)
		{
			if(counters[i]>maxCount)
			{
				maxIndex = i;
				maxCount = counters[i];
			}	
		}
		if(DEBUG) 
		{
			System.out.println("counters[0] is "+counters[0]);
			System.out.println("counters[1] is "+counters[1]);
			System.out.println("Result from majorityVote is "+maxIndex);
		}
		return maxIndex;
	}

	int bestAttribute(List<Instance> instances, List<String> attributes, Map<String, List<String>> attributeValues)
	{
		int numInstances = instances.size();

		double originalEntropy = 0;
		int[] counters = new int[labels.size()];
		for(int i = 0;i<instances.size();i++)
			counters[instances.get(i).label]++;
		for(int i = 0;i<labels.size();i++)
		{
			double probability = (counters[i]*1.0)/instances.size();
			originalEntropy-=probability*(Math.log(probability)/Math.log(2));
		}
		if(DEBUG)
		{
			System.out.println("OriginalEntropy is "+originalEntropy);
			System.out.println("Still have "+attributes.size()+" attributes");
		}

		int res=0;
		double max=0;
		for(int i = 0;i<attributes.size();i++) // try each atttribute
		{
			int order = attributeOrders.get(attributes.get(i));
			if(DEBUG) System.out.println("Try atttribute "+attributes.get(i));
			if(DEBUG) System.out.println("It's order is "+order);
			
			double localEntropy=0;
			if(DEBUG) 
			{
				System.out.println("Going to try "+order+"th attribute");
				System.out.println("There are "+attributeValues.get(attributes.get(i))+" values in this attribute");
			}
			int[][] labelCounters = new int[attributeValues.get(attributes.get(i)).size()][labels.size()];
			int[] attributeValueCounters = new int[attributeValues.get(attributes.get(i)).size()];
			for(int j = 0;j<instances.size();j++)
			{
				attributeValueCounters[instances.get(j).attributes.get(order)]++;
				labelCounters[instances.get(j).attributes.get(order)][instances.get(j).label]++;
			}
			for(int j = 0;j<attributeValues.get(attributes.get(i)).size();j++)
			{
				if(DEBUG) System.out.println("There are "+attributeValueCounters[j]+" instances with attribute "+order+" valued "+j);
				if(attributeValueCounters[j]>0)
				{
					for(int k =0;k<labels.size();k++)
					{
						if(DEBUG) System.out.println("Where "+labelCounters[j][k]+" instances with label "+k);
						double localProbability = (labelCounters[j][k]*1.0)/attributeValueCounters[j];
						if(localProbability>0)
							localEntropy-=(attributeValueCounters[j]*1.0/numInstances)*localProbability*(Math.log(localProbability)/Math.log(2));
					}
				}
					
			}
			if(DEBUG) 
				System.out.println("Information gain is "+(originalEntropy-localEntropy));
			if(originalEntropy-localEntropy>max)
			{
				max = originalEntropy-localEntropy;
				res = i; 
			}
		}
		if(DEBUG) 
			System.out.println("*****************************Best attribute is "+attributes.get(res));
		return res;
	}

	/**
	 * Build a decision tree given a training set then prune it using a tuning
	 * set.
	 * 
	 * @param train: the training set
	 * @param tune: the tuning set
	 */
	DecisionTreeImpl(DataSet train, DataSet tune) 
	{
		this.labels = train.labels;
		this.attributes = train.attributes;
		this.attributeValues = train.attributeValues;

		// TODO: add code here
		this.attributeOrders = new HashMap<String,Integer>();
		for(int i = 0;i<attributes.size();i++)
			attributeOrders.put(attributes.get(i),i);
		root = buildTree(train.instances, attributes, -1, attributeValues, 0);
		double tunningAccuracy = classifySet(tune.instances);
		if(DEBUG)
			System.out.println("Accuracy for tunning set is "+tunningAccuracy);
		// prune 
		double bestAccuracy = 0;
		DecTreeNode bestNode = null;
		int pruneLabel = 0;
		int labelChangeTo=0;
		Queue<DecTreeNode> qNode = new LinkedList<DecTreeNode>();
		Queue<List<Instance>> qInstances = new LinkedList<List<Instance>>();
		qNode.add(root);
		qInstances.add(train.instances);
		while(qNode.peek()!=null)	
		{
			DecTreeNode curr = qNode.poll();
			List<Instance> currSet = qInstances.poll();
			int majorityLabel = majorityVote(currSet);
			int originalLabel = curr.label;
			boolean originalTerminal = curr.terminal;
			curr.label = majorityLabel;
			curr.terminal = true;
			double localAccuracy = classifySet(tune.instances);
			if(DEBUG)
				System.out.println("localAccuracy is "+localAccuracy);
			curr.label = originalLabel;
			curr.terminal = originalTerminal;
			if(localAccuracy>bestAccuracy)
			{
				if(DEBUG)
					System.out.println("****************bestAccuracy update to "+localAccuracy);
				bestAccuracy = localAccuracy;
				bestNode = curr;
				pruneLabel = majorityLabel;
			}
			if(curr.children!=null&&curr.children.size()>0)
			{
				for(int i = 0;i<curr.children.size();i++) // attribute value?
				{
					List<Instance> v_ex = new ArrayList<Instance>();
					int attribute = curr.attribute;
					for(int j=0;j<currSet.size();j++)
					{
						if(currSet.get(j).attributes.get(attribute)==i)
							v_ex.add(currSet.get(j));
					}
					qNode.add(curr.children.get(i));
					qInstances.add(v_ex);
				}
					
			}

		}
		if(bestAccuracy>tunningAccuracy)
		{
			if(DEBUG)
				System.out.println("Found a better tree!");
			bestNode.label = pruneLabel;
			bestNode.terminal = true;
		}
		if(DEBUG)
			System.out.println("Tree building and prunning completed!");
	}

	public double classifySet(List<Instance> instances)
	{
		int numInstances = instances.size();
		int correctCounter = 0;
		if(DEBUG) 
			System.out.println("There are "+numInstances+" instances in classifySet");
		for(int i = 0;i<instances.size();i++)
		{
			int label=0;
			boolean done = false;
			DecTreeNode curr = root;
			while(done==false)
			{
				if(curr.terminal)
				{
					done = true;
					label = curr.label;
				}
				else
				{
					curr = curr.children.get(instances.get(i).attributes.get(curr.attribute));
				}
			}
			if(label==instances.get(i).label)
				correctCounter++;
		}
		return correctCounter*1.0/numInstances;
	}

	@Override
	public String classify(Instance instance) 
	{

		// TODO: add code here
		int res=0;
		boolean done = false;
		DecTreeNode curr = root;
		while(done==false)
		{
			if(curr.terminal)
			{
				done = true;
				res = curr.label;
			}
			else
			{
				if(DEBUG)
				{
					System.out.println("Check against attribute "+attributes.get(curr.attribute));
				}
				curr = curr.children.get(instance.attributes.get(curr.attribute));
			}
		}

		return labels.get(res);
	}

	@Override
	/**
	 * Print the decision tree in the specified format
	 */
	public void print() {

		printTreeNode(root, null, 0);
	}
	
	/**
	 * Prints the subtree of the node
	 * with each line prefixed by 4 * k spaces.
	 */
	public void printTreeNode(DecTreeNode p, DecTreeNode parent, int k) {
		StringBuilder sb = new StringBuilder();
		for (int i = 0; i < k; i++) {
			sb.append("    ");
		}
		String value;
		if (parent == null) {
			value = "ROOT";
		} else{
			String parentAttribute = attributes.get(parent.attribute);
			value = attributeValues.get(parentAttribute).get(p.parentAttributeValue);
		}
		sb.append(value);
		if (p.terminal) {
			sb.append(" (" + labels.get(p.label) + ")");
			System.out.println(sb.toString());
		} else {
			sb.append(" {" + attributes.get(p.attribute) + "?}");
			System.out.println(sb.toString());
			for(DecTreeNode child: p.children) {
				printTreeNode(child, p, k+1);
			}
		}
	}

	@Override
	public void rootInfoGain(DataSet train) 
	{
		this.labels = train.labels;
		this.attributes = train.attributes;
		this.attributeValues = train.attributeValues;
		// TODO: add code here
		int numInstances = train.instances.size();

		double originalEntropy = 0;
		int[] counters = new int[labels.size()];
		for(int i = 0;i<train.instances.size();i++)
			counters[train.instances.get(i).label]++;
		for(int i = 0;i<labels.size();i++)
		{
			double probability = (counters[i]*1.0)/train.instances.size();
			originalEntropy-=probability*(Math.log(probability)/Math.log(2));
		}
		if(DEBUG)
		{
			System.out.println("OriginalEntropy is "+originalEntropy);
			System.out.println("Still have "+attributes.size()+" attributes");
		}

		for(int i = 0;i<attributes.size();i++) // try each atttribute
		{
			double localEntropy=0;

			int[][] labelCounters = new int[attributeValues.get(attributes.get(i)).size()][labels.size()];
			int[] attributeValueCounters = new int[attributeValues.get(attributes.get(i)).size()];
			for(int j = 0;j<train.instances.size();j++)
			{
				attributeValueCounters[train.instances.get(j).attributes.get(i)]++;
				labelCounters[train.instances.get(j).attributes.get(i)][train.instances.get(j).label]++;
			}
			for(int j = 0;j<attributeValues.get(attributes.get(i)).size();j++)
			{
				if(attributeValueCounters[j]>0)
					for(int k =0;k<labels.size();k++)
					{
						if(DEBUG) System.out.println("Where "+labelCounters[j][k]+" instances with label "+k);
						double localProbability = (labelCounters[j][k]*1.0)/attributeValueCounters[j];
						if(localProbability>0)
							localEntropy-=(attributeValueCounters[j]*1.0/numInstances)*localProbability*(Math.log(localProbability)/Math.log(2));
					}
			}
			System.out.format("%s %.5f\n",attributes.get(i),originalEntropy-localEntropy);

		}
	}
}
