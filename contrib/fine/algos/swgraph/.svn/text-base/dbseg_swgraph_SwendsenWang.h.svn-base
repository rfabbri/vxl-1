#ifndef _SWENDSENWANG_H
#define _SWENDSENWANG_H

#include<vector>
#include <deque>
using namespace std;

class SwendsenWangAbstract{ // Abstract SWC aglorithm, needs implementation of some functions to work,
                            // independent of graph structure.
protected:
public:
    int oldl;        // oldl=label before reassignment
    int newl;        // newl=label after reassignment
    vector<int>        Nrelabel;    // nodes that will change label
    int                start,step;    //starting step and current step
    double            startTemp,endTemp;
    int                nsweeps;        //n sweeps after which temp becomes endTemp

    SwendsenWangAbstract()    {
        step=start=0;nsweeps=20;startTemp=20;endTemp=1;
    }

    bool            Do();
    bool            Do_SWC3();
    int                Get_step(){return step;}
    void            Set_step(int s){step=s;}

    virtual double     GetTemp();
            int        Get_sweep(){return (step-start)/node_size();}
    virtual void    SetTemperatureRange(double startT, double endT);
    virtual double     probComponent(vector<int> &N, int which){return 1.;}
    virtual double    lSampleNewModel(int l){return 0;} //change model l based on current state, return logprob
    virtual double    lpNewModel(int l){return 0;}

    //Must be implemented
    virtual double    logprob()=0;                //the posterior
    virtual double    logprob(int l1,int l2)=0;    //the log prob of obj l1, l2, together
    virtual    double    logProdWeights(vector<int> &N, int l)=0;//the log of the SW Cut from N to l
    virtual int        GetLabel(int i)=0;            // the label of i
    virtual void    SetLabel(int i, int l)=0;    // assigns label l to i 
    virtual void    SetLabel(vector<int> &N, int l)=0; //assigns label l to the vertices in N 
    virtual void    SetLabelBack(vector<int> &N, int l){SetLabel(N,l);} //restores label back since was rejected
    virtual int     SampleNewLabel(vector<int> &N,double &prob)=0;//finds a new label for N by sampling
    virtual double     pNewLabel(vector<int> &N, int l)=0;    //prob of N getting label l
    virtual bool    IsEmpty(int l)=0;            // true if there is nothing with label l
    virtual int        GetNewLabel()=0;            // return a new unused label
    virtual int        node_size()=0;                // the number of nodes
    virtual int        GetNodeCount(int l)=0;        // the number of nodes with label l
    virtual void    GetUsedLabels(vector<int> &L)=0;// the set of used labels
    virtual int     SampleComponent(vector<int> &N)=0;    // samples the graph edges, then samples a connected 
                                                        // component of the graph, returns its label
        //the prob of choosing that component, usually cancels. which =0 (A->B) 1(B->A)
//    virtual void    ChangeModelBack(int l){}
};

class Wolff{
public:
            double Grow_component(int i, vector<int> &N);
            double Grow_component(vector<int> &N, deque<int> &Q);
    virtual void Set_visited(int i)=0;
    virtual bool visited(int i)=0;
    virtual double GetSampledNeighbors(int i,vector<int> &N)=0;
};
class DiffusionAbstract{
public:
    int                old_l,new_l;
    int                curr_i;                //current pix for diffusion
    int                d_step;                //current step

    DiffusionAbstract()    {d_step=0;}
            bool    Do_diffusion();
    virtual double     GetTemp(){return 1;}
    virtual double    Sample_boundary(int &i)=0;
    virtual double    p_boundary(int i)=0;
    virtual int        GetLabel(int i)=0;
    virtual void    SetLabel(int i, int l)=0; //assigns label l to i 
    virtual void    SetLabelBack(int i, int l){SetLabel(i,l);} 
    virtual double    lSampleNewModel(int l){return 0;} //change model l based on current state, return logprob
    virtual double    lpNewModel(int l){return 0;}
    virtual int     SampleNewLabel(int i,double &prob)=0;    //finds a new label for i by sampling
    virtual double     pNewLabel(int i, int l)=0;        //prob of i getting label l
    virtual bool    IsEmpty(int l){return false;}    //true if there is nothing with label l
    virtual double    logprob()=0;                    //the posterior
    virtual double    logprob(int l1,int l2)=0;        //the log prob of obj l1, l2, together
};
#endif

