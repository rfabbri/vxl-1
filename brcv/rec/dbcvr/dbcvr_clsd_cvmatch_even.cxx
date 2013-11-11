#include "dbcvr_clsd_cvmatch_even.h"
#include <vcl_cmath.h>
#include <vcl_cstdio.h>

#include <dbsol/dbsol_interp_curve_2d.h>

dbcvr_clsd_cvmatch_even::dbcvr_clsd_cvmatch_even(){
  _n1 = 0;
  _n2 = 0;
  setTemplateSize(3);  
}

dbcvr_clsd_cvmatch_even::dbcvr_clsd_cvmatch_even(const dbsol_interp_curve_2d_sptr c1, 
                                                 const dbsol_interp_curve_2d_sptr c2, 
                                                 int n1, 
                                                 int n2, 
                                                 double R, 
                                                 int template_size){

  // assure shorter curve is _curve1
  if (c1->length() > c2->length()) {   // swap
    // TODO using original curves might cause problems --> check!!
    _curve1 = c2;
    _curve2 = c1;

    _n1=n2;    
    _n2=n1;    
  
  } else {
    _curve1 = c1;
    _curve2 = c2;

    _n1=n1;     //Divide curve1 into n1 evenly spaced intervals
    _n2=n2;     //Divide curve2 into n2 evenly spaced intervals

  }

  //       c2
  //       ^
  //       |
  // L2    -----|
  //       |    |
  //       |    |
  //       |  / |
  //       |/   |
  //       -----'-->  c1           keep c2 as it is and try different starting points on c1
  //            L1

  L1 = _curve1->length();
  L2 = _curve2->length();
  
  // inputs should be closed curves, last interval is the preceding interval of first data point
  // i.e. the interval between last and first data points

  //assert(*(_curve1->point_at(0)) == *(_curve1->point_at(L1)));
  //assert(*(_curve2->point_at(0)) == *(_curve2->point_at(L2)));

  _delta_s1 = L1/_n1;
  _delta_s2 = L2/_n2; // i.e. curves will be sampled with this arclength 
                                     // _n1*_delta_s1 = L1
                                     
  // CAUTION!!:
  // curves are closed and dbsol_interp_curve_2d assumes that the angle of the point
  // for s = 0 is read from s = L, due to the interval structure used in that class.
  
  // NO NEED for duplication
  //: duplicate curve 1 to capture cyclic nature of closed curves
  //for (n=0;n<_n1;n++)
  //  _curve1->add_vertex(c1->x(n), c1->y(n));

  for (int n=0;n<2*_n1;n++){    // 2*n1*_delta_s1 = 2*L1
    vcl_vector<double> tmp1(_n2,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_n2,tmp3);
    _map.push_back(tmp2);
    
    vcl_pair <int,int> tmp4(0,0);
    vcl_vector< vcl_pair <int,int> > tmp5(_n1+_n2,tmp4);
    _finalMap.push_back(tmp5);
  }
  
  _leftMask.insert(_leftMask.begin(),_n2,0);
  //_rightMask.insert(_rightMask.begin(),_n2,2*_n1-1);
  _rightMask.insert(_rightMask.begin(),_n2,2*_n2-1);
  _finalCost.insert(_finalCost.begin(),2*_n1,DP_VERY_LARGE_COST);

  _R = R;
  setTemplateSize(template_size);  
  _normalized_stretch_cost = false;
}

void dbcvr_clsd_cvmatch_even::setTemplateSize (int temp_size) {
  
  XOFFSET.clear();
  YOFFSET.clear();
  
  if (temp_size == 1) {  // 1x1 template
    _template_size = 3;

    int dummyX[3] = {-1, 0,-1};
    int dummyY[3] = {-1,-1, 0};
    
    for (int i = 0; i<3; i++) {
      XOFFSET.push_back(dummyX[i]); 
      YOFFSET.push_back(dummyY[i]); 
    }

  } else if (temp_size == 3) { // 3x3 template
    _template_size = 11;
    
    int dummyX[11] = {-3,-2,-1, 0,-1,-1,-2,-2,-3,-1,-3};
    int dummyY[11] = {-3,-2,-1,-1, 0,-2,-1,-3,-2,-3,-1};
    
    for (int i = 0; i<11; i++) {
      XOFFSET.push_back(dummyX[i]); 
      YOFFSET.push_back(dummyY[i]); 
    }
  
  } else if (temp_size == 5) { // 5x5 template
    _template_size = 21;
    
    int dummyX[21] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5};
    int dummyY[21] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4};
    
    for (int i = 0; i<21; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  
  } else if (temp_size == 11) { // 11x11 template
    _template_size = 93;
    
    int dummyX[93] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5, //5x5
    -1,-5,-6,-6, -1,-2,-3,-4,-5,-6,-7,-7,-7,-7,-7,-7, -1,-3,-5,-7,-8,-8,-8,-8, -1,-2,-4,-5,-7,-8,-9,-9,-9,-9,-9,-9, //9x9
     -1, -3, -7, -9,-10,-10,-10,-10, -1, -2, -3, -4, -5, -6, -7, -8, -9,-10,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11, //11x11
     -1, -5, -7, -11,-12,-12,-12,-12};
    int dummyY[93] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4, //5x5
    -6,-6,-5,-1, -7,-7,-7,-7,-7,-7,-1,-2,-3,-4,-5,-6, -8,-8,-8,-8,-1,-3,-5,-7, -9,-9,-9,-9,-9,-9,-1,-2,-4,-5,-7,-8, //9x9
    -10,-10,-10,-10, -1, -3, -7, -9,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11, -1, -2, -3, -4, -5, -6, -7, -8, -9,-10, //11x11
    -12,-12,-12,-12, -1, -5, -7, -11};

    for (int i = 0; i<93; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
}

void dbcvr_clsd_cvmatch_even::printCost(){
  int i,j;
  vcl_cout << "Cost Matrix" << vcl_endl;
  for (i = 0; i<_n1; i++){
    for (j = 0; j<_n2; j++){
      vcl_printf("%6.3f ",_cost[i][j]);
    }
    vcl_printf("\n");
  }
}

void dbcvr_clsd_cvmatch_even::writeCost(vcl_string fname){
  vcl_FILE *fp=vcl_fopen(fname.c_str(),"w");
  int i,j;
  double c;
  for (i = 0; i<_n1; i++){
    for (j = 0; j<_n2; j++){
      c=_cost[i][j];
      vcl_fwrite(&c,sizeof(double),1,fp);
    }
  }
  vcl_fclose(fp);
}

void dbcvr_clsd_cvmatch_even::printMap(){
  int i,j;
  vcl_printf("Map Matrix\n");
  for (i = 0; i<_n1; i++){
    for (j = 0; j<_n2; j++){
      vcl_printf("(%2d,%2d) ", _map[i][j].first, _map[i][j].second);
    }
    vcl_printf("\n");
  }
} 

void dbcvr_clsd_cvmatch_even::initializeDPMask1(){

  int ii,jj;
  
  for (ii=0;ii<2*_n1;ii++){
    for (jj=0;jj<_n2;jj++){
      _cost[ii][jj]=DP_VERY_LARGE_COST;
    }
  }
  for (jj=0;jj<_n2;jj++){
    _leftMask[jj]=0;
    _rightMask[jj]=2*_n2-1;
  }
}

void dbcvr_clsd_cvmatch_even::initializeDPMask2(int s1, int s2) {
  
  int ci=0,cj=1;
  int Ni=_finalMap[s1].size(); //Size of optimal paths i and j
  int Nj=_finalMap[s2].size();
  int ii;
  int jj;
  int x1,x2,y1,y2;
  int count;

  //intPair *Pi, *Pj;
  //intPair *PiR, *PjR;
 
  int NDi;//Size of discrete paths i and j
  int NDj;
  int start,end;
  int n1,n2;

  n1=_n1;
  n2=_n2;

  vcl_pair <int,int> tmp3(0,0);
  vcl_vector< vcl_pair <int,int> > Pi(n1+n2,tmp3);
  vcl_vector< vcl_pair <int,int> > Pj(n1+n2,tmp3);
  vcl_vector< vcl_pair <int,int> > PiR(n1+n2,tmp3);
  vcl_vector< vcl_pair <int,int> > PjR(n1+n2,tmp3);
  
  count=0;
  for (ii=0;ii<Ni-1;ii++){
    x1=_finalMap[s1][ii].first;x2=_finalMap[s1][ii+1].first;
    y1=_finalMap[s1][ii].second;y2=_finalMap[s1][ii+1].second;
    PiR[count].first=_finalMap[s1][ii].first;
    PiR[count].second=_finalMap[s1][ii].second;
    count++;
    for (jj=y1+1;jj<y2;jj++){
      PiR[count].first=x1;
      PiR[count].first=jj;   // ozge: ??
      count++;
    }
    for (jj=x1+1;jj<x2;jj++){
      PiR[count].first=jj;
      PiR[count].first=y2;
      count++;
    }
  }
  
  PiR[count].first=_finalMap[s1][Ni-1].first;
  PiR[count].second=_finalMap[s1][Ni-1].second;
  count++;
  NDi=count;
  for (count=NDi-1;count>=0;count--)
    Pi[NDi-count]=PiR[count];
  

  //Right path
  count=0;
  for (ii=0;ii<Nj-1;ii++){
    x1=_finalMap[s2][ii].first;x2=_finalMap[s2][ii+1].first;
    y1=_finalMap[s2][ii].second;y2=_finalMap[s2][ii+1].second;
    PjR[count].first=_finalMap[s2][ii].first;
    PjR[count].second=_finalMap[s2][ii].second;
    count++;
    for (jj=x1+1;jj<x2;jj++){
      PjR[count].first=jj;
      PjR[count].second=y1;
      count++;
    }
    for (jj=y1+1;jj<y2;jj++){
      PjR[count].first=x2;
      PjR[count].second=jj;
      count++;
    }
  }
  PjR[count].first=_finalMap[s2][Nj-1].first;
  PjR[count].second=_finalMap[s2][Nj-1].second;
  count++;
  NDj=count;
  for (count=NDj-1;count>=0;count--)
    Pj[NDj-count]=PjR[count];

/*   if (s1==3 && s2==6){ */
/*     for (count=0;count<NDi;count++) */
/*       printf("%3d %3d\n",Pi[count].first,Pi[count].second); */
/*     for (count=0;count<NDj;count++) */
/*       printf("%3d %3d\n",Pj[count].first,Pj[count].second); */
/*   } */

  
  for (jj=0;jj<_n2;jj++){
    while(ci <NDi && PiR[ci].second<jj){
      ci++;
    }
    start = PiR[ci].first;
    while(cj < NDj && PjR[cj].second < jj+1){
      cj++;
    }
    end = PjR[cj-1].first;
    _leftMask[jj]=start;
    _rightMask[jj]=end;
    //cout << jj <<" Start= " << start << " End= " << end << " " << cj-1 << endl;
    for (ii=0;ii<=2*_n1-1;ii++)
      _cost[ii][jj]=DP_VERY_LARGE_COST;
  }

}

void dbcvr_clsd_cvmatch_even::computeDPCosts(int startPoint){
  
  int start,end,i,ip,j,jp,k;
  double cost;
  
  _cost[startPoint][0]=0;
  //for (sum = 1; sum<_n+_m-1; sum++){
  for (j=0;j<_n2;j++){
    start=(startPoint>_leftMask[j] ? startPoint : _leftMask[j]);
    end=(_rightMask[j]<startPoint+_n1-1 ? _rightMask[j] : startPoint+_n1-1);
    for (i=start;i<=end;i++){
      for (k=0;k<_template_size;k++){
              ip=i+XOFFSET[k];
              jp=j+YOFFSET[k];
              if (jp >=0  && ip >= _leftMask[jp] &&  ip <= _rightMask[jp]){
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
}

void dbcvr_clsd_cvmatch_even::findDPCorrespondence(int startPoint){

  int i,j,ip,jp;
  int count;

  vcl_pair <int,int> tmp3(0,0);
  vcl_vector< vcl_pair <int,int> > tmpMap(_n1+_n2+1,tmp3);
  //tmpMap=(intPair*)calloc(d->n1+d->n2+1,sizeof(intPair));

  _finalCost[startPoint]=_cost[_n1+startPoint-1][_n2-1];
  
  ip=_n1+startPoint-1;
  jp=_n2-1;
  i=_n1+startPoint-1;
  j=_n2-1;

  count=0;
  tmpMap[count].first=ip;
  tmpMap[count].second=jp;
  count++;

  while (ip > startPoint || jp > 0){
    ip=_map[i][j].first;
    jp=_map[i][j].second;
    tmpMap[count].first=ip;
    tmpMap[count].second=jp;
    count++;
    i=ip;
    j=jp;
  }
  _finalMap[startPoint].clear();
  vcl_pair <int,int> p;
  //_finalMapSize[startPoint]=count;
  for (i=0;i<count;i++){
    p.first = tmpMap[count-1-i].first;
    p.second = tmpMap[count-1-i].second;
    _finalMap[startPoint].push_back(p);
    //_finalMap[startPoint][i].first=tmpMap[count-1-i].first;
    //_finalMap[startPoint][i].second=tmpMap[count-1-i].second;
  }

  tmpMap.clear();
}

void dbcvr_clsd_cvmatch_even::findOptimalPath(int startPoint){
  //cout << "Computing optimal path for " << startPoint << endl;
  //fflush(stdout);
  computeDPCosts(startPoint); 
  findDPCorrespondence(startPoint);
}

void dbcvr_clsd_cvmatch_even::computeMiddlePaths(int i, int j){
  int k=(i+j)/2;
  if(i<k){
    initializeDPMask2(i,j);
    findOptimalPath(k);
    computeMiddlePaths(i,k);
    computeMiddlePaths(k,j);
  }
}

void dbcvr_clsd_cvmatch_even::Match(){   
  //cout << "in DP Match" << endl;
  
  //: compute arclengths and tangents at even samples of both curves to avoid their recomputation 
  //  at each interval cost computation
  computeCurveProperties();

  //initialize the mask for the match with starting point 0
  initializeDPMask1();
  //Find optimal Path with starting point 0
  findOptimalPath(0);
  
  //Copy the starting point match (0) to the match from _n
  int N0=_finalMap[0].size();
  vcl_pair <int,int> p;
  _finalMap[_n1].clear();
  _finalCost[_n1]=_finalCost[0];
  for (int i=0;i<N0;i++){
    p.first=_finalMap[0][i].first+_n1;
    p.second=_finalMap[0][i].second;
    _finalMap[_n1].push_back(p);
  }
  //vcl_cout <<  "In Match: Done copying  start point"<< vcl_endl;
  computeMiddlePaths(0,_n1);

#if 0
  double minCost = _finalCost[0] ;
  int k_min = 0;
  for (int k=1;k<_n1;k++){
    if (_finalCost[k] < minCost) {
      minCost = _finalCost[k];
      k_min = k;
    }
  }

  vcl_ofstream fpoo; 
  fpoo.open("D:\\contours\\Mpeg-7\\temp_even.out", vcl_ios::app);

  fpoo << 2*_n1 << " " << _n2 << "\n";
  for (int i = 0; i<2*_n1; i++) {
    for (int j = 0; j<_n2; j++) {   // -1 since (0,0) will go to CD
      fpoo << i*_delta_s1 << " ";
      fpoo << j*_delta_s2 << vcl_endl;
    }
  }

  //vcl_cout << "k_min was: " << k_min << " changhed to 0\n";
  //k_min = 0;

  N0=_finalMap[k_min].size();
  fpoo << N0 << "\n";
  for (int i=0; i<N0 ; i++) {
    int ii =_finalMap[k_min][i].first;
    int jj =_finalMap[k_min][i].second;
    fpoo << ii*_delta_s1 << " ";
    fpoo << jj*_delta_s2 << vcl_endl;

    double s1 = ii*_delta_s1;
    double s2 = jj*_delta_s2;

    
    s1 = vcl_fmod(s1, L1);
    s1 = (s1 == 0 ? L1 : s1);

    s2 = (s2 == 0 ? L2 : s2);

    double s1_prev = s1 - _delta_s1;
    if (s1_prev < 0) s1_prev = L1 + s1_prev;
  
    s1_prev = vcl_fmod(s1_prev, L1);
    s1_prev = (s1_prev == 0 ? L1 : s1_prev);

    double s2_prev = s2 - _delta_s2;
    if (s2_prev < 0) s2_prev = L2 + s2_prev;

    s2_prev = vcl_fmod(s2_prev, L2);
    s2_prev = (s2_prev == 0 ? L2 : s2_prev);


    fpoo << curve_angleDiff (_curve1->tangent_angle_at(s1), _curve1->tangent_angle_at(s1_prev)) << " ";
    fpoo << curve_angleDiff (_curve2->tangent_angle_at(s2), _curve2->tangent_angle_at(s2_prev)) << vcl_endl;
  }

  fpoo.close();
#endif
}

//: compute arclengths and tangents at even samples of both curves to avoid their recomputation 
  //  at each interval cost computation
void dbcvr_clsd_cvmatch_even::computeCurveProperties() {
  for (int i = 0; i<2*_n1; i++) 
    _lengths_curve1.push_back(i*_delta_s1);
    
  for (int i = 0; i<2*_n1; i++) {
    double s1 = _lengths_curve1[i];
    s1 = vcl_fmod(s1, L1);
    s1 = (s1 == 0 ? L1 : s1);
    _tangents_curve1.push_back(_curve1->tangent_angle_at(s1));
  }

  for (int i = 0; i<_n2; i++) 
    _lengths_curve2.push_back(i*_delta_s2);

  for (int i = 0; i<_n2; i++) {
    double s2 = _lengths_curve2[i];
    s2 = (s2 == 0 ? L2 : s2);
    _tangents_curve2.push_back(_curve2->tangent_angle_at(s2));
  }
  
}

// Cost of matching the interval [x(ip),x(i)]  to [y(jp),y(j)].
double dbcvr_clsd_cvmatch_even::computeIntervalCost(int i, int ip, int j, int jp){
  
  double s1 = _lengths_curve1[i];
  double s1_p = _lengths_curve1[ip];

  double s2 = _lengths_curve2[j];
  double s2_p = _lengths_curve2[jp];
  
  //Here the cost is based on lengths of the segments.
  double ds1 = s1-s1_p;
  double ds2 = s2-s2_p;
  
  double dF;
  if (_normalized_stretch_cost) {
    if (ds1+ds2 > 1E-5)
      dF = vcl_pow(ds1-ds2,2)/(ds1+ds2);
    else dF = 0;
  } else dF = vcl_fabs(ds1-ds2);

  double dt1 = curve_angleDiff (_tangents_curve1[i], _tangents_curve1[ip]);
  double dt2 = curve_angleDiff (_tangents_curve2[j], _tangents_curve2[jp]);

  double dK = vcl_fabs(dt1-dt2);
  
  return dF + _R*dK;
}
