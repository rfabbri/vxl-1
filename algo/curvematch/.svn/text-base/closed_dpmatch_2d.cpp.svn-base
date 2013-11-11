#include "closed_dpmatch_2d.h"

closed_dpmatch_2d::closed_dpmatch_2d(){
  //curveType c1,c2;
  //vector< vector<floatType> > a;
  //vector<int> b;
  //vector< vector< pair <int,int> > > c;
  //vector<floatType> d;

  //_curve1 = c1;
  //_curve2 = c2;
  //_cost = a;
  //_map = c;
  //_finalMap = c;
  //_finalMapCost = a;
  //_finalCost = d;
  _m = 0;
  _n = 0;
  //_leftMask=b;
  //_rightMask=b;
}

closed_dpmatch_2d::closed_dpmatch_2d(bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2){
  int n;

  _curve1 = c1;
  _curve2 = c2;
  _n=c1->size();     //No. of points in original curve
  _m=_curve2->size();
  //Append a copy to the first curve 
  for (n=0;n<_n;n++)
    _curve1->add_vertex(c1->x(n), c1->y(n));
  

  
  // caller function should not compute properties of the curves to avoid duplicate computation
  _curve1->computeProperties();
  _curve2->computeProperties();
  

  for (n=0;n<2*_n;n++){
    vcl_vector<double> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);
  }
  
  _leftMask.insert(_leftMask.begin(),_m,0);
  _rightMask.insert(_rightMask.begin(),_m,2*_n-1);
  

  for(n=0;n<_n+1;n++){
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(1,tmp3);
    _finalMap.push_back(tmp2);
  }
  vcl_vector<double> tmp5(_n+1,DP_VERY_LARGE_COST);
  _finalCost = tmp5;

  for (n=0;n<_n+1;n++){
    vcl_vector<double> tmp1(_m,DP_VERY_LARGE_COST);
    _finalMapCost.push_back(tmp1);
  }
}



void closed_dpmatch_2d::printCost(){
  int i,j;
  vcl_cout << "Cost Matrix" << vcl_endl;
  for (i = 0; i<_n; i++){
    for (j = 0; j<_m; j++){
      // cout << _cost[i][j] << " ";
      printf("%6.3f ",_cost[i][j]);
    }
    // cout << endl;
    printf("\n");
  }
}

void closed_dpmatch_2d::writeCost(vcl_string fname){
  FILE *fp=fopen(fname.c_str(),"w");
  int i,j;
  double c;
  for (i = 0; i<_n; i++){
    for (j = 0; j<_m; j++){
      c=_cost[i][j];
      fwrite(&c,sizeof(double),1,fp);
    }
  }
  fclose(fp);
}

void closed_dpmatch_2d::printMap(){
  int i,j;
  printf("Map Matrix\n");
  for (i = 0; i<_n; i++){
    for (j = 0; j<_m; j++){
      printf("(%2d,%2d) ", _map[i][j].first, _map[i][j].second);
    }
    printf("\n");
  }
} 

/*void closed_dpmatch_2d::printFinalMap(){
  int j;
  vcl_cout << "Final Map" << vcl_endl;
  for (j = 0; j<_finalMap.size(); j++)
    vcl_cout << _finalMap[j].first << " " << _finalMap[j].second << vcl_endl;;
}*/

void closed_dpmatch_2d::initializeDPMask(int i, int j){

  //cout << "In initializeDPMask with " << i <<" " << j << endl;
  int ci=0,cj=1;
  int Ni=_finalMap[i].size(); //Size of optimal paths i and j
  int Nj=_finalMap[j].size();
  int ii;
  int jj;

  vcl_vector< vcl_pair<int,int> > Pi,Pj;

  //Find approximate path that goes through all grid points
  //Left path
  for (ii=0;ii<Ni-1;ii++){
    int x1,x2,y1,y2;
    x1=_finalMap[i][ii].first;x2=_finalMap[i][ii+1].first;
    y1=_finalMap[i][ii].second;y2=_finalMap[i][ii+1].second;
    Pi.push_back(_finalMap[i][ii]);
    for (jj=y1+1;jj<y2;jj++){
      vcl_pair<int,int> p(x1,jj);
      Pi.push_back(p);
    }
    for (jj=x1+1;jj<x2;jj++){
      vcl_pair<int,int> p(jj,y2);
      Pi.push_back(p);
    }
  }
  Pi.push_back(_finalMap[i][Ni-1]);
  //Right path
  for (ii=0;ii<Nj-1;ii++){
    int x1,x2,y1,y2;
    x1=_finalMap[j][ii].first;x2=_finalMap[j][ii+1].first;
    y1=_finalMap[j][ii].second;y2=_finalMap[j][ii+1].second;
    Pj.push_back(_finalMap[j][ii]);
    for (jj=x1+1;jj<x2;jj++){
      vcl_pair<int,int> p(jj,y1);
      Pj.push_back(p);
    }
    for (jj=y1+1;jj<y2;jj++){
      vcl_pair<int,int> p(x2,jj);
      Pj.push_back(p);
    }
  }
  Pj.push_back(_finalMap[j][Nj-1]);
  
  Ni=Pi.size();
  Nj=Pj.size();
  //   cout << Ni << " " << Nj << endl;
  //   cout << "Path i "<< endl;
  //   for (jj=0;jj<Ni;jj++)
  //     cout << jj << " " << _finalMap[i][jj].first << " " << _finalMap[i][jj].second << endl;
  //   cout << "Path j "<< endl;
  //   for (jj=0;jj<Nj;jj++)
  //     cout << jj << " " << _finalMap[j][jj].first << " " << _finalMap[j][jj].second << endl;
  
  for (jj=0;jj<_m;jj++){
    while(ci <Ni && Pi[ci].second<jj){
      ci++;
    }
    int start = Pi[ci].first;
    while(cj < Nj && Pj[cj].second < jj+1){
      cj++;
    }
    int end = Pj[cj-1].first;
    _leftMask[jj]=start;
    _rightMask[jj]=end;
    //cout << jj <<" Start= " << start << " End= " << end << " " << cj-1 << endl;
    for (ii=0;ii<=2*_n-1;ii++)
      _cost[ii][jj]=DP_VERY_LARGE_COST;
  }

//   cout << "printing masks" << endl;
//   for (jj=0;jj<_m;jj++){
//     cout << jj << " " << _leftMask[jj] << " " << _rightMask[jj] << endl;
//   }
//   cout << "done printing masks" << endl;

}

void closed_dpmatch_2d::initializeDPMask(int startPoint){

  int ii,jj;
  //cout << "In initializeDPMask with no args"<< endl;
  
  for (ii=0;ii<2*_n;ii++){
    for (jj=0;jj<_m;jj++){
      _cost[ii][jj]=DP_VERY_LARGE_COST;
    }
  }
  for (jj=0;jj<_m;jj++){
    _leftMask[jj]=0;
    _rightMask[jj]=2*_n-1;
  }
}

void closed_dpmatch_2d::findOptimalPath(int startPoint){
  //cout << "Computing optimal path for " << startPoint << endl;
  //fflush(stdout);
  computeDPCosts(startPoint);
  findDPCorrespondence(startPoint);

}

void closed_dpmatch_2d::computeDPCosts(int startPoint){

  //printf("IN COMPUTEDPCOSTS N=%d M=%d S=%d\n",_n,_m,startPoint);
  //fflush(stdout);
  
  int XOFFSET[9] = {0,-1,-1,-2,-1,-3,-2,-3,-1};
  int YOFFSET[9] = {-1,-3,-2,-3,-1,-2,-1,-1,0};

  int start,end,i,ip,j,jp,k;
  double cost;
    
  _cost[startPoint][0]=0;
  //for (sum = 1; sum<_n+_m-1; sum++){
  for (j=0;j<_m;j++){
    //start=max(startPoint,_leftMask[j]);
    start=(startPoint>_leftMask[j]?startPoint:_leftMask[j]);
    //end=min(_rightMask[j],startPoint+_n-1);
    end=(_rightMask[j]<startPoint+_n-1?_rightMask[j]:startPoint+_n-1);
    //cout << start << " " << end << endl;
    for (i=start;i<=end;i++){
      for (k=0;k<9;k++){
        ip=i+XOFFSET[k];
        jp=j+YOFFSET[k];
        if (jp >=0  && ip >= _leftMask[jp] &&  ip <= _rightMask[jp]){
          //printf("(%2d %2d),(%2d %2d) %6.3f\n ",i,j,ip,jp,cost);
          //fflush(stdout);
          cost =_cost[ip][jp]+computeIntervalCost(i,ip,j,jp);
          if (_cost[i][j] > cost){
            _cost[i][j]=cost;
            _map[i][j].first=ip;
            _map[i][j].second=jp;
            
          }
        }
      }
    }
  }
  //printCost();//printMap();
  //cout << "Out of compute dp costs"<< endl;
  //fflush(stdout);

}
          

// Cost of matching the interval [x(i-1),x(i)]  to [y(k),y(j)].
double closed_dpmatch_2d::computeIntervalCost(int i, int ip, 
                                                         int j, int jp){
  //Here the cost is based on lengths of the segments.
  double ds1 = stretchCost (_curve1, i,ip);
        double ds2 = stretchCost (_curve2, j,jp);
  double cost = vcl_fabs(ds1-ds2);
  return cost;
}

void closed_dpmatch_2d::findDPCorrespondence(int startPoint){

  //cout << "In DP Corresp" << endl;
  //fflush(stdout);
  int i,j,ip,jp;
  _finalMap[startPoint].clear();
  _finalMapCost[startPoint].clear();
  _finalCost[startPoint]=_cost[_n+startPoint-1][_m-1];
  
  ip=_n+startPoint-1;
  jp=_m-1;
  i=_n+startPoint-1;
  j=_m-1;
  vcl_pair <int,int> p(ip,jp);
  //_finalMap[startPoint].push_back(p);
  _finalMap[startPoint].insert(_finalMap[startPoint].begin(),p);
  _finalMapCost[startPoint].insert(_finalMapCost[startPoint].begin(),
                                  _cost[p.first][p.second]);
  while (ip > startPoint || jp > 0){
    ip=_map[i][j].first;
    jp=_map[i][j].second;
    vcl_pair <int,int> p(ip,jp);
    _finalMap[startPoint].insert(_finalMap[startPoint].begin(),p);
    _finalMapCost[startPoint].insert(_finalMapCost[startPoint].begin(),
                                    _cost[p.first][p.second]);
    i=ip;
    j=jp;
  }
}

void closed_dpmatch_2d::computeMiddlePaths(int i, int j){
  int k=(i+j)/2;
  if(i<k){
    initializeDPMask(i,j);
    findOptimalPath(k);
    computeMiddlePaths(i,k);
    computeMiddlePaths(k,j);
  }
}

void closed_dpmatch_2d::Match(){
  //cout << "in DP Match" << endl;
  
  //initialize the mask for the match with starting point 0
  initializeDPMask(0);
  //Find optimal Path with starting point 0
  findOptimalPath(0);
  
  //Copy the starting point match (0) to the match from _n
  int N0=_finalMap[0].size();
  vcl_pair <int,int> p;
  _finalMap[_n].clear();
  for (int i=0;i<N0;i++){
    p.first=_finalMap[0][i].first+_n;
    p.second=_finalMap[0][i].second;
    _finalMap[_n].push_back(p);
  }
  vcl_cout <<  "In Match: Done copying  start point"<< vcl_endl;
  computeMiddlePaths(0,_n);
}

void closed_dpmatch_2d::Match(int startPoint){
  //initialize the mask for the match for a specific starting point 
  initializeDPMask(startPoint);
  //Find optimal Path for a specific starting point 
  findOptimalPath(startPoint);
}

/////////////////////  closednew_dpmatch_2d functions /////////////////

closednew_dpmatch_2d::closednew_dpmatch_2d(){
  closed_dpmatch_2d::closed_dpmatch_2d();
  _lambda=1.0;
  _R1 = 1.0;
  _R2 = 1.0;
}
closednew_dpmatch_2d::closednew_dpmatch_2d(bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2){
  int n; 
  //////////////////////// from closed_curve_dpmatch(c1, c2) //////////////////
  _curve1 = c1;
  _curve2 = c2;
  //Append a copy to the first curve 
  for (n=0;n<c1->size();n++)
    _curve1->add_vertex(c1->x(n), c1->y(n));
  
  // caller function should not compute properties of the curves to avoid duplicate computation
  _curve1->computeProperties();
  _curve2->computeProperties();
  _n=c1->size();     //No. of points in original curve
  _m=_curve2->size();

  for (n=0;n<2*_n;n++){
    vcl_vector<double> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);
  }
  
  _leftMask.insert(_leftMask.begin(),_m,0);
  _rightMask.insert(_rightMask.begin(),_m,2*_n-1);
  

  for(n=0;n<_n+1;n++){
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(1,tmp3);
    _finalMap.push_back(tmp2);
  }
  vcl_vector<double> tmp5(_n+1,DP_VERY_LARGE_COST);
  _finalCost = tmp5;

  for (n=0;n<_n+1;n++){
    vcl_vector<double> tmp1(_m,DP_VERY_LARGE_COST);
    _finalMapCost.push_back(tmp1);
  }
  ////////////////////////////////////////////////////
  _R1 = 15.0;
  _R2 = 0.0;
  _lambda = 1.0;
}
closednew_dpmatch_2d::closednew_dpmatch_2d(bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2,
                                            double R1,double R2, //vcl_vector<double> lambda, 
              double lambda,
                                            int numLenElems){
  
  int n; 
  _curve1 = c1;
  _curve2 = c2;
  _n=c1->size(); //No. of points in original curve
  _m=_curve2->size();
  
  //Append a copy to the first curve 
  for (n=0;n<_n;n++)
    _curve1->add_vertex(c1->x(n), c1->y(n));

  _curve1->computeProperties();
  _curve2->computeProperties();
  
  for (n=0;n<2*_n;n++){
    vcl_vector<double> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);
  }
  
  _leftMask.insert(_leftMask.begin(),_m,0);
  _rightMask.insert(_rightMask.begin(),_m,2*_n-1);

  for(n=0;n<_n+1;n++){
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(1,tmp3);
    _finalMap.push_back(tmp2);
  }

  vcl_vector<double> tmp5(_n+1,DP_VERY_LARGE_COST);
  _finalCost = tmp5;

  for (n=0;n<_n+1;n++){
    vcl_vector<double> tmp1(_m,DP_VERY_LARGE_COST);
    _finalMapCost.push_back(tmp1);
  }

  _R1 = R1;
  _R2 = R2;
  _lambda =lambda;
  _numLenElems=numLenElems;
}
// Cost of matching the  interval [x(l),x(i)] to the interval [y(k),y(j)].
double closednew_dpmatch_2d::computeIntervalCost(int i, int ip, int j, int jp){
  double cost,dF=0,dK=0;
  double ds1, ds2, dt1, dt2;

  ds1 = stretchCost (_curve1, i,ip);
        ds2 = stretchCost (_curve2, j,jp);

  dF = vcl_fabs(ds1-_lambda*ds2);
  
  dt1 = bendCost(_curve1, i,ip);
        dt2 = bendCost(_curve2, j,jp);
  dK = vcl_fabs(dt1-dt2);
  
  cost = dF+_R1*dK;
  return cost;
}
