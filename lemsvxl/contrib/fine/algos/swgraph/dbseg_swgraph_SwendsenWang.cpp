#include "SwendsenWang.h"
#include <cmath>
#include "functions.h"
#include "sampler.h"

//
// SwendsenWangAbstract
//
//
//#define showlog
bool SwendsenWangAbstract::Do(){
    double p0,p1=0,q01,q10,a,d,qm01,qm10,pc10,pc01;
    double Temp;
#ifdef showlog
    double pp0,pp1;
#endif
    double q1i0,q1i1;

    if (step<start) step=start;
    Temp=GetTemp();    step++;
    oldl=SampleComponent(Nrelabel);//Nrelabel=cluster that will change label
    newl=SampleNewLabel(Nrelabel,q01);    // q01= prob to change label in A->B
    if (newl==oldl)
        return false;                    //no change, so rejected

    if (q01+1==1){                        //if q01==0 reject
        return false;
    }
#ifdef showlog
    pp0=logprob();
#endif
    p0=logprob(oldl,newl);                // posterior, state A
    
    q1i0=logProdWeights(Nrelabel,oldl);    // log prod weights in state A (for A->B)

    pc01 =probComponent(Nrelabel,0);        // prob to choose component in A->B
    qm10=lpNewModel(newl)+lpNewModel(oldl);    // prob to choose model in B->A (does not depend on B)

    SetLabel(Nrelabel,newl);            // Now we are in state B

    qm01=lSampleNewModel(oldl)+lSampleNewModel(newl);// prob to choose model in A->B (does not depend on A)

#ifdef showlog
    pp1=logprob();
#endif
    q10=pNewLabel(Nrelabel,oldl);        // prob to go back to old label by sampling (B->A)
    pc10 =probComponent(Nrelabel,1);        // prob to choose component in B->A

    if (q10+1==1){                        //if q10==0 reject
        SetLabelBack(Nrelabel,oldl);
        return false;
    }

    p1=logprob(oldl,newl);                // posterior, state B
    q1i1=logProdWeights(Nrelabel,newl);// log prod weights in state B (for B->A)

    a=(p1-p0)/Temp+q1i1-q1i0+log((q10/q01)*(pc10/pc01))+qm10-qm01;
    if (a>0) a=0;
    a=exp(a);
//    sumcompsize+=Nrelabel.size();
//    nsteps++;
//    if (Nrelabel.size()>maxcompsize) 
//        maxcompsize=Nrelabel.size();
    d=randdbl(1);
    if (d>=a){        //if reject, undo, don't forget to change model back;
        SetLabelBack(Nrelabel,oldl);
#ifdef showlog
        double pp2=logprob();
        if (fabs(pp2-pp0)>1.){
            FILE *f=fopen("swlog.txt","a+");
            fprintf(f,"l0=%d,l1=%d,pp0=%1.1f,pp1=%f,pp2=%1.1f,p0=%1.1f,p1=%1.1f,q10=%1.1f,q1i1=%1.1f,qm10=%1.1f,q01=%1.1f,q1i0=%1.1f,qm01=%1.1f,a=%1.1f,step=%d\r\n",oldl,newl,pp0,pp1,pp2,p0,p1,q10,q1i1,qm10,q01,q1i0,qm01,a,step);
            fclose(f);
        }
#endif
        return false;
    }
    else{
//        sumaccsize+=Nrelabel.size();nacc++;if (Nrelabel.size()>maxaccsize) maxaccsize=Nrelabel.size();
#ifdef showlog
        if (pp1<pp0-1.){
            FILE *f=fopen("swlog.txt","a+");
            fprintf(f,"l0=%d,l1=%d,pp0=%1.1f,pp1=%f,p0=%1.1f,p1=%1.1f,q10=%1.1f,q1i1=%1.1f,qm10=%1.1f,q01=%1.1f,q1i0=%1.1f,qm01=%1.1f,a=%1.1f,step=%d\r\n",oldl,newl,pp0,pp1,p0,p1,q10,q1i1,qm10,q01,q1i0,qm01,a,step);
            fclose(f);
        }
#endif
        return true;
    }
}

bool SwendsenWangAbstract::Do_SWC3(){
    int oldl;
    double Temp,prob;
    vector<int>    alllabels;
    int i,nCls,l;
    bool allobj;
    msampler s;

    if (step<start) step=start;
    Temp=GetTemp();    step++;
//    if (step==298733)
//        step=step;
    oldl=SampleComponent(Nrelabel);        //Nrelabel=cluster that will change label
    allobj=(GetNodeCount(oldl)==Nrelabel.size());
    GetUsedLabels(alllabels);
    nCls=alllabels.size();
    if (!allobj){
        nCls++;
        l=GetNewLabel();
        alllabels.push_back(l);
    }
    s.SetDimension(nCls,1);
    for (i=0;i<nCls;i++){
        l=alllabels[i];
        SetLabel(Nrelabel,l);    
        if (allobj &&(l==oldl))
            s(i)=logprob();
        else{
            s(i)=logprob()+logProdWeights(Nrelabel,l);
        }
    }
    s.logprob2prob(Temp);
    prob=s.sample(i);
    l=alllabels[i];
    SetLabel(Nrelabel,l);
    return l!=oldl;
}
double    SwendsenWangAbstract::GetTemp(){
    return 0.1;
    return ::getlogTemp(startTemp,endTemp,step,nsweeps*node_size());
}
void SwendsenWangAbstract::SetTemperatureRange(double startT, double endT){
    startTemp=startT;
    endTemp=endT;
}


//
// Wolff
//

double Wolff::Grow_component(vector<int> &C, deque<int> &Q){
    int j,k,l,n;
    vector<int> N;
    double sum=0;
    while (!Q.empty()){
        j=Q.front();
        Q.pop_front();
        sum+=GetSampledNeighbors(j,N);
        n=N.size();
        for (k=0;k<n;k++){
            l=N[k];
            if (!visited(l)){
                C.push_back(l);
                Q.push_back(l);
                Set_visited(l);
            }
        }
    }
    return sum;
}

double Wolff::Grow_component(int i, vector<int> &C){
    //grow component from seed i
    deque<int> Q;
    C.clear();
    C.push_back(i);
    Set_visited(i);
    Q.push_back(i);
    return Wolff::Grow_component(C,Q);
}

bool DiffusionAbstract::Do_diffusion(){
    int i;
    bool newobj;
    double p0,p1=0,q01,q10,a,d;
//    double qm10,qm01;
    double Temp;
#ifdef showlog
    double pp0,pp1;
#endif

    Temp=GetTemp();    d_step++;
    d=Sample_boundary(i);
    if (d+1==1){                            //if q01==0 reject
        return false;
    }
    curr_i=i;
    old_l=GetLabel(i);
    new_l=SampleNewLabel(curr_i,q01);q01*=d;
    newobj=IsEmpty(new_l);
    if (new_l==old_l)
        return false;                        //no change, so rejected

    if (q01+1==1){                            //if q01==0 reject
        return false;
    }

#ifdef showlog
    pp0=logprob();
#endif
    p0=logprob(old_l,new_l);
    
//    qm10=lpNewModel(new_l)+lpNewModel(old_l);

    SetLabel(i,new_l);                    // Now we are in the new config

//    qm01=lSampleNewModel(old_l)+lSampleNewModel(new_l);// prob to choose model in A->B (does not depend on A)

#ifdef showlog
    pp1=logprob();
#endif
    q10=pNewLabel(curr_i,new_l)*p_boundary(i);

    if (q10+1==1){                            //if q10==0 reject
        SetLabel(i,old_l);
        return false;
    }

    p1=logprob(old_l,new_l);
    a=(p1-p0)/Temp+log(q10/q01);//+qm10-qm01;
    if (a>0) a=0;
    a=exp(a);
    d=randdbl(1);
    if (d>=a){        //if reject, undo
        SetLabelBack(i,old_l);
        return false;
    }
    else{
#ifdef showlog
        if (pp1<pp0-0.1){
            FILE *f=fopen("swlog.txt","a+");
            fprintf(f,"l0=%d,l1=%d,pp0=%f,pp1=%f,p0=%f,p1=%f,q10=%f,q01=%f,a=%f\r\n",old_l,new_l,pp0,pp1,p0,p1,q10,q01,a);
            fclose(f);
        }
#endif
        return true;
    }
}




