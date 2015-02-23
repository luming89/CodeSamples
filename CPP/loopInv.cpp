//===--------------- loopInv.cpp - Project 3 for CS 701 ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a skeleton of an implementation for the loopInv
// pass of Univ. Wisconsin-Madison's CS 701 Project 3.
//
//===----------------------------------------------------------------------===//
//
// Author: Luming Zhang
// CS Login: luming
// Version: 1.0
//===----------------------------------------------------------------------===//

#include "flags.h"
#ifdef PRINTDOM 
    static const bool PRINT_DOM = true;
#else
    static const bool PRINT_DOM = false;
#endif
#ifdef PRINTMERGE
    static const bool PRINT_MERGE = true;
#else
    static const bool PRINT_MERGE = false;
#endif
#ifdef PRINTLOOPS
    static const bool PRINT_LOOPS = true;
#else
    static const bool PRINT_LOOPS = false;
#endif
#ifdef PRINTPRE
    static const bool PRINT_PRE = true;
#else
    static const bool PRINT_PRE = false;
#endif
#ifdef PRINTMOVING
    static const bool PRINT_MOVING = true;
#else
    static const bool PRINT_MOVING = false;
#endif

#define DEBUG 0
#define DEBUG_TYPE "loopInv"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CFG.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
//#include <unordered_set>
#include <queue>
#include <utility>
#include <stack>
//#include <list>
using namespace llvm;

STATISTIC(numLoopInvariant, "Number of loop-invariant instructions moved.");

namespace {
  
  template <typename T>
  std::set<T> union_set(std::set<T>& A,std::set<T>& B)
  {
	std::set<T> res;
        res.insert(A.begin(),A.end());
	res.insert(B.begin(),B.end());
	return res;
  }

  template <typename T>
  std::set<T> intersect_set(std::set<T>& A,std::set<T>& B)
  {
        std::set<T> res;
	std::set<T> union_res;
	
        union_res.insert(A.begin(),A.end());
        union_res.insert(B.begin(),B.end());
	for(typename std::set<T>::iterator i=union_res.begin();i!=union_res.end();i++)
	{
	  if(A.find(*i)!=A.end()&&B.find(*i)!=B.end())
	  {
	    res.insert(*i);
	  } 
	}
        return res;
  }

  typedef std::pair<BasicBlock*,BasicBlock*> backEdge;
  class CompareBlocksByName
  {
  public:
    bool operator ()(const BasicBlock* b1, const BasicBlock* b2)
    {
      if(b1->getName().str()<b2->getName().str())
        return true;
      else
        return false;
    }
  };
  class NaturalLoop//for easy use so all public
  {
  public:
    NaturalLoop(BasicBlock* h=NULL, BasicBlock* e=NULL): header(h), ender(e) {}
    BasicBlock* header;
    BasicBlock* ender;
    std::set<BasicBlock*,CompareBlocksByName> body;	
  };
  
  class CompareLoopsBySize
  {
  public:
    bool operator ()(const NaturalLoop* l1, const NaturalLoop* l2)
    {
	if(l1->body.size()>l2->body.size())
	  return true;
	else if(l1->body.size()<l2->body.size())
	  return false;
	else//equal size, then by header name
	{
	  if(l1->header->getName().str()<l2->header->getName().str())
            return true;
      	  else
            return false;
	}
    }
  };

  class CompareLoopsByHeader
  {
  public:
    bool operator ()(const NaturalLoop* l1, const NaturalLoop* l2)
    {
        if(l1->header<l2->header)
          return true;
        else if(l1->header>l2->header)
          return false;
	else
	{
	  if(l1->body.size()<l2->body.size())
            return true;
          else
            return false;
	}
    }
  };

  class PendingMoving
  {
    public:
	PendingMoving(Instruction* k, BasicBlock* b): insn(k), preheader(b)
	{}
        Instruction* insn;
        BasicBlock* preheader;
  };

  class loopInv : public FunctionPass {
  private:
    std::map<Instruction*,int> inst_map;
    std::map<BasicBlock*,std::set<BasicBlock*>*> dom_map;
    std::map<BasicBlock*,std::set<BasicBlock*>*> dom_map_before;
    std::queue<BasicBlock*> worklist_bb;
    std::set<backEdge> back_edges;
    std::multiset<NaturalLoop*,CompareLoopsBySize> natural_loops;
    std::set<NaturalLoop*,CompareLoopsBySize> merged_loops;
    std::queue<PendingMoving*> pendingMovings;
  public:
    static char ID; // Pass identification, replacement for typeid
    loopInv() : FunctionPass(ID) {}
    
    //**********************************************************************
    // runOnFunction
    //**********************************************************************
    virtual bool runOnFunction(Function &F)
    {
      setInstNum(F);
      domAnalysis(F);
      if(PRINT_DOM)
	printDom(F);
      findBackedge(F);
      findNaturalLoops();
      if(natural_loops.size())
        mergeLoops();
      if(PRINT_LOOPS)
	printLoops(F);
      moveLoopInvInsn();
      while(pendingMovings.empty()==false)
      {
        PendingMoving* mov = pendingMovings.front();
        pendingMovings.pop();
        mov->insn->removeFromParent();
        Instruction* term=mov->preheader->getTerminator();
        mov->preheader->getInstList().insert(term,mov->insn);
      }



      back_edges.clear();
      natural_loops.clear();
      merged_loops.clear();
      return false;
    }


    //**********************************************************************
    // print (do not change this method)
    //
    // If this pass is run with -f -analyze, this method will be called
    // after each call to runOnFunction.
    //**********************************************************************
    virtual void print(std::ostream &O, const Module *M) const {
        O << "This is loopInv.\n";
    }


    virtual void setInstNum(Function &F)
    {
      static int count = 1;
      for(Function::iterator i=F.begin();i!=F.end();i++)
      {
        for(BasicBlock::iterator j=i->begin();j!=i->end();j++)
        {
	    this->inst_map[j]=count;
            count++;
        }
      }
    }

    virtual void domAnalysis(Function &F)
    {
      std::set<BasicBlock*> all_nodes;
      #if DEBUG
	std::cerr << "\n";
        std::cerr << "FUNCTION " << F.getName().str() << "\n";
      #endif

      for(Function::iterator i=F.begin();i!=F.end();i++)
      {
	all_nodes.insert(i);
      }

      Function::iterator i=F.begin();
      dom_map[i]=new std::set<BasicBlock*>();
      dom_map[i]->insert(i);//dom(N0) = {N0} where N0 is the start node
      dom_map_before[i]=new std::set<BasicBlock*>();
      i++;
      while(i!=F.end())
      {
	//i++;
	dom_map[i]=new std::set<BasicBlock*>(all_nodes);//or all nodes but N0, initially set dom(N) = {all nodes}
	dom_map_before[i]=new std::set<BasicBlock*>(all_nodes);
	worklist_bb.push(i);//Push each node but N0 onto a worklist
	i++;     
      }

      while(!worklist_bb.empty())
      {
	#if DEBUG
	  std::cerr<<"There are "<<worklist_bb.size()<<" elements in worklist\n";
	#endif
	std::set<BasicBlock*> new_dom;
	std::set<BasicBlock*> new_dom_before;
	BasicBlock* cur_bb=worklist_bb.front();
	worklist_bb.pop();
	std::set<BasicBlock*> old_dom=*dom_map[cur_bb];
	//compute new value for cur_bb
	
	for (pred_iterator PI = pred_begin(cur_bb), E = pred_end(cur_bb); PI != E; ++PI)
        {
	  if(PI==pred_begin(cur_bb))
	  {
	    new_dom=*dom_map[*PI];
	  }
          BasicBlock* Pred = *PI;
	  new_dom=intersect_set(new_dom,*dom_map[*PI]);
	  new_dom_before=new_dom;
	  new_dom.insert(cur_bb);  
	}

	if(new_dom!=old_dom)
	{
	  #if DEBUG
	    std::cerr<<"New dominant set has "<<new_dom.size()<<" elements, the old one has "<<old_dom.size()<<" elements\n";
	    std::cerr<<"Dominant set changed!\n";
	  #endif
	  dom_map[cur_bb]=new std::set<BasicBlock*>(new_dom);
	  dom_map_before[cur_bb]=new std::set<BasicBlock*>(new_dom_before);
	  for(succ_iterator SI=succ_begin(cur_bb);SI!=succ_end(cur_bb);SI++)
          {
            BasicBlock* it=*SI;
	    worklist_bb.push(it);
	  }
	}
      }
    }

    void printDom(Function& F)
    {
      std::cerr << "\n";
      std::cerr << "FUNCTION " << F.getName().str() << "\n";
      for(Function::iterator it=F.begin();it!=F.end();it++)
      {
	std::set<std::string> tmp_set_string;
        std::cerr<< "BASIC BLOCK " << it->getName().str()<<"  DOM-Before: { ";
	std::set<BasicBlock*> tmp_set=*dom_map_before[it];
	for(std::set<BasicBlock*>::iterator i=tmp_set.begin();i!=tmp_set.end();i++)
	{
	  tmp_set_string.insert((*i)->getName().str());
	}
	for(std::set<std::string>::iterator i=tmp_set_string.begin();i!=tmp_set_string.end();i++)
        {
          std::cerr<<*i<<" ";
        }
	std::cerr << "}  DOM-After: { ";
	tmp_set_string.clear();
	tmp_set=*dom_map[it];
        for(std::set<BasicBlock*>::iterator i=tmp_set.begin();i!=tmp_set.end();i++)
        {
          tmp_set_string.insert((*i)->getName().str());
        }
	for(std::set<std::string>::iterator i=tmp_set_string.begin();i!=tmp_set_string.end();i++)
        {
          std::cerr<<*i<<" ";
        }
	std::cerr << "}\n";
      }
    }

    virtual void findBackedge(Function& F)
    {
      std::set<BasicBlock*> visited;
      for(Function::iterator i=F.begin();i!=F.end();i++)
      {
	#if DEBUG
	  std::cerr<<"Going to find backedge for header: "<<i->getName().str()<<"\n";
	#endif
	visited.clear();
	visited.insert(i);
	DFS(i,i,visited);
      }
    }

    virtual void DFS(BasicBlock* bb, BasicBlock* h, std::set<BasicBlock*>& v)
    {
      for (succ_iterator SI = succ_begin(bb), E = succ_end(bb); SI != E; ++SI)
      {
	BasicBlock* Succ = *SI;
	if(v.find(Succ)==v.end())//didn't visit
	{
	  v.insert(Succ);
	  DFS(Succ,h,v);
	}
	else//visited
	{
          if((Succ==h)&&(dom_map_before[bb]->find(Succ)!=dom_map_before[bb]->end()))
          {
 	    #if DEBUG
	      std::cerr<<"Back edge found from "<<bb->getName().str()<<" to "<<Succ->getName().str()<<"\n";
	    #endif
 	    back_edges.insert(std::make_pair(Succ,bb));
	  }
	} 
      }
      #if DEBUG
	std::cerr<<back_edges.size()<<" back edges found\n";
      #endif
    }

    virtual void findNaturalLoops()
    {
      #if DEBUG
	std::cerr<<back_edges.size()<<" back edges found\n";
	std::cerr<<"Going to find natual loops\n";
      #endif
      for(std::set<backEdge>::iterator i=back_edges.begin();i!=back_edges.end();i++)
      {
	NaturalLoop* tmp_nl=new NaturalLoop((*i).first,(*i).second);
	#if DEBUG
	  std::cerr<<"Ender is "<<tmp_nl->ender->getName().str()<<"\n";
	#endif
	std::stack<BasicBlock*> st; 
	st.push(tmp_nl->ender);	//push N onto an empty stack;
	while (!st.empty()) 
	{
	  BasicBlock* D=st.top();
	  st.pop();//pop D from the stack;
	  if (tmp_nl->body.find(D)==tmp_nl->body.end())//if(D not in body) 
	  {
	    tmp_nl->body.insert(D);//body = {D} union body;
	    for (pred_iterator PI = pred_begin(D), E = pred_end(D); PI != E; ++PI)//push each predecessor of D onto the stack.
	    {
	      if(*PI!=tmp_nl->header) st.push(*PI);//if not the header, push
	    }
	  }
	}
	natural_loops.insert(tmp_nl);	
      }
      #if DEBUG
        std::cerr<<natural_loops.size()<<" natural_loops found\n";
      #endif
    }

    virtual void mergeLoops()
    {
      std::multiset<NaturalLoop*,CompareLoopsByHeader> loops_before;
      NaturalLoop* nl_tmp=new NaturalLoop();
      #if DEBUG
	std::cerr<<"Reached mergeLoops()\n";
      #endif
      int count=0;
      BasicBlock* header;
      for(std::set<NaturalLoop*>::iterator i=natural_loops.begin();i!=natural_loops.end();i++)
      {
	loops_before.insert(*i);
      }
      std::set<NaturalLoop*>::iterator it_tmp=loops_before.begin();
      nl_tmp->header=(*it_tmp)->header;//initialize
      header=(*it_tmp)->header;
      for(std::set<NaturalLoop*>::iterator i=loops_before.begin();i!=loops_before.end();i++)
      {
	#if DEBUG
	  std::cerr<<"Size of natural_loops: "<<natural_loops.size()<<"\n";
	  std::cerr<<"Size of loops_before: "<<loops_before.size()<<"\n";
	  std::cerr<<"Processing loop: "<<(*i)->header->getName().str()<<"\n";
	#endif
	if((*i)->header==header)//should be merged
	{
	  #if DEBUG
	    std::cerr<<"Equals "<<header->getName().str()<<"\n";
	  #endif
	  for(std::set<BasicBlock*,CompareBlocksByName>::iterator j=(*i)->body.begin();j!=(*i)->body.end();j++)
          {
	    nl_tmp->body.insert(*j);
	 
  	  }
	  count++;
	}
	else//different, should not be merged
	{
	  if(PRINT_MERGE&&count>1)
	    std::cerr<<"merging "<<count<<" loops with header "<<header->getName().str();
	  merged_loops.insert(nl_tmp);
	  header=(*i)->header;
	  count=1;
	  nl_tmp=new NaturalLoop((*i)->header);
	  for(std::set<BasicBlock*,CompareBlocksByName>::iterator j=(*i)->body.begin();j!=(*i)->body.end();j++)
          {
            nl_tmp->body.insert(*j);

          }
	}
      }
      if(count)
      {
	merged_loops.insert(nl_tmp);
      }
      if(PRINT_MERGE&&count>1)
      {
        std::cerr<<"merging "<<count<<" loops with header "<<header->getName().str();
      }

    }

    virtual void printLoops(Function& F)
    {
      std::cerr << "\n";
      std::cerr << "FUNCTION " << F.getName().str() << "\n";
      std::cerr << "LOOPS\n";
      for(std::set<NaturalLoop*>::iterator i=merged_loops.begin();i!=merged_loops.end();i++)
      {
	std::cerr<<"Head: "<<(*i)->header->getName().str()<<" Body: { ";
	for(std::set<BasicBlock*>::iterator j=(*i)->body.begin();j!=(*i)->body.end();j++)
	{
	  std::cerr<<(*j)->getName().str()<<" ";
	}
	std::cerr<<"}\n";
      }
    }

    virtual void moveLoopInvInsn()
    {
      for(std::set<NaturalLoop*>::iterator i=merged_loops.begin();i!=merged_loops.end();i++)//loop level
      {
	std::set<Instruction*> loop_body;
	for(BasicBlock::iterator j=(*i)->header->begin();j!=(*i)->header->end();j++)
        {
	  loop_body.insert(j);
	}

	for(std::set<BasicBlock*,CompareBlocksByName>::iterator j=(*i)->body.begin();j!=(*i)->body.end();j++)
        {
          for(BasicBlock::iterator k=(*j)->begin();k!=(*j)->end();k++)
          {
	    loop_body.insert(k);
	  }
	}

	for(BasicBlock::iterator k=(*i)->header->begin();k!=(*i)->header->end();)//k++)//check invariant in header
        {
            bool inv_flag=checkMovable(k,loop_body);
	    if(inv_flag)//found loop invariant insn, move it
            {
              loop_body.erase(k);
              //BasicBlock* header=(*i)->header;
              moveInsn(k++,*i);
            }
	    else
	    {
	      k++;
	    }

        }

	for(std::set<BasicBlock*,CompareBlocksByName>::iterator j=(*i)->body.begin();j!=(*i)->body.end();j++)//body level
	{
	  for(BasicBlock::iterator k=(*j)->begin();k!=(*j)->end();)//k++)//block level
          {
	    bool inv_flag=checkMovable(k,loop_body);
	    if(inv_flag)//found loop invariant insn, move it
	    {
	      loop_body.erase(k);
	      //BasicBlock* header=(*i)->header;
	      moveInsn(k++,*i);
	    }
	    else
	    {
	      k++;
	    }
          }
	}
      }
    }

    virtual bool checkMovable(Instruction* k, const std::set<Instruction*>& loop_body)
    {
      bool res=true;
      #if DEBUG
        std::cerr<<"Going to process insn %"<<inst_map[k]<<"\n";
      #endif

      if((k->getOpcode()<14||k->getOpcode()>19)&&(k->getOpcode()<26||k->getOpcode()>32)&&(k->isTerminator()==false)&&k->getOpcode()!=Instruction::Call&&k->getOpcode()!=Instruction::Invoke&&k->getOpcode()!=Instruction::PHI)
      {
        #if DEBUG
	  std::cerr<<"insn %"<<inst_map[k]<<" type qualifies\n";
        #endif 
        for(unsigned l=0;l<k->getNumOperands();l++)
        {
  	  #if DEBUG
             std::cerr<<"Process insn %"<<inst_map[k]<<" "<<l<<"th operand \n";
          #endif
          Value* tmp_value;
          tmp_value=k->getOperand(l);
          if(isa<Instruction>(tmp_value))
          {
	    #if DEBUG
              std::cerr<<l<<"th operand is a pesudo register\n";
            #endif
	    if(loop_body.find((Instruction*)tmp_value)!=loop_body.end())
	    {
	      res=false;
	      break;
	     }
	  }
	  else//not an instruction operand
	  {
		  /*#if DEBUG
                    std::cerr<<l<<"th operand is NOT a pesudo register\n";
                  #endif
		  if(isa<Constant>(tmp_value)==false)
		  {
		    #if DEBUG
                       std::cerr<<l<<"th operand is NOT a literal(constant)\n";
                    #endif
		    inv_flag=false;
		    break;
		  }*/
		  
	  }
        }
      }
      else//operation type does not qualify 
      {
	res=false;
      }

      if(PRINT_MOVING&&res)
      {
        std::cerr<<"moving instruction %"<<inst_map[k]<<"\n";
        numLoopInvariant++;
      }
      return res;
    }

    virtual void moveInsn(Instruction* k, NaturalLoop* i)
    {
	BasicBlock* header=i->header;
	std::vector<BasicBlock*> preheaders; 
	unsigned int numPreheader=0;
	for(pred_iterator PI = pred_begin(i->header), E = pred_end(i->header); PI != E; ++PI)
	{
	  #if DEBUG
	    std::cerr<<"Predecessor is "<<(*PI)->getName().str()<<"\n";
	  #endif
	  if(i->body.find(*PI)==i->body.end())
	  {
	    preheaders.push_back(*PI);
	    numPreheader++;	
	  }
	}
	#if DEBUG
	  std::cerr<<header->getName().str()<<" has "<<numPreheader<<" preheaders\n";
	#endif
	if(numPreheader==1)//move directly
	{
	  Instruction* term=preheaders[0]->getTerminator();
	  k->removeFromParent();
	  preheaders[0]->getInstList().insert(term,k);
	}
	else//need split
	{
	  if(PRINT_PRE)
	  {
	    std::cerr<<"adding preheader for loop with header "<<header->getName().str()<<"\n";
	  }
	  BasicBlock* new_preheader=SplitBlockPredecessors(header, ArrayRef<BasicBlock*>(preheaders),"Suffix",NULL);
          k->removeFromParent();
	  Instruction* term=preheaders[0]->getTerminator();
	  new_preheader->getInstList().insert(term,k);
	 }	
    }
    //**********************************************************************
    // getAnalysisUsage
    //**********************************************************************

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      //AU.setPreservesAll();
    };

  };
  char loopInv::ID = 0;

  // register the loopInv class: 
  //  - give it a command-line argument (loopInv)
  //  - a name ("Loop Invariant")
  //  - a flag saying that if we modify the CFG
  //  - a flag saying this if this is an analysis pass
  RegisterPass<loopInv> X("loopInv", "Loop Invariant",
			   true, true);
}
