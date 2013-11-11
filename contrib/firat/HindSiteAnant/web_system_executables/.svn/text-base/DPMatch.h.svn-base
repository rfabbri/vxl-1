// CAN

#ifndef __TBS_DPMATCH_DEF__
#define __TBS_DPMATCH_DEF__

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#define DP_VERY_LARGE_COST 1E12

template <class curveType,class floatType>
class DPMatch
{
public:
  DPMatch();
  //Constructor
  DPMatch(curveType &c1,curveType &c2,int nstart, int mstart);
  ~DPMatch(){};
  //access functions
  floatType finalCost(){return _finalCost;};
  floatType finalCost(floatType cost){_finalCost=cost; return _finalCost;};
  vcl_vector < vcl_pair <int,int> > finalMap(){return _finalMap;};
  vcl_vector< floatType > finalMapCost(){return _finalMapCost;};
  int n(){return _n;};
  int m(){return _m;};

  curveType curve1(){return _curve1;};
  curveType curve2(){return _curve2;};
  void match();
  vcl_pair<int,int> Start(){vcl_pair<int,int> tmp(_nstart,_mstart); return tmp;}
  vcl_pair<int,int> End(){vcl_pair<int,int> tmp(_nend,_mend); return tmp;}
  double length;
protected:
  //Data
  curveType _curve1;
  curveType _curve2;
  vcl_vector< vcl_vector<floatType> > _cost;
  vcl_vector< vcl_vector< vcl_pair <int,int> > > _map;
  vcl_vector< vcl_vector< vcl_pair <floatType,floatType> > > _angle;
  vcl_vector< vcl_pair <int,int> > _finalMap;
  floatType _finalCost;
  vcl_vector< floatType > _finalMapCost;
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
  floatType _lambda;
  
  //Functions
  void initializeDPCosts();
  void computeDPCosts();
  void findDPCorrespondence();
  //Altered from virtual function to real function from NewDPMatch
  floatType computeIntervalCost(int i, int ip, int j, int jp, int ri, int rj, int C_FLAG);
  // CAN DEBUG
  void write_dp_costs(vcl_string fname);
};

//###########################
template <class curveType,class floatType>
DPMatch<curveType,floatType>::DPMatch() 
{
  vcl_vector< vcl_vector<floatType> > a;
  vcl_vector< vcl_vector< vcl_pair <int,int> > > b;
  vcl_vector< vcl_pair <int,int> > c;
  vcl_vector< floatType > d;

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

//Code added to combine DPMatch and NewDPMatch
//Constructor from NewDPMatch
template <class curveType,class floatType>
DPMatch<curveType,floatType>::DPMatch(curveType &c1, curveType &c2, int nstart, int mstart)
{  
  vcl_vector< vcl_vector<floatType> > a;
  vcl_vector< vcl_vector< vcl_pair <int,int> > > b;
  vcl_vector< vcl_pair <int,int> > c;
  vcl_vector< floatType > d;

  _curve1 = c1;
  _curve2 = c2;
  _cost = a;
  _map = b;
  _finalMap = c;
  _finalMapCost = d;

  int n;   
  _flip = false;
  _n=_curve1.num_fine_points();
  _m=_curve2.num_fine_points();
  
  // initialize the DP table cost array, backlinking pointers, angle value array
  for (n=0;n<_n;n++)
  {
    vcl_vector<floatType> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);

    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);

    vcl_pair<floatType,floatType> tmp4(0,0);
    vcl_vector <vcl_pair<floatType,floatType> > tmp5(_m,tmp4);
    _angle.push_back(tmp5);
  }
 
  _finalCost = DP_VERY_LARGE_COST;
  _R1 = 15.0;
  _lambda =1.0;
  _numLenElems=1;
  _nstart=nstart;
  _mstart=mstart;
  _start_flag=false;
  _end_flag=false;
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::initializeDPCosts()
{
  double l10,l20;

  l10=_curve1.arcLength(_nstart);
  l20=_curve2.arcLength(_mstart);

  double c0=l10/l20;

  if(c0<TH_L || c0>TH_U) 
    _start_flag=true; //_cost[0][0]=DP_VERY_LARGE_COST; //(c0+1/c0)*(l10+l20)/20;
  //else
  _cost[0][0]=0;//-2/(c0+1/c0) * (l10+l20)/20;
     
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::computeDPCosts()
{
  // JONAH
  int XOFFSET[6] = {-1,-1,-2,-2,-1,-3};
  int YOFFSET[6] = {-1,-2,-1,-2,-3,-1};

  // CAN
//  int XOFFSET[9] = {-1, 0,-1,-1,-2,-2,-3,-1,-3};
//  int YOFFSET[9] = {-1,-1, 0,-2,-1,-3,-2,-3,-1};

  int i,ip,j,jp,k,r;
  floatType cost;
  float min_cost;
  int ii,iip,jj,jjp;

  double tlc;

  _nend=0;
  _mend=0;

  double ivc, pvc;
  int last_good_row = -1;
  int start=1;

  min_cost=0.0; r=1; i=1; j=r;
  while(r<_n && r<_m) 
  {  
    ii=i+_nstart;if(ii>_n-1) ii-=_n;
    jj=j+_mstart;if(jj>_m-1) jj-=_m;
    tlc=fabs(_curve1.merge_length(_nstart,ii)-_curve2.merge_length(_mstart,jj))*0.1;

    // JONAH
    for (k=0;k<4;k++)
    // CAN
//    for (k=0;k<9;k++)
    { 
      ip=i+XOFFSET[k];
      jp=j+YOFFSET[k];

      iip=ip+_nstart;if(iip>_n-1) iip-=_n;
      jjp=jp+_mstart;if(jjp>_m-1) jjp-=_m;

      if (ip>=0 &&  jp>=0) 
      { 

        if (ip==0 && jp==0)
          ivc=computeIntervalCost(ii,iip,jj,jjp,ip,jp,0);
        else
          ivc = computeIntervalCost(ii,iip,jj,jjp,ip,jp,1);

        pvc = _cost[ip][jp];
        cost = pvc+ivc;

        if (cost<_cost[i][j])
        {
          _cost[i][j]=cost;
          _map[i][j].first=ip; 
          _map[i][j].second=jp;
          _angle[i][j].first=_curve1.merge_angle(iip,ii);
          _angle[i][j].second=_curve2.merge_angle(jjp,jj);
        }  
      }
    }

    if(_cost[i][j]+tlc<min_cost) 
    {
      min_cost = static_cast<float>(_cost[i][j]+tlc);
      last_good_row = r;
      _nend=i;
      _mend=j;
    }
    else if(_cost[i][j]-min_cost<10.0)
      last_good_row = r;

    if(i<r) i++;
    else j--;

    if(j<start)
    {
      r++;
      if(r-last_good_row > 2)
        return;
      
      j=r;
      start=static_cast<int>(ceil((double)r/2.0));
      i=start;
    }

    if(i>_n-1) i=_n-1;
    if(j>_m-1) j=_m-1;  
  }  
}


//From NewDPMatch.cpp:

// Cost of matching the  interval [x(l),x(i)] to the interval [y(k),y(j)].
template <class curveType,class floatType>
floatType DPMatch<curveType,floatType>::computeIntervalCost(int i, int ip, int j, int jp, int ri,int rj, int flag)
{
  floatType cost,dF=0,dK=0;

  double l1,l2,tLen,c,l10,l20,c01,c02,angle1,angle2,ang,adj; 

  angle1=0.0;
  angle2=0.0;
  ang=0.0;
  //JONAH
  l1=_curve1.merge_length(ip,i);
  l2=_curve2.merge_length(jp,j);
  // CAN
  //l1=_curve1.total_length(ip,i);
  //l2=_curve2.total_length(jp,j);

  c=(l1/l2+l2/l1)/2.0;
  
  l10=_curve1.total_length(ip,i);
  c01=l10/l1;
  
  l20=_curve2.total_length(jp,j);
  c02=l20/l2;
  
  if(flag) 
  {   
    //JONAH
    angle1 = angleDiff(_curve1.merge_angle(ip,i),_angle[ri][rj].first);
    angle2 = angleDiff(_curve2.merge_angle(jp,j),_angle[ri][rj].second);
    //CAN
    //angle1 = angleDiff(_curve1.angle(i),_curve1.angle(ip));
    //angle2 = angleDiff(_curve2.angle(j),_curve2.angle(jp));

    ang = fabs(angle1-angle2);
  }
  
  tLen=(_curve1.merge_length(_nstart,i) + _curve2.merge_length(_mstart,j))/2.0;
  adj=1.0*(1.0-TH_A)*exp(-pow(1.5*tLen/TH_LA,4))+TH_A;

  // I think these two conditional checks take care of the energy function value being to high
  // In that case, the normalized energy function is not used, instead the cost is too high!!!
  // Wow, I just noticed that the first if condition is absolutely unnecessary with the new curve class
  // Also, the first two checks of the second condition is unnecessary
  if( ((c01<1.0-TH_D || c01>1.0+TH_D ) && l1>MIN_D) || (( c02<1.0-TH_D || c02>1.0+TH_D) && l2>MIN_D) )
    cost=DP_VERY_LARGE_COST;
  else if(c<1.0-2.0*TH_L || c>1.0+2.0*TH_L || ang>2.0*adj)
    cost=DP_VERY_LARGE_COST;
  else 
  {
    // JONAH
    dF = -(exp(-pow(c-1,8)/(2*pow(TH_L,8)))) + exp(-0.5); 
    dK = -(exp(-pow(ang,8)/(2*pow(adj,8)))) + exp(-0.5);
    // CAN
    //dF = -(exp(-pow(c-1, TRANSITION_STEEPNESS)/(2*pow(TH_L, TRANSITION_STEEPNESS)))) + 0.5;
    //dK = -(exp(-pow(ang, TRANSITION_STEEPNESS)/(2*pow(adj, TRANSITION_STEEPNESS)))) + 0.5;
    cost = (dF+LAM*dK) * (l1+l2)/20;
  }
  return cost;
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::findDPCorrespondence()
{
  int i,j,ip,jp;
  _finalMap.clear();
  _finalMapCost.clear();
  
  ip=_nend;
  jp=_mend;
  i=_nend;
  j=_mend;
  vcl_pair <int,int> p(i,j);
  _finalMap.push_back(p);
  _finalMapCost.push_back(_cost[p.first][p.second]);
  
  while (ip > 0 || jp > 0)
  {
    ip=_map[i][j].first;
    jp=_map[i][j].second;
    
    vcl_pair <int,int> p(ip,jp);
    _finalMap.push_back(p);
    _finalMapCost.push_back(_cost[p.first][p.second]);

    if((ip==0 && jp!=0) || (ip!=0 && jp==0)) 
    {
      vcl_cout << "ERROR" << vcl_endl;
      exit(-1);
    }

    _finalCost=_cost[_nend][_mend];

    i=ip;
    j=jp;    
  }
}

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::match()
{
  _cost[0][0]=0;
  _map[0][0].first=-1;
  _map[0][0].second=-1;
 
  computeDPCosts();
  findDPCorrespondence();
}
#endif

template <class curveType,class floatType>
void DPMatch<curveType,floatType>::write_dp_costs(vcl_string fname)
{
  vcl_ofstream out(fname.c_str());
  for(int i=0; i<_n; i++)
  {
    for(int j=0; j<_m; j++)
    {
      out << _cost[i][j] << " ";
    }
    out << vcl_endl;
  }
  out.close();
}
