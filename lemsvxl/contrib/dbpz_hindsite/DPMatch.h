
#ifndef __TBS_DPMATCH_DEF__
#define __TBS_DPMATCH_DEF__

#include <iostream>
#include <string>
#include <vector>

#include "Curve.h"

//#include "thresholds.h"


#define DP_VERY_LARGE_COST 1E12

template <class curveType,class floatType>
class DPMatch{
 public:
  DPMatch();
  DPMatch(curveType &c1, curveType &c2);
  
  //Constructor from NewDPMatch:
  DPMatch(curveType &c1,curveType &c2,int nstart, int mstart);
  ~DPMatch(){};
  
  
  //access functions
  floatType finalCost(){return _finalCost;};
  floatType finalCost(floatType cost){_finalCost=cost; return _finalCost;};
  std::vector < std::pair <int,int> > finalMap(){return _finalMap;};
  std::vector< floatType > finalMapCost(){return _finalMapCost;};
  int n(){return _n;};
  int m(){return _m;};

  //display functions (debug)
  void printCost();
  void writeCost(std::string f);
  void printMap();
  void printFinalMap();
  curveType curve1(){return _curve1;};
  curveType curve2(){return _curve2;};
  void match();
  std::pair<int,int> Start(){std::pair<int,int> tmp(_nstart,_mstart); return tmp;}
  std::pair<int,int> End(){std::pair<int,int> tmp(_nend,_mend); return tmp;}
  double length;
 protected:

  //Data
  curveType _curve1;
  curveType _curve2;
  std::vector< std::vector<floatType> > _cost;
  std::vector< std::vector< std::pair <int,int> > > _map;
  std::vector< std::vector< std::pair <floatType,floatType> > > _angle;
  std::vector< std::pair <int,int> > _finalMap;
  floatType _finalCost;
  std::vector< floatType > _finalMapCost;
  int _n;
  int _m;

  int _nend;
  int _mend;

  int _nstart;
  int _mstart;

  bool _start_flag;
  bool _end_flag;
  bool _flip;

  //From header file of NewDPMatch
  int _numLenElems;
  floatType _R1;
  floatType _R2;
  floatType _lambda;
  
  //Functions
  void initializeDPCosts();
  void computeDPCosts();
  void findDPCorrespondence();
  //Altered from virtual function to real function from NewDPMatch
  floatType computeIntervalCost(int i, int ip, int j, int jp,
        int ri, int rj, int C_FLAG);

  
};

//###########################


template <class curveType,class floatType>
DPMatch<curveType,floatType>::DPMatch() {

  std::vector< std::vector<floatType> > a;
  std::vector< std::vector< std::pair <int,int> > > b;
  std::vector< std::pair <int,int> > c;
  std::vector< floatType > d;

  curveType c1,c2;
  _curve1 = c1;
  _curve2 = c2;
  _cost = a;
  _map = b;
  _finalMap = c;
  _finalMapCost = d;
  _finalCost = 0;
  _m = 0;
  _n = 0;

  
}

template <class curveType,class floatType>
DPMatch<curveType,floatType>::DPMatch(curveType &c1, curveType &c2){
  
  int n;

  //std::cout << "In DPMatch Constructor" << std::endl;
  _curve1 = c1;
  _curve2 = c2;
  
  std::cout << "Curve1" << std::endl;
  _curve1.printElems();
  std::cout << "Curve End" << std::endl;

  std::cout << "Curve2" << std::endl;
  _curve2.printElems();
  std::cout << "Curve End" << std::endl;
  _curve1.computeProperties();
  _curve2.computeProperties();
  _n=_curve1.numPoints();
  _m=_curve2.numPoints();

  for (n=0;n<_n;n++){
    std::vector<floatType> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    std::pair <int,int> tmp3(0,0);
    std::vector< std::pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);
    }
  _finalMap = 0;
  _finalMapCost = 0;
  _finalCost = DP_VERY_LARGE_COST;
  
  //From NewDPMatch
  _R1 = 1.0;
  _R2 = 1.0;
}

//Code added to combine DPMatch and NewDPMatch
//Constructor from NewDPMatch
template <class curveType,class floatType>
DPMatch<curveType,floatType>::DPMatch(curveType &c1, curveType &c2, 
              int nstart, int mstart){
  
  std::vector< std::vector<floatType> > a;
  std::vector< std::vector< std::pair <int,int> > > b;
  std::vector< std::pair <int,int> > c;
  std::vector< floatType > d;

  _curve1 = c1;
  _curve2 = c2;
  _cost = a;
  _map = b;
  _finalMap = c;
  _finalMapCost = d;

  int n;   
  _flip = false;
  _n=_curve1.numPoints();
  _m=_curve2.numPoints();
  
  for (n=0;n<_n;n++){
    std::vector<floatType> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);

    std::pair <int,int> tmp3(0,0);
    std::vector< std::pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);

    std::pair<floatType,floatType> tmp4(0,0);
    std::vector <std::pair<floatType,floatType> > tmp5(_m,tmp4);
    _angle.push_back(tmp5);
  }
 
  _finalCost = DP_VERY_LARGE_COST;
  _R1 = 15.0;
  _R2 = 0.0;
  _lambda =1.0;
  _numLenElems=1;
  _nstart=nstart;
  _mstart=mstart;
  _start_flag=false;
  _end_flag=false;
  

  
  
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::printCost(){
  int i,j;
  std::cout << "Cost Matrix" << std::endl;
  for (i = 0; i<33; i++){
    for (j = 0; j<33; j++){
      if(i<ceil((double)j/3.0) || j<ceil((double)i/3.0))
  printf("%s","  --- ");
      else if(_cost[i][j]>5000)
  printf("%s","  --- ");
      else if(_cost[i][j]>50)
  printf("%s","  BIG ");
      else
  printf("%5.1f ",_cost[i][j]);
  //printf("%3.2f ",_angle[i][j].second);
    }
    printf("\n");
  }
} 

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::writeCost(std::string fname){
  FILE *fp=fopen(fname.c_str(),"w");
  int i,j;
  floatType c;
  for (i = 0; i<_n; i++){
    for (j = 0; j<_m; j++){
      c=_cost[i][j];
      fwrite(&c,sizeof(floatType),1,fp);
    }
  }
  fclose(fp);
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::printMap(){
  int i,j;
  printf("Map Matrix\n");
  for (i = 0; i<25; i++){
    for (j = 0; j<25; j++){
      printf("(%2d,%2d) ", _map[i][j].first, _map[i][j].second);
    }
    printf("\n");
  }
} 

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::printFinalMap(){
  int j;
  std::cout << "Final Map" << std::endl;
  for (j = 0; j<_finalMap.size(); j++)
    std::cout << _finalMap[j].first << " " << _finalMap[j].second << std::endl;;
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::initializeDPCosts(){


  double l10,l20;

  l10=_curve1.arcLength(_nstart);
  l20=_curve2.arcLength(_mstart);

  //std::cout<<_nstart<<" "<<_mstart<<std::endl;  
  //  printf("%f %f\n",l10,l20);

  double c0=l10/l20;

  if(c0<TH_L || c0>TH_U) 
    _start_flag=true; //_cost[0][0]=DP_VERY_LARGE_COST; //(c0+1/c0)*(l10+l20)/20;
  //else
  _cost[0][0]=0;//-2/(c0+1/c0) * (l10+l20)/20;
     
}


template <class curveType,class floatType>
void DPMatch<curveType,floatType>::computeDPCosts(){
 

int XOFFSET[6] = {-1,-1,-2,-2,-1,-3};
int YOFFSET[6] = {-1,-2,-1,-2,-3,-1};

  int i,ip,j,jp,k,r;
  floatType cost;
  float min_cost;
  int ii,iip,jj,jjp;
  
  double tlc;

  //int pnend,pmend,cnend,cmend;

  _nend=0;
  _mend=0;
  //pnend=0;
  //pmend=0;
  //cnend=0;
  //cmend=0;
  
  double ivc, pvc;
  int last_good_row = -1;
  int start=1;

  min_cost=0.0;r=1;i=1;j=r;
  while(r<_n && r<_m) {
    
    ii=i+_nstart;if(ii>_n-1) ii-=_n;
    jj=j+_mstart;if(jj>_m-1) jj-=_m;
    tlc=fabs(_curve1.mergeLength(_nstart,ii)-_curve2.mergeLength(_mstart,jj))*0.1;
    
    for (k=0;k<4;k++){ 
      ip=i+XOFFSET[k];
      jp=j+YOFFSET[k];
      
      iip=ip+_nstart;if(iip>_n-1) iip-=_n;
      jjp=jp+_mstart;if(jjp>_m-1) jjp-=_m;
  
      if (ip>=0 &&  jp>=0) { 

  if (ip==0 && jp==0)
    ivc=computeIntervalCost(ii,iip,jj,jjp,ip,jp,0);
  else
    ivc = computeIntervalCost(ii,iip,jj,jjp,ip,jp,1);

  pvc = _cost[ip][jp];
  cost = pvc+ivc;

  if (cost<_cost[i][j]){
    _cost[i][j]=cost;
    _map[i][j].first=ip; 
    _map[i][j].second=jp;
    _angle[i][j].first=_curve1.mergeAngle(iip,ii);
    _angle[i][j].second=_curve2.mergeAngle(jjp,jj);
  }  
      }
    }
    
    if(_cost[i][j]+tlc<min_cost) {
      min_cost = static_cast<float>(_cost[i][j]+tlc);
      last_good_row = r;
      _nend=i;
      _mend=j;
    }
    else if(_cost[i][j]-min_cost<10.0)
      last_good_row = r;
   
    if(i<r) i++;
    else j--;
    
    if(j<start){
      r++;
      if(r-last_good_row > 2){
  //  if(_nstart==38 && _mstart==0){
  //    std::cout << _nend << " " << _mend << " " << min_cost << std::endl;
  //    printCost();
  //    printMap();
  //  }      
  return;
      }
      j=r;
      start=static_cast<int>(ceil((double)r/2.0));
      i=start;
    }
    
    if(i>_n-1) i=_n-1;
    if(j>_m-1) j=_m-1;
    
  //pnend=cnend;
  //pmend=cmend;
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  double out_cost = _cost[i][j];
#endif
  
  }  
}


//From NewDPMatch.cpp:

// Cost of matching the  interval [x(l),x(i)] to the interval [y(k),y(j)].
template <class curveType,class floatType>
floatType DPMatch<curveType,floatType>::computeIntervalCost(int i, int ip, int j, int jp,
                     int ri,int rj, int flag){
  floatType cost,dF=0,dK=0;

  double l1,l2,tLen,c,l10,l20,c01,c02,angle1,angle2,ang,adj; 

  angle1=0.0;
  angle2=0.0;
  ang=0.0;

  l1=_curve1.mergeLength(ip,i);
  l2=_curve2.mergeLength(jp,j);
  c=(l1/l2+l2/l1)/2.0;
  
  l10=_curve1.totalLength(ip,i);
  c01=l10/l1;
  
  l20=_curve2.totalLength(jp,j);   
  c02=l20/l2;
  
  if(flag) {   
    angle1 = angleDiff(_curve1.mergeAngle(ip,i),_angle[ri][rj].first);
    angle2 = angleDiff(_curve2.mergeAngle(jp,j),_angle[ri][rj].second);
    ang = fabs(angle1-angle2);
  }
  
  tLen=(_curve1.mergeLength(_nstart,i) + _curve2.mergeLength(_mstart,j))/2.0;
  adj=1.0*(1.0-TH_A)*exp(-pow(1.5*tLen/TH_LA,4))+TH_A;

  if( ((c01<1.0-TH_D || c01>1.0+TH_D ) && l1>MIN_D) || (( c02<1.0-TH_D || c02>1.0+TH_D) && l2>MIN_D) ){ 
    cost=DP_VERY_LARGE_COST;
  }
  else if(c<1.0-2.0*TH_L || c>1.0+2.0*TH_L || ang>2.0*adj) {
    cost=DP_VERY_LARGE_COST;
  }
  else {
    dF = -(exp(-pow(c-1,8)/(2*pow(TH_L,8)))) + exp(-0.5); 
    dK = -(exp(-pow(ang,8)/(2*pow(adj,8)))) + exp(-0.5);
    cost = (dF+LAM*dK) * (l1+l2)/20;
  }
  
  
  /*
  if(_nstart==6 && _mstart==7) {
    std::cout << i << " " << j << " " << ip << " " << jp << std::endl;
    std::cout << "Length1: " << l1 << std::endl;
    std::cout << "Length2: " << l2 << std::endl;
    std::cout << "Ang1: " << _curve1.mergeAngle(ip,i) << std::endl; 
    std::cout << "Ang2: " << _curve2.mergeAngle(jp,j) << std::endl;
    std::cout << "Ang1p: " << _angle[ri][rj].first << std::endl; 
    std::cout << "Ang2p: " << _angle[ri][rj].second << std::endl;
    std::cout << "Angle1: " << angle1 << std::endl; 
    std::cout << "Angle2: " << angle2 << std::endl;
    std::cout << "Adiff: " << ang << std::endl;
    std::cout << "PCost: " << _cost[ri][rj] << std::endl;
    std::cout << "Cost: " << cost << std::endl;
    std::cout << "Tcost: " << _cost[ri][rj]+cost << std::endl << std::endl;
  }
  */
  return cost;  


}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::findDPCorrespondence(){

  int i,j,ip,jp;
  _finalMap.clear();
  _finalMapCost.clear();
  
  ip=_nend;
  jp=_mend;
  i=_nend;
  j=_mend;
  std::pair <int,int> p(i,j);
  _finalMap.push_back(p);
  _finalMapCost.push_back(_cost[p.first][p.second]);
  
  //std::cout << _nstart << " " << _mstart << std::endl << "-----" << std::endl;
  
  while (ip > 0 || jp > 0){
    ip=_map[i][j].first;
    jp=_map[i][j].second;
    
    //*********************************************************

    //double a1=angleDiff(_angle[i][j].first,_angle[ip][jp].first);
    //double a2=angleDiff(_angle[i][j].second,_angle[ip][jp].second);

    //std::cout << a1 << " " << a2 << " " << fabs(a1-a2) << std::endl;

    /*
      ii=i+_nstart;if(ii>_n-1) ii-=_n;
      jj=j+_mstart;if(jj>_m-1) jj-=_m;
      iip=ip+_nstart;if(iip>_n-1) iip-=_n;
      jjp=jp+_mstart;if(jjp>_m-1) jjp-=_m;
      
      if(ip==0 && jp==0)
      std::cout << computeIntervalCost(ii,iip,jj,jjp,ip,jp,0) << std::endl; 
      else
      std::cout << computeIntervalCost(ii,iip,jj,jjp,ip,jp,1) << std::endl;
    */    

    //*********************************************************

    std::pair <int,int> p(ip,jp);
    _finalMap.push_back(p);
    _finalMapCost.push_back(_cost[p.first][p.second]);

    //std::cout << ip << " " << jp << std::endl;

    if((ip==0 && jp!=0) || (ip!=0 && jp==0)) {
      std::cout << "ERROR" << std::endl;
      exit(-1);
    }

    _finalCost=_cost[_nend][_mend];

    i=ip;
    j=jp;    
  }
  //std::cout << std::endl;
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::match(){

  _cost[0][0]=0;
  _map[0][0].first=-1;
  _map[0][0].second=-1;
 
  computeDPCosts();
  findDPCorrespondence();
}




#endif
