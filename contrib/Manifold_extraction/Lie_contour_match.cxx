#include "Lie_contour_match.h"
#include "Lie_contour_match_sptr.h"
#include <vcl_cmath.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>

#include <dbsol/dbsol_interp_curve_2d.h>

//value in (-Pi,Pi]
double Lie_curve_fixAngleMPiPi (double a)
{
  if (a <= -vnl_math::pi) return a+2*vnl_math::pi;
  if (a >   vnl_math::pi) return a-2*vnl_math::pi;
  return a;
}

//Does a1-a2, value in (-Pi,Pi]
double Lie_curve_angleDiff (double a1, double a2)
{
   return(Lie_curve_fixAngleMPiPi(a1-a2));
}

double  Lie_curve_maxof (double a, double b, double c){
  if (a>b && a>c)
    return a;  
  else if (b>c && b>a)
    return b;
  else if (c>b && c>a)
    return c;
  else
    return a;
}

Lie_contour_match::Lie_contour_match(){
  _n1 = 0;
  _n2 = 0;
  setTemplateSize(3);  
}

Lie_contour_match::Lie_contour_match(const dbsol_interp_curve_2d_sptr c1, 
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


void Lie_contour_match::setTemplateSize (int temp_size) {
  
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
    _template_size = 9;
    
    int dummyX[9] = {-1, 0,-1,-1,-2,-2,-3,-1,-3};
    int dummyY[9] = {-1,-1, 0,-2,-1,-3,-2,-3,-1};
    
    for (int i = 0; i<9; i++) {
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

void Lie_contour_match::printCost(){
  int i,j;
  vcl_cout << "Cost Matrix" << vcl_endl;
  for (i = 0; i<_n1; i++){
    for (j = 0; j<_n2; j++){
      vcl_printf("%6.3f ",_cost[i][j]);
    }
    vcl_printf("\n");
  }
}

void Lie_contour_match::writeCost(vcl_string fname){
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

void Lie_contour_match::printMap(){
  int i,j;
  vcl_printf("Map Matrix\n");
  for (i = 0; i<_n1; i++){
    for (j = 0; j<_n2; j++){
      vcl_printf("(%2d,%2d) ", _map[i][j].first, _map[i][j].second);
    }
    vcl_printf("\n");
  }
} 

void Lie_contour_match::initializeDPMask1(){

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

void Lie_contour_match::initializeDPMask2(int s1, int s2) {
  
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

void Lie_contour_match::computeDPCosts(int startPoint){
  
  int start,end,i,ip,j,jp,k;
  double cost;
  
  _cost[startPoint][0]=0;
  //for (sum = 1; sum<_n+_m-1; sum++){
  for (j=0;j<_n2;j++){
    start=(startPoint>_leftMask[j] ? startPoint : _leftMask[j]);
    end=(_rightMask[j]<startPoint+_n1-1 ? _rightMask[j] : startPoint+_n1-1);

    // vcl_cout << "end: " << end << vcl_flush;
    for (i=start;i<=end;i++){
      for (k=0;k<_template_size;k++){
              ip=i+XOFFSET[k];
              jp=j+YOFFSET[k];
              if (jp >=0  && ip >= _leftMask[jp] &&  ip <= _rightMask[jp]){
                cost =_cost[ip][jp]+computeLieIntervalCost(i,ip,j,jp);
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

void Lie_contour_match::findDPCorrespondence(int startPoint){

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

void Lie_contour_match::findOptimalPath(int startPoint){
  //cout << "Computing optimal path for " << startPoint << endl;
  //fflush(stdout);
  computeDPCosts(startPoint); 
  findDPCorrespondence(startPoint);
}

void Lie_contour_match::computeMiddlePaths(int i, int j){
  int k=(i+j)/2;
  if(i<k){
    initializeDPMask2(i,j);
    findOptimalPath(k);
    computeMiddlePaths(i,k);
    computeMiddlePaths(k,j);
  }
}

void Lie_contour_match::Match(){   
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

}

//: compute arclengths and tangents at even samples of both curves to avoid their recomputation 
  //  at each interval cost computation
void Lie_contour_match::computeCurveProperties() {
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

// Lie Cost for matching the interval [x(ip),x(i)]  to [y(jp),y(j)].
double Lie_contour_match::computeLieIntervalCost(int i, int ip, int j, int jp){
  
  double s1 = _lengths_curve1[i];
  double s1_p = _lengths_curve1[ip];

  double s2 = _lengths_curve2[j];
  double s2_p = _lengths_curve2[jp];
  
  //Here the cost is based on lengths of the segments.
  double ds1 = vcl_fabs(s1-s1_p);
  double ds2 = vcl_fabs(s2-s2_p);
  
  double dF;
  
    if ((ds1 > 1E-10) && (ds2 > 1E-10))
        dF = ((ds2 > ds1)? vcl_log(ds2/ds1) : vcl_log(ds1/ds2));
      
    else if (ds1 < 1E-10)
        dF = vcl_log(ds2);
    else if (ds2 < 1E-10)
        dF = vcl_log(ds1);
    else
        dF = 0;

   
    
    s1 = vcl_fmod(s1, _curve1->length());
    s1 = (s1 == 0 ? _curve1->length() : s1);

    s1_p = vcl_fmod(s1_p, _curve1->length());
    s1_p = (s1_p == 0 ? _curve1->length() : s1_p);

    s2 = vcl_fmod(s2, _curve1->length());
    s2 = (s2 == 0 ? _curve1->length() : s2);

    s2_p = vcl_fmod(s2_p, _curve1->length());
    s2_p = (s2_p == 0 ? _curve1->length() : s2_p);

  

 vsol_point_2d_sptr  SP_C1 =  _curve1->point_at(s1);
 vsol_point_2d_sptr  EP_C1 = _curve1->point_at(s1_p);

 vsol_point_2d_sptr SP_C2 =  _curve2->point_at(s2);
 vsol_point_2d_sptr EP_C2 = _curve2->point_at(s2_p);
 
double angle_1,angle_2;
double m_C1 = 0,x_diff_C1 = SP_C1->x() - EP_C1->x(),y_diff_C1 = SP_C1->y() - EP_C1->y();

if (vcl_fabs(x_diff_C1) > 1E-5)
    {
  angle_1 = vcl_atan2(y_diff_C1,x_diff_C1);
    }
else if ((vcl_fabs(x_diff_C1) < 1E-5) && (vcl_fabs(y_diff_C1) > 1E-5))
    {
    if (x_diff_C1 < 0)
    angle_1 = -0.5*(vnl_math::pi);
    else
        angle_1 = 0.5*(vnl_math::pi);
    }
else
    {
    angle_1 = 0;
    }

double m_C2 = 0,x_diff_C2 = SP_C2->x() - EP_C2->x(),y_diff_C2 = SP_C2->y() - EP_C2->y();

if (vcl_fabs(x_diff_C2) > 1E-5)
    {
  angle_2 = vcl_atan2(y_diff_C2,x_diff_C2);
    }
else if ((vcl_fabs(x_diff_C2) < 1E-5) && (vcl_fabs(y_diff_C2) > 1E-5))
    {
    if (x_diff_C2 < 0 )
    angle_2 = -0.5*(vnl_math::pi);
    else 
        angle_2 = 0.5*(vnl_math::pi);
    }
else
    {
    angle_2 = 0;
    }

double dL1,dL2;

if (ds1 > 1E-10)
dL1 = vcl_log(ds1);
else
dL1 = 0;

if (ds2 > 1E-10)
dL2 = vcl_log(ds2);
else
dL2 = 0;

double cost = 0,t1 = 0,t2 = 0;

if ((angle_2 != 0) || (dL2 != 0))
t1 = (angle_1*angle_2 + dL1*dL2)/(angle_2*angle_2 + dL2*dL2);

if ((angle_1 != 0) || (dL1 != 0))
t2 = (angle_1*angle_2 + dL1*dL2)/(angle_1*angle_1 + dL1*dL1);

double dK = angle_1 - angle_2;
double cost1 = 0,cost2 = 0;

cost = vcl_sqrt((angle_2 - angle_1)*(angle_2 - angle_1) + (dL2 - dL1)*(dL2 - dL1));
//cost1 = vcl_sqrt((t1*angle_2 - angle_1)*(t1*angle_2 - angle_1) + (t1*dL2 - dL1)*(t1*dL2 - dL1));
//cost2 = vcl_sqrt((t2*angle_1 - angle_2)*(t2*angle_1 - angle_2) + (t2*dL1 - dL2)*(t2*dL1 - dL2));
//
//cost = (cost1 > cost2 ? cost2 : cost1);
//
//if (cost == 0)
//cost = vcl_sqrt(dF*dF + dK*dK);

return cost;

 // double dt1 = Lie_curve_angleDiff (_tangents_curve1[i], _tangents_curve1[ip]);
 // double dt2 = Lie_curve_angleDiff (_tangents_curve2[j], _tangents_curve2[jp]);
 // // double dK = vcl_fabs(dt1-dt2);

 //double dK = angle_1 - angle_2;

 // return vcl_sqrt(dF*dF + dK*dK);
}

