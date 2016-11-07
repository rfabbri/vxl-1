#include "DPMatch.h"

//int XOFFSET[9] = { 0,-1,-1,-2,-1,-3,-2,-3,-1};
//int YOFFSET[9] = {-1,-3,-2,-3,-1,-2,-1,-1, 0};
int XOFFSET[9] = {-1, 0,-1,-1,-2,-2,-3,-1,-3};
int YOFFSET[9] = {-1,-1, 0,-2,-1,-3,-2,-3,-1};

template <class curveType,class floatType>
DPMatch<curveType,floatType>::DPMatch(){

  vector< vector<floatType> > a;
  vector< vector< pair <int,int> > > b;
  vector< pair <int,int> > c;
  vector< floatType > d;

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

  cout << "In DPMatch Constructor" << endl;
  _curve1 = c1;
  _curve2 = c2;
  
  cout << "Curve1" << endl;
  _curve1.printElems();
  cout << "Curve End" << endl;
  cout << "Curve2" << endl;
  _curve2.printElems();
  cout << "Curve End" << endl;
  _curve1.computeProperties();
  _curve2.computeProperties();
  _n=_curve1.numPoints();
  _m=_curve2.numPoints();
  for (n=0;n<_n;n++){
    vector<floatType> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    pair <int,int> tmp3(0,0);
    vector< pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);
    }
//  _finalMap = 0;
//  _finalMapCost = 0;
  _finalMap.clear();
  _finalMapCost.clear();
  _finalCost = DP_VERY_LARGE_COST;
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::printCost(){
  int i,j;
  cout << "Cost Matrix" << endl;
  for (i = 0; i<_n; i++){
    for (j = 0; j<_m; j++){
      // cout << _cost[i][j] << " ";
      printf("%6.3f ",_cost[i][j]);
    }
    // cout << endl;
    printf("\n");
  }
} 

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::writeCost(string fname){
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
  for (i = 0; i<_n; i++){
    for (j = 0; j<_m; j++){
      printf("(%2d,%2d) ", _map[i][j].first, _map[i][j].second);
    }
    printf("\n");
  }
} 

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::printFinalMap(){
  int j;
  cout << "Final Map" << endl;
  for (j = 0; j<_finalMap.size(); j++)
    cout << _finalMap[j].first << " " << _finalMap[j].second << endl;;
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::initializeDPCosts(){

  //Cost Matrix Initialization
  _cost[0][0]=0.0;
}


template <class curveType,class floatType>
void DPMatch<curveType,floatType>::computeDPCosts(){

  //printf("IN COMPUTEDPCOSTS N=%d M=%d \n",_n,_m);
  //fflush(stdout);
  
  int sum,start,i,ip,j,jp,k;
  floatType cost;
  //printCost();
  //printMap();
    

  //int diff,oldiff;
  for (sum = 1; sum<_n+_m-1; sum++){
    //printf("Sum=%d\n",sum);
    //     fflush(stdout);
    start=max(0,sum-_m+1);
    for (i=start;(i<=_n-1 && i<=sum);i++){
      j=sum-i;
      //printf("(%d %d)\n ",i,j);
      fflush(stdout);
      for (k=0;k<9;k++){
	ip=i+XOFFSET[k];
	jp=j+YOFFSET[k];
	//diff=XOFFSET[k]+YOFFSET[k];
	if (ip >= 0 &&  jp >=0){
	  //printf("(%d %d),(%d %d) %6.3f\n ",i,j,ip,jp,cost);
	  //fflush(stdout);
	  cost =_cost[ip][jp]+computeIntervalCost(i,ip,j,jp);
	  if (cost < _cost[i][j]){
	    _cost[i][j]=cost;
	    _map[i][j].first=ip;
	    _map[i][j].second=jp;
	  }
	}
      }
    }
  }

  //printCost();printMap();
}
	  

// Cost of matching the interval [x(i-1),x(i)]  to [y(k),y(j)].
template <class curveType,class floatType>
floatType DPMatch<curveType,floatType>::computeIntervalCost(int i, int ip, 
							 int j, int jp){
  //Here the cost is based on lengths of the segments.
  floatType s11,s12,s21,s22;
  floatType cost;
  //cout << i << j << k << endl;
    
  s11=_curve1.arcLength(ip);
  s12=_curve1.arcLength(i);
  s21=_curve2.arcLength(jp);
  s22=_curve2.arcLength(j);
  cost = fabs(fabs(s12-s11)-fabs(s22-s21));
  return cost;
}

template <class curveType,class floatType>
floatType DPMatch<curveType,floatType>::computeIntervalCost1(int i, int ip, 
							 int j, int jp){
  //Here the cost is based on lengths of the segments.
  floatType s11,s12,s21,s22;
  floatType cost;
  //cout << i << j << k << endl;
    
  s11=_curve1.arcLength(ip);
  s12=_curve1.arcLength(i);
  s21=_curve2.arcLength(jp);
  s22=_curve2.arcLength(j);
  cost = fabs(fabs(s12-s11)-fabs(s22-s21));
  return cost;
}






template <class curveType,class floatType>
void DPMatch<curveType,floatType>::findDPCorrespondence(){

  //cout << "In DP Corresp" << endl;
  //fflush(stdout);
  int i,j,ip,jp;
  _finalMap.clear();
  _finalMapCost.clear();
  _finalCost=_cost[_n-1][_m-1];
  
  ip=_n-1;
  jp=_m-1;
  i=_n-1;
  j=_m-1;
  pair <int,int> p(ip,jp);
  _finalMap.push_back(p);
  _finalMapCost.push_back(_cost[p.first][p.second]);


  while (ip > 0 || jp > 0){
    ip=_map[i][j].first;
    jp=_map[i][j].second;
    pair <int,int> p(ip,jp);
    _finalMap.push_back(p);
    _finalMapCost.push_back(_cost[p.first][p.second]);
    
    _cost1.push_back(computeIntervalCost1(i,ip,j,jp));

    i=ip;
    j=jp;
  }
  _cost1.push_back(0);
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::match(){
  //cout << "in DP Match" << endl;
  initializeDPCosts();
  //cout << "initializeDPCosts done" << endl;
  computeDPCosts();
  //cout << "computeDPCosts done" << endl;
  findDPCorrespondence();
  //cout << "corresp done" << endl;
}

