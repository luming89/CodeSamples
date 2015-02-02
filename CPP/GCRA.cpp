//===-- Gcra.cpp - Graph-coloring Register Allocator --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
// This file does Graph-coloring Register Allocation, for CS 701 Project 4.
//
//===--------------------------------------------------------------------===//
//
// Implementer: Luming Zhang
// CS login: luming
// Version: 2.0
//
//===--------------------------------------------------------------------===//

#define DEBUG_TYPE "gcra"
#include "flags.h"
#include <map>
#include "RDfact.h"
#include <stack>
#include <queue>
#include <iostream>
#include <iterator>
#include <string>
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#define MY_DEBUG 0
#define EXTRA_CREDIT 0
using namespace llvm;

typedef map<const MachineBasicBlock *, set<unsigned>*> BBtoRegMap;
typedef map<const MachineInstr *, set<unsigned>*> InstrToRegMap;
typedef map<const MachineBasicBlock *, set<RDfact *>*> BBtoRDfactMap;
typedef map<const MachineInstr *, set<RDfact *>*> InstrToRDfactMap;



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

  class CompareByReg;
  class LiveRange
  {
  
  public: LiveRange() : reg(0), stacked(false), color(0), hasCall(false), colored(false)
	  {};
	  unsigned reg;
	  MachineInstr *def;
	  //set<unsigned>* range;
	  set<unsigned> range;
	  set<MachineInstr*> rangeInstr;
	  //set<MachineInstr*> range;
	  set<MachineInstr*> calls;
	  //string name;
	  //set<unsigned> neighbors;
	  set<LiveRange*> neighbors;
	 
	  bool stacked;
	  bool hasCall;
	  bool colored;
	  unsigned color;
  };

  class CompareByReg
  {
  public:
    bool operator ()(const LiveRange* l1, const LiveRange* l2)
    {
      
      if(l1->reg<l2->reg) // if just compare this, different liveRange with same reg will be eliminated. Why?
        return true;
      else if(l1->reg>l2->reg)
        return false;
      else // reg equals
      {
	set<unsigned>::iterator it1=l1->range.begin();
	set<unsigned>::iterator it2=l2->range.begin();
	while(it1!=l1->range.end()&&it2!=l2->range.end())
	{
	  if(*it1<*it2)
	    return true;
	  if(*it1>*it2)
	    return false;
	  it1++;
	  it2++;
	}
	if(it1==l1->range.end())
	  return true;
	else
	  return false;
      }
    }
  };


  class Gcra : public MachineFunctionPass {
  private:
    static const bool DEBUG = false;//true;
    static const bool DEBUG_AVAIL = false; // print register classes


#ifdef PRINTLIVE
    static const bool PRINT_LIVE = true;
#else
    static const bool PRINT_LIVE = false;
#endif

#ifdef PRINTRD
    static const bool PRINT_RD = true;
#else
    static const bool PRINT_RD = false;
#endif

#ifdef PRINTRANGES
    static const bool PRINT_RANGES = true;
#else
    static const bool PRINT_RANGES = false;
#endif

#ifdef PRINTGRAPH
    static const bool PRINT_GRAPH = true;
#else
    static const bool PRINT_GRAPH = false;
#endif


    
    // reg class -> non-spill regs for that class
    map<int, std::set<unsigned>*> regClassToAvailPregSetMap;
    // reg class -> sprill regs for that class
    map<int, std::set<unsigned>*> regClassToSpillRegSetMap;

    set<RDfact *> RDfactSet;
    
    map<MachineInstr *, unsigned> InstrToNumMap;
    
    BBtoRegMap liveBeforeMap;
    BBtoRegMap liveAfterMap;
    BBtoRegMap liveVarsGenMap;
    BBtoRegMap liveVarsKillMap;
    InstrToRegMap insLiveBeforeMap;
    InstrToRegMap insLiveAfterMap;
    std::set<unsigned> *argPregSet;  // set of regs used to pass in args
    MachineBasicBlock *firstBB;      // first basic block in curr function

    BBtoRDfactMap RDbeforeMap;
    BBtoRDfactMap RDafterMap;
    BBtoRDfactMap RDgenMap;
    BBtoRDfactMap RDkillMap;
    InstrToRDfactMap insRDbeforeMap;
    InstrToRDfactMap insRDafterMap;

    set<LiveRange*,CompareByReg> initLiveRange;
    set<LiveRange*,CompareByReg> finalLiveRange;
    set<unsigned> usedPreg;

    MachineRegisterInfo *MRI;
    
    int numRegClasses;

  public:
    static char ID; // Pass identification, replacement for typeid
    
    //**********************************************************************
    // constructor
    //**********************************************************************
    Gcra() : MachineFunctionPass(ID) {
    }
    
    //**********************************************************************
    // runOnMachineFunction
    //
    //**********************************************************************
    bool runOnMachineFunction(MachineFunction &Fn) {
      if (DEBUG || PRINT_LIVE || PRINT_RD) {
	std::cerr << "START FUNCTION " << Fn.getFunction()->getName().str() << "\n";
      }

      // GET NUM REGISTER CLASSES
      getNumRegClasses(Fn);

      // INITIALIZE FOR EACH FN
      RDfactSet.clear();
      RDbeforeMap.clear();
      RDafterMap.clear();
      InstrToNumMap.clear();
      liveBeforeMap.clear();
      liveAfterMap.clear();
      liveVarsGenMap.clear();
      liveVarsKillMap.clear();
      insLiveBeforeMap.clear();
      insLiveAfterMap.clear();
      
      RDbeforeMap.clear();
      RDafterMap.clear();
      RDgenMap.clear();
      RDkillMap.clear();
      insRDbeforeMap.clear();
      insRDafterMap.clear();
      
      initLiveRange.clear();
      finalLiveRange.clear();      
      regClassToAvailPregSetMap.clear();
      regClassToSpillRegSetMap.clear();

      //usedPreg.clear(); // seems do not need

      // STEP 1: get sets of regs, set of defs, set of RDfacts,
      //         instruction-to-number map
      if (! doInit(Fn)) 
      {
	// no virtual registers in this function -- not much to do
	MRI->clearVirtRegs();
	return 0;
      }

      if (DEBUG) {
	printInstructions(Fn);
      }

      // STEP 2: live analysis for all registers (fill in globals
      //         liveBeforeMap and liveAfterMap for blocks, and
      //         globals insLiveBeforeMap and insLiveAfterMapfor
      //         instructions)
      if (DEBUG) {
	std::cerr << "START LIVE ANALYSIS\n";
      }
      doLiveAnalysis(Fn);
      if (PRINT_LIVE) {
	printLiveResults(Fn);
      }
      
      // STEP 2(a): add an RDfact to (global) RDfactSet for each
      //            preg used to pass an arg to this fn;
      //            must do this after live anal, since that's
      //            where we compute argPregSet
      addArgPregsToRDfactSet();
      
      // STEP 3: reaching defs analysis (fill in globals RDbeforeMap and
      //         RDafterMap for blocks, and globals insRDbeforeMap and
      //         insRDafterMap for instructions)
      if (DEBUG) {
	std::cerr << "\nSTART REACHING DEFS ANALYSIS\n";
      }
      doReachingDefsAnalysis(Fn);
      if (PRINT_RD) {
	printRDResults(Fn);
      }
     
      computeLiveRange(Fn);
      mergeLiveRange();
      if(PRINT_RANGES)
      {
	printRanges(initLiveRange,false,Fn); // print initial live range, why skeleton pass Fn to print? To get name...
	printRanges(finalLiveRange,true,Fn); // print final live range
      }

      buildInterferenceGraph(Fn);

      if(PRINT_GRAPH)
	printInterferenceGraph(Fn);

      /*************** EXTRA CREDIT *****************/
      //#if EXTRA_CREDIT
	setSpillRegister(Fn); 
      //#endif     
      /**********************************************/
      colorInterferenceGraph(Fn);
      replaceVregWithPreg(Fn);
      /*************** EXTRA CREDIT *****************/
      //#if EXTRA_CREDIT
        handleUncolored(Fn);
      //#endif
      /**********************************************/
      handleFunctionCalls(Fn);

      //exit(0); // prevent coredump until reg alloc is implemented
      MRI->clearVirtRegs();
      return true;
    }
    
    virtual void computeLiveRange(MachineFunction& Fn) 
    {
	int debug_count=0;	
	// back to here
	set<LiveRange*> lookUp;
	stack<MachineBasicBlock *> worklist;  
	for (MachineFunction::iterator MFIt = Fn.begin(); MFIt != Fn.end(); MFIt++)
	worklist.push(MFIt);

	while(!worklist.empty())
	{
	  MachineBasicBlock* cur_mbb = worklist.top(); // BB level
	  worklist.pop();
#if MY_DEBUG==1
  //std::cerr << "DEBUG::: worklist current block is "<<cur_mbb->getName().str() << "\n";
#endif
          for (MachineBasicBlock::iterator MBBIt = cur_mbb->begin(); MBBIt!=cur_mbb->end(); MBBIt++) // iterate through instruction
	  {
	    set<RDfact*> reaching;
	    bool isCall = false;
            const MCInstrDesc &MCID = MBBIt->getDesc();
            if (MCID.isCall())
            {
#if MY_DEBUG==2
  cerr<<"Instruction %"<<InstrToNumMap[MBBIt]<<" is a call\n";
#endif
              isCall = true;
            }
	    for(set<RDfact *>::iterator IT = insRDbeforeMap[MBBIt]->begin(); IT != insRDbeforeMap[MBBIt]->end(); IT++) // insert each def into reaching;
	    {
		RDfact* oneRDfact = *IT;
		reaching.insert(oneRDfact);
	    }
	    // searching reaching's element in live set
	    for(set<RDfact*>::iterator it_reaching=reaching.begin();it_reaching!=reaching.end();it_reaching++)
	    {
	      if(insLiveBeforeMap[MBBIt]->find((*it_reaching)->getReg())!=insLiveBeforeMap[MBBIt]->end()) // found in live before 
	      {
#if MY_DEBUG==1
  //std::cerr<<"DEBUG::: LiveBefore: Def and alive "<<(*it_reaching)->getReg()<<"\n";
#endif
		// when found, check if this reg has an open live range, if so insert this instruction into live range, if not, create one then insert
		bool openLiveRange=false;
		for(set<LiveRange*>::iterator it_lookUp=lookUp.begin();it_lookUp!=lookUp.end();it_lookUp++)
		{
		  if((*it_lookUp)->reg==(*it_reaching)->getReg()&&(*it_lookUp)->def==(*it_reaching)->getInstr()) // found in openLiveRange
		  {
		    openLiveRange=true;
		    (*it_lookUp)->range.insert(InstrToNumMap[MBBIt]);
		    (*it_lookUp)->rangeInstr.insert(MBBIt);
		    if(isCall)
		    {
		      (*it_lookUp)->hasCall = isCall;
#if MY_DEBUG==2
  cerr<<"%"<<InstrToNumMap[MBBIt]<<" is a call and inserted into ";
  if(TargetRegisterInfo::isVirtualRegister((*it_lookUp)->reg))
    cerr<<"%"<<TargetRegisterInfo::virtReg2Index((*it_lookUp)->reg);
  else
    cerr<<"R"<<(*it_lookUp)->reg;
  cerr<<"'s live range\n";
#endif
		      (*it_lookUp)->calls.insert(MBBIt);
		    }
		    break;
		  }
		}
		if(openLiveRange==false) // didn't find in openLiveRange
		{
#if MY_DEBUG==1
  //std::cerr<<"DEBUG::: LiveBefore: Didn't find openLiveRange for "<<(*it_reaching)->getReg()<<", so create a new one\n";
#endif
		  LiveRange* newLiveRange = new LiveRange();
		  newLiveRange->reg=(*it_reaching)->getReg();
		  newLiveRange->range.insert(InstrToNumMap[MBBIt]);
		  newLiveRange->rangeInstr.insert(MBBIt);
		  newLiveRange->def = (*it_reaching)->getInstr();
		  if(isCall) 
		  {
		    newLiveRange->hasCall = isCall;
		    newLiveRange->calls.insert(MBBIt);
		  }
		  lookUp.insert(newLiveRange);
		} // openLiveRange find if		
	      }
	    }
	  
	  /***************** DEF AFTER AND LIVE AFTER *****************/
	    reaching.clear(); // def after and live after part
	    for(set<RDfact *>::iterator IT = insRDafterMap[MBBIt]->begin(); IT != insRDafterMap[MBBIt]->end(); IT++) // insert each def into reaching;
	    {
	      RDfact *oneRDfact = *IT;
              reaching.insert(oneRDfact);
	      if(oneRDfact->getInstr()==MBBIt)
              {
#if MY_DEBUG==1
  //std::cerr<<"Instr: "<<InstrToNumMap[oneRDfact->getInstr()]<<": Reg "<<oneRDfact->getReg()<<" should be added!\n";
#endif
		bool openLiveRange=false;
                for(set<LiveRange*>::iterator it_lookUp=lookUp.begin();it_lookUp!=lookUp.end();it_lookUp++)
                {
                  if((*it_lookUp)->reg==oneRDfact->getReg()&&(*it_lookUp)->def==oneRDfact->getInstr())
                  {
                    openLiveRange=true;
                    (*it_lookUp)->range.insert(InstrToNumMap[MBBIt]);
		    (*it_lookUp)->rangeInstr.insert(MBBIt);
		    if(isCall)
		    {
		      (*it_lookUp)->hasCall = isCall;
		      (*it_lookUp)->calls.insert(MBBIt);
		    }
                    break;
                  }
                }
                if(openLiveRange==false)
                {
#if MY_DEBUG==1
  //std::cerr<<"DEBUG::: LiveAfter: Didn't find openLiveRange for "<<oneRDfact->getReg()<<", so create a new one\n";
#endif
                  LiveRange* newLiveRange = new LiveRange();
                  newLiveRange->reg=oneRDfact->getReg();
                  newLiveRange->range.insert(InstrToNumMap[MBBIt]);
		  newLiveRange->rangeInstr.insert(MBBIt);
		  newLiveRange->def = oneRDfact->getInstr();
		  if(isCall) 
		  {
		    newLiveRange->hasCall = isCall;
		    newLiveRange->calls.insert(MBBIt);
		  }
                  lookUp.insert(newLiveRange);
                }
              }
	    }
	    // searching reaching's element in live set
	    for(set<RDfact*>::iterator it_reaching=reaching.begin();it_reaching!=reaching.end();it_reaching++)
	    {
	      if(insLiveAfterMap[MBBIt]->find((*it_reaching)->getReg())!=insLiveAfterMap[MBBIt]->end()) // found
	      {
#if MY_DEBUG==1
  //std::cerr<<"DEBUG::: LiveAfter: Def and alive "<<(*it_reaching)->getReg()<<"\n";
#endif
		  // when found, check if this reg has an open live range, if so insert this instruction into live range, if not, create one then insert
	        bool openLiveRange=false;
	        for(set<LiveRange*>::iterator it_lookUp=lookUp.begin();it_lookUp!=lookUp.end();it_lookUp++)
	        {
		  if((*it_lookUp)->reg==(*it_reaching)->getReg()&&((*it_lookUp)->def==(*it_reaching)->getInstr()))
		  {
		    openLiveRange=true;
		    (*it_lookUp)->range.insert(InstrToNumMap[MBBIt]);
		    (*it_lookUp)->rangeInstr.insert(MBBIt);
		    if(isCall) 
		    {
		      (*it_lookUp)->hasCall = isCall;
		      (*it_lookUp)->calls.insert(MBBIt);
		    }
		    break;
		  }
	        }
	        if(openLiveRange==false)
	        {
#if MY_DEBUG==1
  //std::cerr<<"DEBUG::: LiveAfter: Didn't find openLiveRange for "<<*it_reaching<<", so create a new one\n";
#endif
		  LiveRange* newLiveRange = new LiveRange();
		  newLiveRange->reg=(*it_reaching)->getReg();
		  newLiveRange->range.insert(InstrToNumMap[MBBIt]);
		  newLiveRange->rangeInstr.insert(MBBIt);
		  newLiveRange->def = (*it_reaching)->getInstr();
		  if(isCall) 
		  {
		    newLiveRange->hasCall = isCall;
		    newLiveRange->calls.insert(MBBIt);
		  }
		  lookUp.insert(newLiveRange);
	        }		
	      }
	    } // it_reaching loop
	  }
	} // worklist while loop

	/** flush everything in open live range set into initLiveRange set **/
        for(set<LiveRange*>::iterator it_lookUp=lookUp.begin();it_lookUp!=lookUp.end();it_lookUp++)
        {
	  initLiveRange.insert(*it_lookUp);
	}
	lookUp.clear();
    }

    virtual void mergeLiveRange()
    {
      LiveRange* prev=*initLiveRange.begin();
      LiveRange* work=new LiveRange();
      work->reg = prev->reg;
      work->range.insert(prev->range.begin(),prev->range.end());
      work->rangeInstr.insert(prev->rangeInstr.begin(),prev->rangeInstr.end());
      work->hasCall = prev->hasCall;
      work->calls = prev->calls;
      for(set<LiveRange*>::iterator it=++initLiveRange.begin();it!=initLiveRange.end();it++)
      {
        if((*it)->reg==work->reg) // same reg, may need merge
	{
#if MY_DEBUG==1
  //cerr << "Same reg "<<work->reg<<", may need merge\n";
#endif
	  if(intersect_set<unsigned>(work->range,(*it)->range).size())
	  {
	    // merge
#if MY_DEBUG==1
  //cerr << "Same reg "<<work->reg<<", should be merged\n";
#endif
	    work->range = union_set<unsigned> (work->range,(*it)->range);
	    work->rangeInstr = union_set<MachineInstr*> (work->rangeInstr,(*it)->rangeInstr);
	    if((*it)->hasCall)
	    {
	      work->hasCall = true;
	      work->calls.insert((*it)->calls.begin(),(*it)->calls.end());
	    }
	  }
	  else // same reg, but dont need merge
	  {
	    // dont merge
	    finalLiveRange.insert(work);
	    work=new LiveRange();
	    work->reg = (*it)->reg;
            work->range.insert((*it)->range.begin(),(*it)->range.end());
	    work->rangeInstr.insert((*it)->rangeInstr.begin(),(*it)->rangeInstr.end());
	    work->hasCall = (*it)->hasCall;
	    work->calls = (*it)->calls;
	  }
	}
	else // different reg, put work directly into final
	{
	  finalLiveRange.insert(work);
          work=new LiveRange();
	  work->reg = (*it)->reg;
	  work->range.insert((*it)->range.begin(),(*it)->range.end());
	  work->rangeInstr.insert((*it)->rangeInstr.begin(),(*it)->rangeInstr.end());
	  work->hasCall = (*it)->hasCall;
	  work->calls = (*it)->calls;
	}
      } // initLiveRange loop
      finalLiveRange.insert(work);
    }


    virtual void printRanges(set<LiveRange*,CompareByReg>& liveRange, bool final_flag,MachineFunction& Fn)
    {
      if(final_flag)
	std::cerr<<"\nFINAL LIVE RANGES FOR FUNCTION "<<Fn.getName().str()<<"\n\n";
      else
	std::cerr<<"\nINITIAL LIVE RANGES FOR FUNCTION "<<Fn.getName().str()<<"\n\n";
      std::cerr<<"Physical Registers"<<"\n";
      for(set<LiveRange*>::iterator it_LR=liveRange.begin();it_LR!=liveRange.end();it_LR++)
      {
	LiveRange* tmp_LR = *it_LR;
	if (TargetRegisterInfo::isPhysicalRegister(tmp_LR->reg))
        {
          std::cerr << "R" << tmp_LR->reg;//<<": { ";
	  std::cerr<<": { ";
          for(set<unsigned>::iterator it_int=tmp_LR->range.begin();it_int!=tmp_LR->range.end();it_int++)
          {
            std::cerr << "%"<<*it_int <<" ";
          }
          std::cerr<<"} ";
#if MY_DEBUG==2
	  if(tmp_LR->hasCall)
	  {
	    cerr<<" This range for R"<<tmp_LR->reg<<" has call(s): { ";
	    for(set<MachineInstr*>::iterator it = tmp_LR->calls.begin();it!=tmp_LR->calls.end();it++)
	    {
	      cerr<<"%"<<InstrToNumMap[*it]<<" ";
	    }
	    cerr<<"} ";
	  }
#endif
	cerr<<"\n";
	}
      }
      std::cerr<<"\n";      

      // print virtual register
      std::cerr<<"Virtual Registers\n";
      for(set<LiveRange*>::iterator it_LR=liveRange.begin();it_LR!=liveRange.end();it_LR++)
      {
        LiveRange* tmp_LR = *it_LR;
        if (TargetRegisterInfo::isVirtualRegister(tmp_LR->reg))
        {
          std::cerr << "%" << TargetRegisterInfo::virtReg2Index(tmp_LR->reg);//<<": { ";
          std::cerr<<": { ";
          for(set<unsigned>::iterator it_int=tmp_LR->range.begin();it_int!=tmp_LR->range.end();it_int++)
          {
            std::cerr << "%"<<*it_int <<" ";
          }
          std::cerr<<"} ";
#if MY_DEBUG==2
          if(tmp_LR->hasCall)
          {
            cerr<<" This range for %"<<TargetRegisterInfo::virtReg2Index(tmp_LR->reg) <<" has call(s): { ";
            for(set<MachineInstr*>::iterator it = tmp_LR->calls.begin();it!=tmp_LR->calls.end();it++)
            {
              cerr<<"%"<<InstrToNumMap[*it]<<" ";
            }
            cerr<<"} ";
          }
	  else
	  {
	    cerr<<"This range doesn't have call(s)";
	  }
#endif
	cerr<<"\n";
	}
      }
    }

    virtual void buildInterferenceGraph(MachineFunction& Fn)
    {
      for(set<LiveRange*>::iterator it_outer=finalLiveRange.begin();it_outer!=finalLiveRange.end();it_outer++)
      {
	for(set<LiveRange*>::iterator it_inner=next(it_outer,1);it_inner!=finalLiveRange.end();it_inner++)
	{
	  //cerr<<union_set<unsigned>((*it_outer)->range,(*it_inner)->range).size()<<"\n";
	  if(intersect_set<unsigned>((*it_outer)->range,(*it_inner)->range).size())
	  {
	    
#if MY_DEBUG==1
  //cerr<<(*it_outer)->reg<<" interferes with "<<(*it_inner)->reg<<"\n";
#endif
	    set<unsigned> availReg1;
	    set<unsigned> availReg2;

	    // calculate available register set for (*it_outer)->reg
	    if(TargetRegisterInfo::isPhysicalRegister((*it_outer)->reg)) // outer is a physical reg
	    {
#if MY_DEBUG==1
  //cerr<<"Outer "<<(*it_outer)->reg<<" is a preg\n";
#endif
	      availReg1.insert((*it_outer)->reg);
	      const TargetRegisterInfo* TRI = Fn.getTarget().getRegisterInfo();
              MCRegAliasIterator* it = new MCRegAliasIterator((*it_outer)->reg, TRI, false);
	      while (it->isValid()) 
	      {
  		// **it has type "unsigned"; i.e., it is a preg that is an alias of P
#if MY_DEBUG==1
  //cerr<<"Alias is "<<**it<<"\n";
#endif	
		availReg1.insert(**it);
  		++(*it);
	      }	
	      //delete it?
	    }
	    else // outer is a virtual reg
	    {
#if MY_DEBUG==1
  //cerr<<"Outer "<<(*it_outer)->reg<<" is a vreg\n";
#endif
	      MachineRegisterInfo *MRI = &Fn.getRegInfo();
	      int vregClass = MRI->getRegClass((*it_outer)->reg)->getID(); // what class is virtual reg V in
	      availReg1 = *(regClassToAvailPregSetMap[vregClass]);
	      for(set<unsigned>::iterator iter=availReg1.begin();iter!=availReg1.end();iter++)
	      {
		const TargetRegisterInfo* TRI = Fn.getTarget().getRegisterInfo();
                MCRegAliasIterator* it = new MCRegAliasIterator(*iter, TRI, false);
                while (it->isValid())
                {
                  // **it has type "unsigned"; i.e., it is a preg that is an alias of P
#if MY_DEBUG==1
  //cerr<<"Alias is "<<**it<<"\n";
#endif
                  availReg1.insert(**it);
                  ++(*it);
                }
	      }	      
	    }

	    // calculate available register set for (*it_inner)->reg
	    if(TargetRegisterInfo::isPhysicalRegister((*it_inner)->reg)) // inner is a physical reg
	    {
#if MY_DEBUG==1
  //cerr<<"Inner "<<(*it_inner)->reg<<" is a preg\n";
#endif
	      availReg2.insert((*it_inner)->reg);
	      const TargetRegisterInfo* TRI = Fn.getTarget().getRegisterInfo();
              MCRegAliasIterator* it = new MCRegAliasIterator((*it_inner)->reg, TRI, false);
	      while (it->isValid()) 
	      {
  		// **it has type "unsigned"; i.e., it is a preg that is an alias of P
#if MY_DEBUG==1
  //cerr<<"Alias is "<<**it<<"\n";
#endif	
		availReg2.insert(**it);
  		++(*it);
	      }	
	      //delete it?
	    }
	    else // inner is a virtual reg
	    {
#if MY_DEBUG==1
  //cerr<<"Inner "<<(*it_inner)->reg<<" is a vreg\n";
#endif
	      MachineRegisterInfo *MRI = &Fn.getRegInfo();
	      int vregClass = MRI->getRegClass((*it_inner)->reg)->getID(); // what class is virtual reg V in
#if MY_DEBUG==1
  //cerr<<(*it_inner)->reg<<" is in register class "<<vregClass<<"\n";
#endif
	      availReg2 = *(regClassToAvailPregSetMap[vregClass]);

	      for(set<unsigned>::iterator iter=availReg2.begin();iter!=availReg2.end();iter++)
              {
                const TargetRegisterInfo* TRI = Fn.getTarget().getRegisterInfo();
                MCRegAliasIterator* it = new MCRegAliasIterator(*iter, TRI, false);
                while (it->isValid())
                {
                  // **it has type "unsigned"; i.e., it is a preg that is an alias of P
#if MY_DEBUG==1
  //cerr<<"Alias is "<<**it<<"\n";
#endif
                  availReg2.insert(**it);
                  ++(*it);
                }
              }
	    }
#if MY_DEBUG==1
  //cerr<<"DEBUG::: Reg set for outer\n";
  //printRegSet(availReg1);
  //cerr<<"\nDEBUG::: Reg set for inner\n";
  //printRegSet(availReg2);
  //cerr<<"\n";
#endif
	    // check to see if available register set non-disjoint
	    if(intersect_set<unsigned>(availReg1,availReg2).size())
	    {
#if MY_DEBUG==1
  //cerr<<"Adding edge\n\n";
#endif
	      (*it_outer)->neighbors.insert(*it_inner);
	      (*it_inner)->neighbors.insert(*it_outer);
	    }
#if MY_DEBUG==1
else
  cerr<<"\n";
#endif
	  } // condition 1
	} // inner loop
      } // outer loop
    }
  
    virtual void printInterferenceGraph(MachineFunction& Fn)
    {
      std::cerr<<"\nINTERFERENCE GRAPH FOR FUNCTION "<<Fn.getName().str()<<"\n\n";
      std::cerr<<"Physical Registers"<<"\n";
      for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++)
      {
	if (TargetRegisterInfo::isPhysicalRegister((*it)->reg))
        {
          std::cerr << "R" << (*it)->reg<<"'s neighbors: { ";
	  set<LiveRange*, CompareByReg> tmp;
	  tmp.insert((*it)->neighbors.begin(),(*it)->neighbors.end());


	  for(set<LiveRange*>::iterator it_LR=tmp.begin();it_LR!=tmp.end();it_LR++)
          {
	    if (TargetRegisterInfo::isPhysicalRegister((*it_LR)->reg))
	    {
	      std::cerr<< "R" << (*it_LR)->reg<<" ";
	    }
          }
	  for(set<LiveRange*>::iterator it_LR=tmp.begin();it_LR!=tmp.end();it_LR++)
          {
            if (TargetRegisterInfo::isVirtualRegister((*it_LR)->reg))
            {
	      std::cerr<< "%" <<TargetRegisterInfo::virtReg2Index((*it_LR)->reg)<<" ";
            }
	  }

          std::cerr<<"}\n";
        }
      }
    
      std::cerr<<"\nVirtual Registers"<<"\n";
      for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++)
      {
        if (TargetRegisterInfo::isVirtualRegister((*it)->reg))
        {
          std::cerr << "%" << TargetRegisterInfo::virtReg2Index((*it)->reg)<<"'s neighbors: { ";
	  set<LiveRange*, CompareByReg> tmp;
          tmp.insert((*it)->neighbors.begin(),(*it)->neighbors.end());


          for(set<LiveRange*>::iterator it_LR=tmp.begin();it_LR!=tmp.end();it_LR++)
          {
            if (TargetRegisterInfo::isPhysicalRegister((*it_LR)->reg))
            {
              std::cerr<< "R" << (*it_LR)->reg<<" ";
            }
          }
          for(set<LiveRange*>::iterator it_LR=tmp.begin();it_LR!=tmp.end();it_LR++)
          {
            if (TargetRegisterInfo::isVirtualRegister((*it_LR)->reg))
            {
              std::cerr<< "%" <<TargetRegisterInfo::virtReg2Index((*it_LR)->reg)<<" ";
            }
          }

          std::cerr<<"}\n";
        }
      }
      std::cerr<<"\n";
    }

    virtual void setSpillRegister(MachineFunction& Fn)
    {
      #if EXTRA_CREDIT
	cerr<<"Take a look at usedPreg: \n";
	printRegSet(&usedPreg); // & just want to use the other overloaded printRegSet
	std::cerr<<"\nNum of register classes is "<<numRegClasses<<"\n";
      #endif
      for(int i = 0;i<numRegClasses;i++)
	regClassToSpillRegSetMap[i] = new std::set<unsigned>();
      //regClassToAvailPregSetMap;
      //map<int, std::set<unsigned>*> regClassToSpillRegSetMap;
      for(int i = 0;i<numRegClasses;i++) // find 3 spill register for every register class
      {
#if EXTRA_CREDIT
  cerr<<"Going to find spill reg for reg class "<<i<<"\n";
#endif
	if(regClassToSpillRegSetMap[i]->size()<3)
	{
	  for(std::set<unsigned>::iterator it = regClassToAvailPregSetMap[i]->begin(); it != regClassToAvailPregSetMap[i]->end();)// it++)
	  {
	    if(usedPreg.find(*it)!=usedPreg.end()) // find a one not used in this fn	    
	    {
	      it++;
	      continue;
	    }
	    unsigned spillReg = *it;
	    
#if EXTRA_CREDIT
  cerr<<"Going to insert R"<<spillReg<<" as spill reg to reg class "<<i<<"\n";
#endif
	    regClassToSpillRegSetMap[i]->insert(spillReg);
	    regClassToAvailPregSetMap[i]->erase(it++);
	    // remove spillReg from all the available preg set of register class
	    for(int j = 0;j<numRegClasses;j++)
	    {
	      if(regClassToAvailPregSetMap[j]->find(spillReg)!=regClassToAvailPregSetMap[j]->end())
	      { // find, remove spillReg 
	        regClassToAvailPregSetMap[j]->erase(spillReg);
		// and insert this spillReg to this reg class, this is correct? 
		regClassToSpillRegSetMap[j]->insert(spillReg);
	      }
	    }
	    /************** PROCESSING ALIASINGS OF THIS PREG **************/
	    const TargetRegisterInfo *TRI = Fn.getTarget().getRegisterInfo();
	    MCRegAliasIterator *it_alias = new MCRegAliasIterator(spillReg, TRI, false);
	    while (it_alias->isValid()) 
	    {// **it has type "unsigned"; i.e., it is a preg that is an alias of P
#if EXTRA_CREDIT
  cerr<<"Aliasings of R"<<spillReg<<" is R"<<**it_alias<<"\n";
#endif	    
	      for(int j = 0;j<numRegClasses;j++)
              {
                if(regClassToAvailPregSetMap[j]->find(**it_alias)!=regClassToAvailPregSetMap[j]->end())
                { // find, remove spillReg 
                  regClassToAvailPregSetMap[j]->erase(**it_alias);
		  // and insert this spillReg to this reg class, this is correct? 
                  regClassToSpillRegSetMap[j]->insert(**it_alias);
                }
              }

  	      ++(*it_alias);

	    }
	    /***************************************************************/
	    if(regClassToSpillRegSetMap[i]->size()>=3) // if get enough spill reg
	    break; 
	  } // available preg level
	}
#if EXTRA_CREDIT 
	cerr<<"Spill reg set for reg class: "<<i<<"\n";
	printRegSet(regClassToSpillRegSetMap[i]);
	cerr<<"Avail reg set for reg class: "<<i<<"\n";
	printRegSet(regClassToAvailPregSetMap[i]);
#endif
      } // each register class loop
    }

    virtual void colorInterferenceGraph(MachineFunction& Fn)
    {
      stack<LiveRange*> workStation1;
      //stack<LiveRange*> workStation2;
      set<unsigned> stacked;
      bool findEasy=false;
      bool haveVirtual=false;
      /******************** PUSH ********************/
      do
      {
	findEasy=false;
	haveVirtual=false;
	for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++)
	{
	  if((*it)->stacked==false&&TargetRegisterInfo::isVirtualRegister((*it)->reg)) // virtual and unstacked
	  { 
	      #if MY_DEBUG==1
	        cerr<<"%"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<" is a virtual reg\n";
	      #endif
	      haveVirtual = true;
	      // calculate # of unstacked neighbors
	      int numUnstackedNeighbors=0;
	      set<unsigned> availReg;
	      for(set<LiveRange*>::iterator it_LR=(*it)->neighbors.begin();it_LR!=(*it)->neighbors.end();it_LR++)
              {
	        if(stacked.find((*it_LR)->reg)==stacked.end())
		{  
		  numUnstackedNeighbors++;
	        }
	      }
	      // # of registers available for V's register class
	      int numAvailableRegisters=0;
	      MachineRegisterInfo *MRI = &Fn.getRegInfo();
              int vregClass = MRI->getRegClass((*it)->reg)->getID(); // what class is virtual reg V in
              availReg = *(regClassToAvailPregSetMap[vregClass]);
              /*for(set<unsigned>::iterator iter=availReg.begin();iter!=availReg.end();iter++)
              {
                const TargetRegisterInfo* TRI = Fn.getTarget().getRegisterInfo();
                MCRegAliasIterator* it_alias = new MCRegAliasIterator(*iter, TRI, false);
                while (it_alias->isValid())
                {
                  // **it has type "unsigned"; i.e., it is a preg that is an alias of P
                  availReg.insert(**it_alias);
                  ++(*it_alias);
                }
              }*/
#if MY_DEBUG==1
  cerr<<"Available register are :";
  printRegSet(availReg);
  cerr<<"\n";
#endif
	      numAvailableRegisters=availReg.size();
#if MY_DEBUG==1
  cerr<<"num of unstacked neighbors is "<<numUnstackedNeighbors<<", register class is "<< vregClass<<", num of available register is "<<numAvailableRegisters<<"\n";
#endif
	      if(numUnstackedNeighbors<numAvailableRegisters)
	      {
#if MY_DEBUG==1
  cerr<<"Found an easy one: ";
  if(TargetRegisterInfo::isVirtualRegister((*it)->reg))
    cerr<<"%"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<"\n";
  else
    cerr<<"R"<<(*it)->reg<<"\n";
#endif
	        findEasy=true;
	        workStation1.push(*it);
	        stacked.insert((*it)->reg);
		(*it)->stacked = true;
	      }
	  } // if unstacked and virtual
        } // finalLiveRange loop  	

	if(findEasy==false&&haveVirtual) // dont have easy one but still have virtual register
	{
	  #if MY_DEBUG==2
	    cerr<<"dont have easy one but still have virtual register\n";
	  #endif
	  int max=0;
	  LiveRange* ptr;
	  for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++)
          {
            if((*it)->stacked==false&&TargetRegisterInfo::isVirtualRegister((*it)->reg))
	    {
	      // calculate # of unstacked neighbors
              int numUnstackedNeighbors=0;
              set<unsigned> availReg;
              for(set<LiveRange*>::iterator it_LR=(*it)->neighbors.begin();it_LR!=(*it)->neighbors.end();it_LR++)
              {
                if(stacked.find((*it_LR)->reg)==stacked.end())
                {  
		  numUnstackedNeighbors++;
                }
              }
	      if(numUnstackedNeighbors>max)
	      {
		max=numUnstackedNeighbors;
		ptr=*it;
	      }

	    }
	  } // for loop to find the max unstacked neighbor liveRange
	  workStation1.push(ptr);
	  ptr->stacked=true;
	  stacked.insert(ptr->reg);
	}
      } while(haveVirtual);
      // push all physical ones
      for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++)
      {
        if((*it)->stacked==false&&TargetRegisterInfo::isPhysicalRegister((*it)->reg)) // physical and unstacked
	{
#if MY_DEBUG==1
  cerr<<"Going to push preg R"<<(*it)->reg<<"\n";
#endif
	  workStation1.push(*it);
          (*it)->stacked=true;
          stacked.insert((*it)->reg);
	}
      }
      /*****************  POP  *********************/
      LiveRange* cur;
      while(workStation1.empty()==false) 
      {
	cur=workStation1.top();
	workStation1.pop();
	cur->stacked=false; // is this the correct position to put this statement?
	if(TargetRegisterInfo::isPhysicalRegister(cur->reg)) // physical one
	{
#if MY_DEBUG==2
  cerr<<"R"<<cur->reg<<" popped\n";
#endif
	  cur->color = cur->reg;
	  cur->colored = true;
	}
	else // virtual one
	{
#if MY_DEBUG==2
  cerr<<"%"<<TargetRegisterInfo::virtReg2Index(cur->reg)<<" popped\n";
#endif
	  set<unsigned> poppedNeighborRegs;
	  for(set<LiveRange*>::iterator it_LR=cur->neighbors.begin();it_LR!=cur->neighbors.end();it_LR++)
	  {
	    if((*it_LR)->stacked==false) // it is popped
	    {
	      poppedNeighborRegs.insert((*it_LR)->color);
	    }
	  }
	  for(set<unsigned>::iterator iter=poppedNeighborRegs.begin();iter!=poppedNeighborRegs.end();iter++) // put all the aliasings into account
          {
            const TargetRegisterInfo* TRI = Fn.getTarget().getRegisterInfo();
            MCRegAliasIterator* it_alias = new MCRegAliasIterator(*iter, TRI, false);
            while (it_alias->isValid())
            {
              // **it has type "unsigned"; i.e., it is a preg that is an alias of P
              poppedNeighborRegs.insert(**it_alias);
              ++(*it_alias);
            }
          } // processing preg aliasing
	  
	  // color this virtual reg
	  set<unsigned> availReg;
	  MachineRegisterInfo *MRI = &Fn.getRegInfo();
          int vregClass = MRI->getRegClass(cur->reg)->getID(); // what class is virtual reg V in
          availReg = *(regClassToAvailPregSetMap[vregClass]);
	  bool colorable = false;
	  for(set<unsigned>::iterator it=availReg.begin();it!=availReg.end();it++)
	  {
	    set<unsigned> aliasing;
	    aliasing.insert(*it);
	    const TargetRegisterInfo* TRI = Fn.getTarget().getRegisterInfo();
            MCRegAliasIterator* it_alias = new MCRegAliasIterator(*it, TRI, false);
            while (it_alias->isValid())
            {
              // **it has type "unsigned"; i.e., it is a preg that is an alias of P
              aliasing.insert(**it_alias);
              ++(*it_alias);
            }
#if MY_DEBUG==1
  cerr<<"poppedNeighborRegs: ";
  printRegSet(poppedNeighborRegs);
  cerr<<"aliasing: ";
  printRegSet(aliasing);
  cerr<<"\n";
#endif
	    if(intersect_set<unsigned>(poppedNeighborRegs,aliasing).size()==0)
	    {
	      cur->color = *it;
	      cur->colored = true;
	      colorable = true;
#if MY_DEBUG==2
  cerr<<"Using R"<<*it<<" for %"<<TargetRegisterInfo::virtReg2Index(cur->reg)<<"\n";
#endif
	      break;
	    }
	  } // for loop to find a preg for this vreg
#if MY_DEBUG==3
	  if(colorable==false)
	  {
	    cerr<<"Cannot find a preg for %"<<TargetRegisterInfo::virtReg2Index(cur->reg)<<"\n";
	    cerr<<"Going to exit\n";
	    //exit(0);
	  }
#endif
	} // else: virtual ones
      } // workStation1(pop) loop
#if EXTRA_CREDIT
  //cerr<<"There are "<<workStation2.size()<<" uncolored live range\n";
#endif


#if MY_DEBUG==3
  cerr<<"Coloring completed\n";
#endif
    } // function

    virtual void replaceVregWithPreg(MachineFunction& Fn)
    {
      
#if EXTRA_CREDIT
      int uncolored=0;
      for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++)
      {
	
	if((*it)->colored==false)
	{
	  cerr<<"Replace vreg: Final live range of %"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<" is uncolored\n";
	  uncolored++;
	}
      }
      cerr<<"Replace vreg: There are "<<uncolored<<" uncolored live range\n";

      cerr<<"Replace vreg: Going to replace vreg with preg for all colored\n";
#endif
      const TargetRegisterInfo *TRI = Fn.getTarget().getRegisterInfo();
      for(MachineFunction::iterator MFIt=Fn.begin();MFIt!= Fn.end();MFIt++)
      {
        for(MachineBasicBlock::iterator MBBIt = MFIt->begin(); MBBIt != MFIt->end(); MBBIt++) 
	{
	  int numOp = MBBIt->getNumOperands();
#if MY_DEBUG==1
  cerr<<"Instruction "<<InstrToNumMap[MBBIt]<<" has "<<numOp<<" operands\n";
#endif
          for (int i = 0; i < numOp; i++) 
	  {
	    MachineOperand &MOp = MBBIt->getOperand(i);
	    if(MOp.isReg()&&TargetRegisterInfo::isVirtualRegister(MOp.getReg()))
	    {
#if MY_DEBUG==1
  cerr<<"Processing "<<i<<"th operand, which is a vreg\n";
#endif
	      unsigned P; // preg
	      unsigned V = MOp.getReg();
	      bool replaceable = false;
	      for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++)
	      {
		if((*it)->reg==V&&(*it)->colored)
		{
#if MY_DEBUG==1
  cerr<<"Found a potential live range of "<<i<<"th operand\n";
#endif
		  if((*it)->range.find(InstrToNumMap[MBBIt])!=(*it)->range.end())
		  {
#if MY_DEBUG==1
  cerr<<"Found the correct live range of "<<i<<"th operand\n";
#endif

		    replaceable = true;
		    P = (*it)->color;
		  }
		}
	      }
	      if(replaceable)
	      {
		MOp.substPhysReg(P, *TRI);
#if MY_DEBUG==2
  cerr<<i<<"th operand of instruction %"<<InstrToNumMap[MBBIt]<<" has been replaced by preg R"<<P<<"\n";
#endif
	      }
#if MY_DEBUG==2
	      else
		cerr<<"Wrong. Should be able to replace\n";
#endif
	    }
	  } // Operand level
	} // Instruction level
      } // BasicBlock level
#if MY_DEBUG==2
  cerr<<"Replacing completed\n";
#endif
    }

    virtual void handleUncolored(MachineFunction& Fn)
    {
      const TargetRegisterInfo *TRI = Fn.getTarget().getRegisterInfo();
      const TargetInstrInfo *TII = Fn.getTarget().getInstrInfo();
      MachineRegisterInfo *MRI = &Fn.getRegInfo();
      for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++) // for each final range
      {
        if((*it)->colored==false) // if it is still uncolored
        {
	  int vregClass = MRI->getRegClass((*it)->reg)->getID();
#if EXTRA_CREDIT
  cerr<<"Handle uncolored: Final live range %"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<"(RC: "<<vregClass<<") is uncolored!\n";
#endif
	  // Get stack space for V
      	  const TargetRegisterClass *RC = MRI->getRegClass((*it)->reg);
	  int frameIndex = Fn.getFrameInfo()->CreateSpillStackObject(RC->getSize(), RC->getAlignment());
	  for(set<MachineInstr*>::iterator it_instr = (*it)->rangeInstr.begin();it_instr != (*it)->rangeInstr.end();it_instr++) // iterate each instruction in this range
	  {
#if EXTRA_CREDIT
  cerr<<"Handle uncolored: processing %"<<InstrToNumMap[*it_instr]<<" instruction\n";
#endif
	    int numOp = (*it_instr)->getNumOperands();

	    set<unsigned> usedSpillReg; // maybe wrong 
	    map<unsigned,unsigned> vregToPreg;
	    
            for (int i = 0; i < numOp; i++)
            {
              MachineOperand &MOp = (*it_instr)->getOperand(i);
              if(MOp.isReg()&&TargetRegisterInfo::isVirtualRegister(MOp.getReg())&& MOp.getReg()==(*it)->reg)// operand is a reg and vreg and the live range reg
              {
#if EXTRA_CREDIT
  cerr<<i<<"th operand is %"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<"\n";
#endif
		bool findSreg = false;
                unsigned P; // preg
#if EXTRA_CREDIT
		cerr<<"There are "<<regClassToSpillRegSetMap[vregClass]->size()<<" spill registers in this class\n";
#endif
		for(set<unsigned>::iterator it_sreg = regClassToSpillRegSetMap[vregClass]->begin();it_sreg != regClassToSpillRegSetMap[vregClass]->end();it_sreg++) // iterate through all sreg in this reg class
		{
		  
		  if(usedSpillReg.find(*it_sreg)==usedSpillReg.end())// didn't find, haven't use
		  {
		    findSreg= true;
#if EXTRA_CREDIT
  cerr<<"Handle uncolored: use R"<<*it_sreg<<" for %"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<"\n";
#endif
		    vregToPreg[MOp.getReg()]=*it_sreg; // map
		    usedSpillReg.insert(*it_sreg);
		    /**** INSERT ALL ALIASING ****/
		    //const TargetRegisterInfo *TRI = Fn.getTarget().getRegisterInfo();
		    MCRegAliasIterator *it_alias = new MCRegAliasIterator(*it_sreg, TRI, false);
		    while (it_alias->isValid()) 
		    {  // **it has type "unsigned"; i.e., it is a preg that is an alias of P
		      usedSpillReg.insert(**it_alias);
		      ++(*it_alias);
		    } // aliasing finding loop
		    break; // already found, break
		  }
		} // iterate through all sreg in this reg class
		if(findSreg == false)
		{
#if EXTRA_CREDIT
		  cerr<<"Didn't find a sreg for %"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<"(RC: "<<vregClass<<"), something wrong\n";
  exit(0);
#endif
		}
	      } // if operand is vreg
	    } // operand iteration
	    // now replace vreg with sreg
	    for (int i = 0; i < numOp; i++)
            {
              MachineOperand &MOp = (*it_instr)->getOperand(i);
              if(MOp.isReg()&&TargetRegisterInfo::isVirtualRegister(MOp.getReg())&&MOp.getReg()==(*it)->reg) // operand is a reg and vreg and the live range reg
              {
		const TargetRegisterClass *RC = MRI->getRegClass((*it)->reg);
		MachineBasicBlock *MBB = (*it_instr)->getParent();
#if EXTRA_CREDIT
  cerr<<"Handle uncolored: going to replace "<<i<<"th operand %"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<" with R"<<vregToPreg[MOp.getReg()]<<"\n";
#endif
                //MOp.substPhysReg(vregToPreg[MOp.getReg()], *TRI);
		if(MOp.isUse())
		{
		  TII->loadRegFromStackSlot(*MBB, *it_instr, vregToPreg[MOp.getReg()], frameIndex, RC, TRI);
		}
		if(MOp.isDef())
                {
		  MachineBasicBlock::iterator nextI = *it_instr;
		  nextI++;
		  TII->storeRegToStackSlot(*MBB, nextI, vregToPreg[MOp.getReg()], true, frameIndex, RC, TRI);			
		}
/*#if EXTRA_CREDIT
  cerr<<"Handle uncolored: going to replace "<<i<<"th operand %"<<TargetRegisterInfo::virtReg2Index((*it)->reg)<<" with R"<<vregToPreg[MOp.getReg()]<<"\n";
#endif*/
  		MOp.substPhysReg(vregToPreg[MOp.getReg()], *TRI);
	      }
	    }

	  } // range machine instr iteration
	} // if not colored scope
      } // final live range iteration
#if EXTRA_CREDIT
  cerr<<"Handle uncolored completed\n";
#endif
    }

    virtual void handleFunctionCalls(MachineFunction& Fn)
    {
      for(set<LiveRange*>::iterator it=finalLiveRange.begin();it!=finalLiveRange.end();it++) // For each colored live range (for a virtual register V) such that the live range includes a call
      {
	if(TargetRegisterInfo::isVirtualRegister((*it)->reg)&&(*it)->hasCall&&(*it)->color) 
	{
#if MY_DEBUG==2
  cerr<<"Found a live range with call(s)\n";
#endif
	  // Allocate stack space in which to save the allocated physical register R across the call (do this just once for each live range that includes a call)
	  MachineRegisterInfo *MRI = &Fn.getRegInfo();
	  const TargetRegisterClass *RC = MRI->getRegClass((*it)->reg);
	  int frameIndex = Fn.getFrameInfo()->CreateSpillStackObject(RC->getSize(), RC->getAlignment());
	  // For each call instruction I in the live range
	  for(set<MachineInstr*>::iterator it_instr = (*it)->calls.begin();it_instr!= (*it)->calls.end();it_instr++)
	  {
	    // Generate code to store the current contents of the allocated preg R before the call
	    // do these two assignments just once per function
#if MY_DEBUG==2
  cerr<<"Do this only once per function!\n";
#endif
	    const TargetRegisterInfo *TRI = Fn.getTarget().getRegisterInfo();
	    const TargetInstrInfo *TII = Fn.getTarget().getInstrInfo();
	    // do this for each call instruction I
	    MachineBasicBlock *MBB = (*it_instr)->getParent();
	    TII->storeRegToStackSlot(*MBB, *it_instr, (*it)->color, true, frameIndex, RC, TRI);
	    
	    // Generate code to re-load the allocated preg R after that instruction (i.e., before the next instruction):
	    MachineBasicBlock::iterator IT = *it_instr;
	    IT++;
	    TII->loadRegFromStackSlot(*MBB, IT, (*it)->color, frameIndex, RC, TRI);
	  }
	}
      }
	
#if MY_DEBUG==2
  cerr<<"Handle function calls completed\n";
#endif
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      // Eliminate PHI nodes before we get the CFG.
      // This works by inserting copies into predecessor blocks.
      // So the code is no longer in SSA form.
      AU.addRequiredID(PHIEliminationID); 
      // This pass used to be required.  Including it now causes
      // a runtime error.
      //      AU.addRequiredID(TwoAddressInstructionPassID);
      MachineFunctionPass::getAnalysisUsage(AU);
    }
    
  private:
    //**********************************************************************
    // doInit
    //
    // fill in
    //  RDfactSet:     set of all reaching-def facts in this function
    //                 i.e., the universe of facts for reaching-defs
    //                       analysis
    //  InstrToNumMap: map from instruction to unique # (for debugging)
    //  vregToAvailPregSetMap
    //                 map from each vreg used in this fn to its available
    //                 set of pregs (taking into account the "allocation
    //                 order" and the "reserved regs" and not putting any
    //                 preg that occurs in this fn -- or any of its aliases --
    //                 in an available set)
    //
    // Return T iff there are vregs in this fn
    //**********************************************************************
    bool doInit(MachineFunction &Fn) 
    {
      bool yesVregs = false;
      std::set<unsigned> usedPregSet;   // pregs that occur in this fn
                                        // useful when finding spill registers
      std::set<unsigned> vregSet;       // vregs defined in this fn
      MRI = &Fn.getRegInfo();
      const TargetRegisterInfo *TRI = Fn.getTarget().getRegisterInfo();
      
      // iterate over all basic blocks, all instructions in a block,
      // all operands in an instruction
      int insNum = 1;
      for (MachineFunction::iterator MFIt = Fn.begin(), MFendIt = Fn.end();
	   MFIt != MFendIt; MFIt++) 
      {
	for (MachineBasicBlock::iterator MBBIt = MFIt->begin(),
	       MBBendIt = MFIt->end(); MBBIt != MBBendIt; MBBIt++) 
	{
	  //*MBBIt is a MachineInstr
	  InstrToNumMap[MBBIt] = insNum;
	  insNum++;
	  int numOp = MBBIt->getNumOperands();
	  for (int i = 0; i < numOp; i++) 
	  {
	    MachineOperand &MOp = MBBIt->getOperand(i);  
	    if (MOp.isReg() && MOp.getReg()) 
	    {
	      unsigned reg = MOp.getReg();
	      // Here if this operand is
	      //  (a) a register
	      //  (b) not special reg 0
	      // Add it to vregSet or usedPregSet depending on whether
	      // it is a vreg or a preg; if a preg, also add all aliases
	      if (TargetRegisterInfo::isVirtualRegister(reg)) {
		vregSet.insert(reg);
		yesVregs = true;  // found a vreg!
	      } 
	      else 
	      {
		usedPregSet.insert(reg);
		addAliases(&usedPregSet, reg, TRI);
	      }
	      if (MOp.isDef()) 
	      {
		RDfactSet.insert(new RDfact(reg, MBBIt));
	      } // end a def of a reg
	    } // end operand is a register
	  } // end for each operand
	} // end iterate over all instructions in 1 basic block
      } // end iterate over all basic blocks in this fn
      
      // now fill in regClassToAvailPregSetMap:
      // iterate over register classes (for this machine architecture)
      // for each, get its available set of pregs, taking into account
      // the "allocation order" and the "reserved regs" and the
      // set of pregs already used in this fn
      BitVector reservedRegs = TRI->getReservedRegs(Fn);
      for (int k=0; k<numRegClasses; k++) {
	set<unsigned> *availPregSet = new set<unsigned>();
	regClassToAvailPregSetMap[k] = availPregSet; // empty set
	const TargetRegisterClass *trc = TRI->getRegClass(k);
	ArrayRef<uint16_t> rawOrder = trc->getRawAllocationOrder(Fn);
	ArrayRef<uint16_t>::iterator rItr = rawOrder.begin();
	while (rItr != rawOrder.end()) 
	{
	  if (reservedRegs.test(*rItr)) 
	  {
	    // this register is reserved -- do NOT add it to avail set
	    ++rItr;
	  } 
	  else 
	  {
	    //  add to avail set for this reg class
	    unsigned preg = *rItr;
	    availPregSet->insert(preg);
	    ++rItr;
	  }
	} // end iterate over rawOrder
	if (DEBUG_AVAIL) 
	{
	  std::cerr << "Avail set for register class " << k << ": ";
	  printRegSet(availPregSet);
	} // end if DEBUG_AVAIL
      } // end iterate over register classes
      usedPreg = usedPregSet;
      return yesVregs;
    } // end doInit


    //**********************************************************************
    // addAliases
    //
    // given: S         ptr to set of registers
    //        reg       (unsigned) one reg
    //        TRI       TargetRegisterInfo
    //
    // do: add all aliases of reg to S (only a preg has aliases)
    //**********************************************************************
    void addAliases(std::set<unsigned> *S, unsigned reg,
		    const TargetRegisterInfo *TRI) {
      if (TargetRegisterInfo::isPhysicalRegister(reg)) {
	MCRegAliasIterator *it = new MCRegAliasIterator(reg, TRI, false);
	while (it->isValid()) {
	  S->insert(**it);
	  ++(*it);
	}
      }      
    }
    
    //**********************************************************************
    // doLiveAnalysis
    //**********************************************************************
    void doLiveAnalysis(MachineFunction &Fn) {
      // initialize live maps to empty
      liveBeforeMap.clear();
      liveAfterMap.clear();
      liveVarsGenMap.clear();
      liveVarsKillMap.clear();
      insLiveBeforeMap.clear();
      insLiveAfterMap.clear();
      
      analyzeBasicBlocksLiveVars(Fn);
      analyzeInstructionsLiveVars(Fn);
    }
    
    //**********************************************************************
    // doReachingDefsAnalysis
    //**********************************************************************
    void doReachingDefsAnalysis(MachineFunction &Fn) {
      analyzeBasicBlocksRDefs(Fn);
      analyzeInstructionsRDefs(Fn);
    }
    
    //**********************************************************************
    // analyzeBasicBlocksLiveVars
    //
    // iterate over all basic blocks bb
    //    bb.gen = all upwards-exposed uses in bb
    //    bb.kill = all defs in bb
    //    put bb on the worklist
    //
    // also fill in (globals) firstBB, argPregSet
    //**********************************************************************d
    void analyzeBasicBlocksLiveVars(MachineFunction &Fn) {
      
      // initialize all before/after/gen/kill sets and
      // put all basic blocks on the worklist
      set<MachineBasicBlock *> worklist;
      firstBB = 0;
      MachineInstr *firstInstr = 0;      // first instruction in curr function
      for (MachineFunction::iterator MFIt = Fn.begin(), MFendIt = Fn.end();
	   MFIt != MFendIt; MFIt++) {
	if (firstInstr == 0) {
	  firstBB = MFIt;
	  MachineBasicBlock::iterator MBBIt = MFIt->begin();
	  //*MBBIt is a MachineInstr
	  firstInstr = MBBIt;
	}
	liveBeforeMap[MFIt] = new set<unsigned>();
	liveAfterMap[MFIt] = new set<unsigned>();
	liveVarsGenMap[MFIt] = getUpwardsExposedUses(MFIt);
	liveVarsKillMap[MFIt] = getAllDefs(MFIt);
	worklist.insert(MFIt);
      }
      
      // while the worklist is not empty {
      //   remove one basic block bb
      //   compute new bb.liveAfter = union of liveBefore's of all successors
      //   replace old liveAfter with new one
      //   compute new bb.liveBefore = (bb.liveAfter - bb.kill) union bb.gen
      //   if bb.liveBefore changed {
      //      replace old liveBefore with new one
      //      add all of bb's predecessors to the worklist
      //   }
      // }
      while (! worklist.empty()) {
	// remove one basic block and compute its new liveAfter set
	set<MachineBasicBlock *>::iterator oneBB = worklist.begin();
	MachineBasicBlock *bb = *oneBB;
	worklist.erase(bb);
	
	set<unsigned> *newLiveAfter = computeLiveAfter(bb);
	
	// update the liveAfter map
	liveAfterMap.erase(bb);
	liveAfterMap[bb] = newLiveAfter;
	// compute its new liveBefore, see if it has changed (it can only
	// get bigger)
	set<unsigned> *newLiveBefore = computeLiveBefore(bb);
	set<unsigned> *oldLiveBefore = liveBeforeMap[bb];
	if (newLiveBefore->size() > oldLiveBefore->size()) {
	  // update the liveBefore map and put all preds of bb on worklist
	  liveBeforeMap.erase(bb);
	  liveBeforeMap[bb] = newLiveBefore;
	  for (MachineBasicBlock::pred_iterator PI = bb->pred_begin(),
		 E = bb->pred_end();
	       PI != E; PI++) {
	    worklist.insert(*PI);
	  }
	}
      }
      argPregSet = liveBeforeMap[firstBB];
    }
    
    //**********************************************************************
    // analyzeBasicBlocksRDefs
    //**********************************************************************
    void analyzeBasicBlocksRDefs(MachineFunction &Fn) {
      // iterate over all basic blocks bb computing
      //    bb.gen = for each reg v defined in bb at inst: the RDfact
      //             (v, inst)
      //    bb.kill = all dataflow facts with reg v
      // also put bb on the worklist
      
      set<MachineBasicBlock *> worklist;
      for (MachineFunction::iterator MFIt = Fn.begin(), MFendIt = Fn.end();
	   MFIt != MFendIt; MFIt++) {
	RDbeforeMap[MFIt] = new set<RDfact *>();
	RDafterMap[MFIt] = new set<RDfact *>();
	RDgenMap[MFIt] = getRDgen(MFIt);
	RDkillMap[MFIt] = getRDkill(MFIt);
	worklist.insert(MFIt);
      }
      
      // while the worklist is not empty {
      //   remove one basic block bb
      //   compute new bb.RDbefore = union of RDafter's of all preds
      //   replace old RDbefore with new one
      //   compute new bb.RDafter = (bb.RDbefore - bb.RDkill) union
      //                              bb.RDgen
      //   if bb.RDafter changed {
      //      replace old RDbefore with new one
      //      add all of bb's succs to the worklist
      //   }
      // }
      while (! worklist.empty()) {
	// remove one basic block and compute its new RDbefore set
	set<MachineBasicBlock *>::iterator oneBB = worklist.begin();
	MachineBasicBlock *bb = *oneBB;
	worklist.erase(bb);
	
	set<RDfact *> *newRDbefore = computeRDbefore(bb);
	
	// update the RDbefore map
	RDbeforeMap.erase(bb);
	RDbeforeMap[bb] = newRDbefore;
	// compute its new RDafter, see if it has changed (it can only
	// get bigger)
	set<RDfact *> *newRDafter = computeRDafter(bb);
	set<RDfact *> *oldRDafter = RDafterMap[bb];
	if (newRDafter->size() > oldRDafter->size()) {
	  // update the RDafter map and put all succs of bb on worklist
	  RDafterMap.erase(bb);
	  RDafterMap[bb] = newRDafter;
	  for (MachineBasicBlock::succ_iterator PI = bb->succ_begin(),
		 E = bb->succ_end();
	       PI != E; PI++) {
	    worklist.insert(*PI);
	  }
	}
      }
    }
    
    // **********************************************************************
    // computeLiveBefore
    //
    // given: bb          ptr to a MachineBasicBlock 
    //
    // do:    compute and return bb's current LiveBefore set:
    //          (bb.liveAfter - bb.kill) union bb.gen
    // **********************************************************************
    set<unsigned> *computeLiveBefore(MachineBasicBlock *bb) {
      return regSetUnion(regSetSubtract(liveAfterMap[bb],
					liveVarsKillMap[bb]
					),
			 liveVarsGenMap[bb]
			 );
    }
    
    
    // **********************************************************************
    // computeLiveAfter
    //
    // given: bb  ptr to a MachineBasicBlock 
    //
    // do:    compute and return bb's current LiveAfter set: the union
    //        of the LiveBefore sets of all of bb's CFG successors
    // **********************************************************************
    set<unsigned> *computeLiveAfter(MachineBasicBlock *bb) {
      set<unsigned> *result = new set<unsigned>();
      for (MachineBasicBlock::succ_iterator SI = bb->succ_begin();
	   SI != bb->succ_end(); SI++) {
	MachineBasicBlock *oneSucc = *SI;
	result = regSetUnion(result, liveBeforeMap[oneSucc]);
      }
      
      return result;
    }
    
    
    // **********************************************************************
    // computeRDbefore
    //
    // given: bb  ptr to a MachineBasicBlock 
    //
    // do:    compute and return bb's current RDbefore set: the union
    //        of the RDafter sets of all of bb's CFG preds, except if
    //        bb is *first*, then a set of RDfacts for the pregs that
    //        are used to pass in args
    // **********************************************************************
    set<RDfact *> *computeRDbefore(MachineBasicBlock *bb) {
      set<RDfact *> *result = new set<RDfact *>();
      if (bb == firstBB) {
	for (std::set<unsigned>::iterator IT = argPregSet->begin();
	     IT != argPregSet->end();
	     IT++) {
	  result->insert(new RDfact(*IT, 0));
	}
      } else {
	for (MachineBasicBlock::pred_iterator SI = bb->pred_begin();
	     SI != bb->pred_end(); SI++) {
	  MachineBasicBlock *onePred = *SI;
	  result = RDsetUnion(result, RDafterMap[onePred]);
	}
	if (result->size() > RDfactSet.size()) {
	  std::cerr << "INTERNAL ERROR, bad new RDfact before set\n";
	  printRDSet(result);
	  std::cerr << "\n";
	  exit(1);
	}
      }
      return result;
    }
    
    // **********************************************************************
    // computeRDafter
    //
    // given: bb          ptr to a MachineBasicBlock 
    //
    // do:    compute and return bb's current RDafter set:
    //          (bb.RDbefore - bb.kill) union bb.gen
    // **********************************************************************
    set<RDfact *> *computeRDafter(MachineBasicBlock *bb) {
      return RDsetUnion(RDsetSubtract(RDbeforeMap[bb],
				      RDkillMap[bb]
				      ),
			RDgenMap[bb]
			);
    }
    
    
    
    // **********************************************************************
    // regSetUnion
    //
    // given: S1, S2          ptrs to sets of regs
    // do:    return a ptr to (*S1 union *S2)
    // **********************************************************************
    set<unsigned> *regSetUnion(set<unsigned> *S1, set<unsigned> *S2) {
      set<unsigned> *result = new set<unsigned>();
      // iterate over S1
      for (set<unsigned>::iterator oneRegPtr = S1->begin();
	   oneRegPtr != S1->end();
	   oneRegPtr++) {
	result->insert(*oneRegPtr);
      }
      
      // iterate over S2
      for (set<unsigned>::iterator oneRegPtr = S2->begin();
	   oneRegPtr != S2->end();
	   oneRegPtr++) {
	result->insert(*oneRegPtr);
      }
      
      return result;
    }
    
    // **********************************************************************
    // RDsetUnion
    //
    // given: S1, S2          ptrs to sets of ptrs to RDfacts
    // do:    return a ptr to (*S1 union *S2)
    // **********************************************************************
    set<RDfact *> *RDsetUnion(set<RDfact *> *S1, set<RDfact *> *S2) {
      set<RDfact *> *result = new set<RDfact *>();
      // iterate over S1
      for (set<RDfact *>::iterator oneRDfact = S1->begin();
	   oneRDfact != S1->end();
	   oneRDfact++) {
	result->insert(*oneRDfact);
      }
      
      // iterate over S2
      for (set<RDfact *>::iterator oneRDfact = S2->begin();
	   oneRDfact != S2->end();
	   oneRDfact++) {
	result->insert(*oneRDfact);
      }
      
      return result;
    }
    
    
    // **********************************************************************
    // regSetSubtract
    //
    // given: S1, S2          ptrs to sets of regs
    // do:    return a ptr to (*S1 - *S2)
    //
    // **********************************************************************
    set<unsigned> *regSetSubtract(set<unsigned> *S1, set<unsigned> *S2) {
      set<unsigned> *result = new set<unsigned>();
      // iterate over S1; for each element, if it is NOT in S2, then
      // add it to the result
      for (set<unsigned>::iterator S1RegPtr = S1->begin();
	   S1RegPtr != S1->end();
	   S1RegPtr++) {
	if (S2->count(*S1RegPtr) == 0) {
	  result->insert(*S1RegPtr);
	}
      }
      
      return result;
    }
    
    // **********************************************************************
    // RDsetSubtract
    //
    // given: S1, S2          ptrs to sets of RDfact ptrs
    // do:    return a ptr to (*S1 - *S2)
    //
    // **********************************************************************
    set<RDfact *> *RDsetSubtract(set<RDfact *> *S1, set<RDfact *> *S2) {
      set<RDfact *> *result = new set<RDfact *>();
      // iterate over S1; for each element, if it is NOT in S2, then
      // add it to the result
      for (std::set<RDfact *>::iterator S1It = S1->begin();
	   S1It != S1->end();
	   S1It++) {
	RDfact *fact1 = *S1It;
	bool found = false;
	std::set<RDfact *>::iterator S2It = S2->begin();
	while (!found && S2It != S2->end()) {
	  RDfact *fact2 = *S2It;
	  if (fact1->getReg() == fact2->getReg() &&
	      fact1->getInstr() == fact2->getInstr()) found = true;
	  S2It++;
	}
	if (!found) result->insert(fact1);
      } // end iterate over S1
      return result;
    }

    //**********************************************************************
    // analyzeInstructionsLiveVars
    //
    // do live-var analysis at the instruction level:
    //   iterate over all basic blocks
    //   for each, iterate backwards over instructions, propagating
    //             live-var info:
    //     for each instruction inst
    //             live-before = (live-after - kill) union gen
    //     where kill is the defined reg of inst (if any) and
    //           gen is all reg-use operands of inst
    //**********************************************************************
    void analyzeInstructionsLiveVars(MachineFunction &Fn) {
      for (MachineFunction::iterator bb = Fn.begin(), bbe = Fn.end(); 
	   bb != bbe; bb++) {
	// no reverse iterator and recursion doesn't work,
	// so create vector of instructions for backward traversal
	vector<MachineInstr *> instVector;
	for (MachineBasicBlock::iterator inIt = bb->begin();
	     inIt != bb->end();
	     inIt++) {
	  instVector.push_back(inIt);
	}
	
	liveForInstr(instVector, liveAfterMap[bb]);
      }
    }
    
    //**********************************************************************
    // analyzeInstructionsRDefs
    //
    // given reaching-defs before and after facts for basic block,
    // compute before/after facts for each instruction in each basic block
    //
    // for one instruction: RDafter = (RDbefore - kill) union gen
    // where kill is all dataflow facts with the regs that are defined
    // by this instruction (if any), and gen is the set of facts (reg, inst)
    // for all regs defined by this instruction (if any)
    //**********************************************************************
    void analyzeInstructionsRDefs(MachineFunction &Fn) {
      // iterate over all basic blocks in this function
      for (MachineFunction::iterator bb = Fn.begin(), bbe = Fn.end(); 
	   bb != bbe; bb++) {
	set<RDfact *> *RDbefore = RDbeforeMap[bb];
	// iterate over all instructions in this basic block
	for (MachineBasicBlock::iterator inIt = bb->begin();
	     inIt != bb->end();
	     inIt++) {
	  insRDbeforeMap[inIt] = RDbefore;
	  set<RDfact *> *kill = new set<RDfact *>();
	  set<RDfact *> *gen = new set<RDfact *>();
	  set<unsigned> *regDefs = getOneInstrRegDefs(inIt);
	  // if at least one reg was defined
	  // then compute gen and kill sets for this instruction
	  if (regDefs->size() > 0) {
	    for (set<unsigned>::iterator regIt = regDefs->begin();
		 regIt != regDefs->end(); regIt++) {
	      unsigned oneDef = *regIt;
	      gen->insert(new RDfact(oneDef, inIt));
	      // iterate over all RDfacts, see which are killed
	      for (set<RDfact *>::iterator IT = RDfactSet.begin();
		   IT != RDfactSet.end(); IT++) {
		RDfact *oneRDfact = *IT;
		unsigned oneReg = oneRDfact->getReg();
		if (oneReg == oneDef) {
		  kill->insert(oneRDfact);
		}
	      } // end iterate over all RDfacts to compute kill
	    } // end iterate over set of regs defined by one instruction

	    // we've now defined the gen and kill sets so we can
	    // compute the "after" fact for this instruction
	    set<RDfact *> *RDafter = RDsetUnion(RDsetSubtract(RDbefore, kill),
						gen);
	    insRDafterMap[inIt] = RDafter;
	    RDbefore = RDafter;
	  } else {
	    // this instruction doesn't define any reg
	    insRDafterMap[inIt] = RDbefore;
	  }
	} // end iterate over all instructions in 1 basic block
      } // end iterate over all basic blocks
    }
    
    // **********************************************************************
    // getUpwardsExposedUses
    //
    // given: bb      ptr to a basic block
    // do:    return a ptr to the set of regs that are used before
    //        being defined in bb
    // **********************************************************************
    set<unsigned> *getUpwardsExposedUses(MachineBasicBlock *bb) {
      set<unsigned> *result = new set<unsigned>();
      set<unsigned> *defs = new set<unsigned>();
      for (MachineBasicBlock::iterator instruct = bb->begin(),
	     instructEnd = bb->end(); instruct != instructEnd; instruct++) {
	set<unsigned> *uses = getOneInstrRegUses(instruct);
	set<unsigned> *upUses = regSetSubtract(uses, defs);
	result = regSetUnion(result, upUses);
	set<unsigned> *defSet = getOneInstrRegDefs(instruct);
	for (set<unsigned>::iterator IT = defSet->begin();
	     IT != defSet->end(); IT++) {
	  unsigned oneDef = *IT;
	  defs->insert(oneDef);
	}
      } // end iterate over all instrutions in this basic block
      
      return result;
    }
    
    
    // **********************************************************************
    // getRDgen
    //
    // given: bb      ptr to a basic block
    // do:    return a set of reaching-def facts: the ones that occur in bb
    // **********************************************************************
    set<RDfact *> *getRDgen(MachineBasicBlock *bb) {
      set<RDfact *> *result = new set<RDfact *>();
      for (MachineBasicBlock::iterator instruct = bb->begin(),
	     instructEnd = bb->end(); instruct != instructEnd; instruct++) {
	set<unsigned> *defSet = getOneInstrRegDefs(instruct);
	for (set<unsigned>::iterator IT = defSet->begin();
	     IT != defSet->end(); IT++) {
	  unsigned oneDef = *IT;
	  result->insert(new RDfact(oneDef, instruct));
	}
      } // end iterate over all instructions in this basic block
      
      return result;
    }
    
    // **********************************************************************
    // getRDkill
    //
    // given: bb      ptr to a basic block
    // do:    return a set of reaching-def facts: the ones whose reg
    //        component is defined in bb
    // **********************************************************************
    set<RDfact *> *getRDkill(MachineBasicBlock *bb) {
      set<RDfact *> *result = new set<RDfact *>();
      for (MachineBasicBlock::iterator instruct = bb->begin(),
	     instructEnd = bb->end(); instruct != instructEnd; instruct++) {
	set<unsigned> *defSet = getOneInstrRegDefs(instruct);
	for (set<unsigned>::iterator IT = defSet->begin();
	     IT != defSet->end(); IT++) {
	  unsigned oneDef = *IT;
	  for (set<RDfact *>::iterator IT = RDfactSet.begin();
	       IT != RDfactSet.end(); IT++) {
	    RDfact *oneRDfact = *IT;
	    unsigned oneReg = oneRDfact->getReg();
	    if (oneReg == oneDef) {
	      result->insert(oneRDfact);
	    }
	  } // end iterate over all RDfacts in the whole fn
	} // end iterate over all defs in this instruction
      } // end iterate over all instructions in this basic block
      
      return result;
    }
    
    //**********************************************************************
    // getOneInstrRegUses
    //
    // return the set of registers (virtual or physical) used by the
    // given instruction
    //**********************************************************************
    set<unsigned> *getOneInstrRegUses(MachineInstr *instruct) {
      set<unsigned> *result = new set<unsigned>();
      unsigned numOperands = instruct->getNumOperands();
      for (unsigned n=0; n<numOperands; n++) {
	MachineOperand MOp = instruct->getOperand(n);
	if (MOp.isReg() && MOp.getReg() && MOp.isUse()) {
	  unsigned reg = MOp.getReg();
	  result->insert(reg);
	}
      } // end for each operand of current instruction
      return result;
    }
    
    //**********************************************************************
    // getOneInstrRegDefs
    //
    // return a ptr to a set of the registers defined by this instruction
    //**********************************************************************
    set<unsigned> *getOneInstrRegDefs(MachineInstr *instruct) {
      set<unsigned> *result = new set<unsigned>();
      unsigned numOperands = instruct->getNumOperands();
      for (unsigned n=0; n<numOperands; n++) {
	MachineOperand MOp = instruct->getOperand(n);
	if (MOp.isReg() && MOp.getReg() && MOp.isDef()) {
	  unsigned reg = MOp.getReg();
	  result->insert(reg);
	}
      } // end for each operand of current instruction
      return result;
    }
    
    // **********************************************************************
    // getAllDefs
    //
    // given: bb      ptr to a basic block
    // do:    return the set of regs that are defined in bb
    // **********************************************************************
    set<unsigned> *getAllDefs(MachineBasicBlock *bb) {
      set<unsigned> *result = new set<unsigned>();
      
      // iterate over all instructions in bb
      //   for each operand that is a non-zero reg:
      //     if it is a def then add it to the result set
      // return result
      // 
      for (MachineBasicBlock::iterator instruct = bb->begin(),
	     instructEnd = bb->end(); instruct != instructEnd; instruct++) {
	unsigned numOperands = instruct->getNumOperands();
	for (unsigned n=0; n<numOperands; n++) {
	  MachineOperand MOp = instruct->getOperand(n);
	  if (MOp.isReg() && MOp.getReg() && MOp.isDef()) {
	    result->insert(MOp.getReg());
	  }
	} // end for each operand of current instruction
      } // end iterate over all instrutions in this basic block
      return result;
    }
    
    // **********************************************************************
    // liveForInstr
    //
    // given: instVector vector of ptrs to Instructions for one basic block
    //        liveAfter  live after set for the *last* instruction in the block
    //
    // do:    compute and set liveAfter and liveBefore for each instruction
    //        liveAfter = liveBefore of next instruction
    //        liveBefore = (liveAfter - kill) union gen
    // **********************************************************************
    void liveForInstr(vector<MachineInstr *>instVector,
		      set<unsigned> *liveAfter) {
      while (instVector.size() > 0) {
	MachineInstr *oneInstr = instVector.back();
	instVector.pop_back();
	insLiveAfterMap[oneInstr] = liveAfter;
	
	// create liveBefore for this instruction
	// (which is also liveAfter for the previous one in the block)
	//   remove the reg defined here (if any) from the set
	//   then add all used reg operands
	
	set<unsigned> *liveBefore;
	set<unsigned> *gen = getOneInstrRegUses(oneInstr);
	set<unsigned> *kill = getOneInstrRegDefs(oneInstr);
	if (kill->size() != 0) {
	  liveBefore = regSetUnion(regSetSubtract(liveAfter, kill), gen);
	} else {
	  liveBefore = regSetUnion(liveAfter, gen);
	}
	
	// add this instruction's liveBefore set to the map
	// and prepare for the next iteration of the loop
	insLiveBeforeMap[oneInstr] = liveBefore;
	liveAfter = liveBefore;
      } // end while
    }
    
    //**********************************************************************
    // getNumRegClasses
    //
    // set field numRegClasses to value for this machine arch
    //**********************************************************************
    void getNumRegClasses(MachineFunction &Fn) {
      numRegClasses = Fn.getTarget().getRegisterInfo()->getNumRegClasses();
#if MY_DEBUG==1
      std::cerr<<"DEBUG::numRegClasses is "<<numRegClasses<<"\n";
#endif
    }
    
    // **********************************************************************
    // printInstructions
    // **********************************************************************
    void printInstructions(MachineFunction &F) {
      std::cerr << "\nMACHINE INSTRUCTIONS\n";
      // iterate over all basic blocks
      for (MachineFunction::iterator bb = F.begin(); bb != F.end(); bb++) {
	cerr << "Basic Block " << bb->getName().str() << "\n";
	// iterate over instructions, printing each
	for (MachineBasicBlock::iterator inIt = bb->begin(), ine = bb->end();
	     inIt != ine; inIt++) {
	  MachineInstr *oneI = inIt;
	  cerr << "%" << InstrToNumMap[oneI] << ": ";
	  oneI->dump();
	}
      }
      std::cerr << "\n";
    }

    // **********************************************************************
    // printLiveResults
    //
    // given: MachineFunction F
    //
    // do:    for each basic block in F {
    //           print fn name, bb number, liveBefore and After sets
    //           for each instruction, print instruction num, liveBefore and
    //               liveAfter
    //        }
    // 
    // **********************************************************************
    void printLiveResults(MachineFunction &F) {
      std::cerr << "\nLIVE VARS\n";
      
      // iterate over all basic blocks
      for (MachineFunction::iterator bb = F.begin(); bb != F.end(); bb++) {
	// print name of basic block
	std::cerr << "\nBASIC BLOCK " << bb->getName().str() << "\n";
	// print live before and after sets
	std::cerr << "  L-Before: ";
	printRegSet(liveBeforeMap[bb]);
	std::cerr << "  L-After: ";
	printRegSet(liveAfterMap[bb]);
	
	// iterate over instructions, printing each live set
	// (note that liveAfter of one instruction is liveBefore of the next one)
	for (MachineBasicBlock::iterator inIt = bb->begin(), ine = bb->end();
	     inIt != ine; inIt++) {
	  std::cerr << "%" << InstrToNumMap[inIt] << ": ";
	  std::cerr << "\tL-Before: ";
	  printRegSet(insLiveBeforeMap[inIt]);
	  std::cerr << "\tL-After: ";
	  printRegSet(insLiveAfterMap[inIt]);
	}
      }
    }
    
    // **********************************************************************
    // printRDResults
    //
    // given: MachineFunction F
    //
    // do:    for each basic block in F {
    //           print fn name, bb number, RDBefore and After sets
    //           for each instruction, print instruction num, RDBefore and
    //               RDAfter
    //        }
    // 
    // **********************************************************************
    void printRDResults(MachineFunction &F) {
      std::cerr << "\nREACHING DEFS\n";
      
      // iterate over all basic blocks
      for (MachineFunction::iterator bb = F.begin(); bb != F.end(); bb++) {
	// print name of basic block
	std::cerr << "BASIC BLOCK " << bb->getName().str() << "\n";
	// print RD before and after sets
	std::cerr << "  RD-Before: ";
	printRDSet(RDbeforeMap[bb]);
	std::cerr << "  RD-After: ";
	printRDSet(RDafterMap[bb]);
	
	// iterate over instructions, printing each RD set
	// (note that RDAfter of one instruction is RDBefore of the next one)
	for (MachineBasicBlock::iterator inIt = bb->begin(), ine = bb->end();
	     inIt != ine; inIt++) {
	  std::cerr << "%" << InstrToNumMap[inIt] << ": ";
	  std::cerr << "\tRD-Before: ";
	  printRDSet(insRDbeforeMap[inIt]);
	  std::cerr << "\tRD-After: ";
	  printRDSet(insRDafterMap[inIt]);
	  std::cerr << "\n";
	}
      }
    }
    
    // **********************************************************************
    // printRegSet
    //
    // given: S      ptr to set of regs (unsigned)
    // do:    print the set
    // ********************************************************************
    void printRegSet(set<unsigned> *S) 
    {
      std::cerr << "{";
      for (set<unsigned>::iterator IT = S->begin(); IT != S->end(); IT++) {
	unsigned reg = *IT;
	std::cerr << " ";
	printReg(reg);
      }
      std::cerr << " }\n";
    }
      
    // **********************************************************************
    // printReg
    //
    // given: unsigned           vreg or preg
    //
    // do: print R for physical, % for virtual, followed by reg number
    //
    // NOTE: To print virtual-reg index use:
    //           TargetRegisterInfo::virtReg2Index(reg)
    // **********************************************************************
    void printReg(unsigned reg) 
    {
      if (TargetRegisterInfo::isPhysicalRegister(reg))
      {
	std::cerr << "R" << reg;
      } 
      else 
      {
	std::cerr << "%" << TargetRegisterInfo::virtReg2Index(reg);
      }
    }
    // **********************************************************************
    // printRDSet
    //
    // given: S      ptr to set of RDfact
    // do:    print the set
    // **********************************************************************
    void printRDSet(set<RDfact *> *S) {
      std::cerr << "{";
      for (set<RDfact *>::iterator IT = S->begin(); IT != S->end(); IT++) {
	RDfact *oneRDfact = *IT;
	MachineInstr *oneIns = oneRDfact->getInstr();
	std::cerr << "(";
	printReg(oneRDfact->getReg());
	std::cerr << ", %" << InstrToNumMap[oneIns] << ") ";
      }
      std::cerr << " }\n";
    }
    
    //**********************************************************************
    // printRegSet
    //**********************************************************************
    void printRegSet(set<unsigned> S) {
      for (set<unsigned>::iterator IT = S.begin(); IT != S.end(); IT++) {
	unsigned reg = *IT;
	std::cerr << reg << " ";
      }
    }
    
    // **********************************************************************
    // update (global) RDfactSet by adding a fact of the form (p, 0)
    // for each preg p in (global) argPregSet
    // **********************************************************************
    void addArgPregsToRDfactSet() {
      for (std::set<unsigned>::iterator IT = argPregSet->begin();
	   IT != argPregSet->end();
	   IT++) {
	RDfactSet.insert(new RDfact(*IT, 0));
      }
    }

  };
  
  // The library-inclusion mechanism requires the following:
  char Gcra::ID = 0;
  
  FunctionPass *createGcra() { return new Gcra(); }
  
  static RegisterRegAlloc register_gcra("gc",
					"graph-coloring register allocator",
					createGcra);
}
