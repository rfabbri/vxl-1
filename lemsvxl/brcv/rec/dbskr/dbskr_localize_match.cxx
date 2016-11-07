#include <dbskr/dbskr_localize_match.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_scurve.h>

#define ALMOST_EQUAL(X,Y,Z)   ((vcl_fabs(X-Y) < Z) ? true : false)

dbskr_localize_match::dbskr_localize_match(){

  vcl_vector<int> a;
  vcl_map <int, float> b;
  vcl_vector< vcl_pair <int,int> > c;
  vcl_vector <double> d;
  vcl_vector <float> e;
  dbskr_scurve_sptr c1,c2;

  _curve1 = c1;
  _curve2 = c2;

  _c1Map=a;
  _c2Map=a;
  _initMap=c;

  _prevCostC1 = b;
  _prevCostC2 = b;
  _currCostC1 = b;
  _currCostC2 = b;

  _cost=e;
  _finalCost=DP_VERY_LARGE_COST;

  _n=0;
  _m=0;
  _interpn=0;
  _interpm=0;
  
  _R=1;
  _lambda=e;
  _finalMap=c;
  _ds1=d;
  _ds2=d;
  _dt1=d;
  _dt2=d;
  _numCostElems=0;
}

//dbskr_localize_match::dbskr_localize_match(dbskr_scurve_sptr c1, 
//              dbskr_scurve_sptr c2,
//              vcl_vector< vcl_pair <int,int> > initMap,
//              double sampleSize) {

dbskr_localize_match::dbskr_localize_match(dbskr_scurve_sptr c1,
                                           dbskr_scurve_sptr c2, 
                                           dbskr_scurve_sptr dc1,
                                           dbskr_scurve_sptr dc2,
                                           vcl_vector<int>& map1,
                                           vcl_vector<int>& map2,
                                           vcl_vector< vcl_pair<int,int> > initMap,
                                           float R) {

  vcl_vector<int> a;
  vcl_map <int, float> b;
  vcl_vector <float> d;

  _n=c1->num_points();
  _m=c2->num_points();
  
  dbskr_scurve_sptr interpC1 = dc1;
  dbskr_scurve_sptr interpC2 = dc2;

  _interpn=interpC1->num_points();
  _interpm=interpC2->num_points();
  
  _curve1=interpC1;
  _curve2=interpC2;

 /* int ind1=0;
  int ind2=0;
  */
  for (int i=initMap.size()-1;i>=0;i--){
    int p1=initMap[i].first;
    int p2=initMap[i].second;
    /*
    float s1=c1->arc_length(p1);
    float s2=c2->arc_length(p2);
    
    //while(!almostEqual(interpC1.arcLength(ind1),s1,1E-5) && ind1<_interpn )
    while(!ALMOST_EQUAL(interpC1->arc_length(ind1),s1,1E-5) && ind1<_interpn )
      ind1++;
    //while(!almostEqual(interpC2.arcLength(ind2),s2,1E-5) && ind2<_interpm )
    while(!ALMOST_EQUAL(interpC2->arc_length(ind2),s2,1E-5) && ind2<_interpm )
      ind2++;
    */
    _c1Map.push_back(map1[p1]);
    _c2Map.push_back(map2[p2]);
    
    /*if (verbose){
      printf("(%d %d) (%d %d) (%d %d)\n",p1,p2,ind1,ind2,map1[p1],map2[p2]);
      fflush(stdout);
    }*/
    vcl_pair<int,int> p(map1[p1],map2[p2]);
    _map[p]=vcl_pair<int,int>(0,0);
  }
  

  _prevCostC1 = b;
  _prevCostC2 = b;
  _currCostC1 = b;
  _currCostC2 = b;

  _cost= d;
  _finalCost=DP_VERY_LARGE_COST;

  //: default is 6.0f 
  _R=R;
  
  //: ozge changed to defaults
  //_lambda=lambda;
  _lambda.push_back(1.0f);
  _lambda.push_back(1.0f);
  _lambda.push_back(1.0f);

  //: ozge changed to defaults
  //_numCostElems = numCostElems;
  _numCostElems = 3;
  vcl_vector <double> v(_numCostElems,0);
  _ds1=v;
  _ds2=v;
  _dt1=v;
  _dt2=v;

}

void dbskr_localize_match::match(){

  _finalCost=0.0;
  int numSamples=_c1Map.size();

  int i=1;
  int prev1s, prev1e, prev2s, prev2e;
  int curr1s, curr1e, curr2s, curr2e;
  int prev1,prev2;
  int curr1,curr2;

  float cost;
  vcl_pair <int,int> prevPt;

  _finalMap.clear();
  prevPt.first=0;
  prevPt.second=0;

  _finalMap.push_back(prevPt);
  //First point in the alignment curve
  if (numSamples > 2){
    i=2;
    curr1s= _c1Map[i-2];
    curr1e= _c1Map[i];
    curr2s= _c2Map[i-2];
    curr2e= _c2Map[i];

    prev1=_c1Map[0];
    prev2=_c2Map[0];
    
    curr2=_c2Map[i-1];
    for ( curr1=curr1s;curr1<=curr1e; curr1++)
      _prevCostC1[curr1]=computeIntervalCost(prev1,curr1,prev2,curr2);
    
    curr1=_c1Map[i-1];
    for ( curr2=curr2s;curr2<=curr2e; curr2++)
      _prevCostC2[curr2]=computeIntervalCost(prev1,curr1,prev2,curr2);
  }
  else
    return;
  
  for(i=2;i<numSamples-1;i++){
    prev1s= _c1Map[i-2];
    prev1e= _c1Map[i];
    prev2s= _c2Map[i-2];
    prev2e= _c2Map[i];
    curr1s= _c1Map[i-1];
    curr1e= _c1Map[i+1];
    curr2s= _c2Map[i-1];
    curr2e= _c2Map[i+1];
    
    _currCostC1.clear();
    _currCostC2.clear();

    curr2=_c2Map[i];
    for (curr1=curr1s;curr1<curr1e;curr1++){
      _currCostC1[curr1]=DP_VERY_LARGE_COST;
      prev2=_c2Map[i-1];
      for (prev1=prev1s;(prev1<curr1 && prev1<prev1e); prev1++){
  float incCost=computeIntervalCost(prev1,curr1,prev2,curr2);
  cost =_prevCostC1[prev1]+incCost;

  if (cost < _currCostC1[curr1]){
    _currCostC1[curr1]=cost;

    _map[vcl_pair<int,int>(curr1,curr2)].first=prev1;
    _map[vcl_pair<int,int>(curr1,curr2)].second=prev2;
    prevPt.first=prev1;
    prevPt.second=prev2;
  }
      }
      prev1=_c1Map[i-1];
      for (prev2=prev2s;(prev2<curr2 && prev2<prev2e); prev2++){
  float incCost=computeIntervalCost(prev1,curr1,prev2,curr2);
  cost =_prevCostC2[prev2]+incCost;

  if (cost < _currCostC1[curr1]){
    _currCostC1[curr1]=cost;

    _map[vcl_pair<int,int>(curr1,curr2)].first=prev1;
    _map[vcl_pair<int,int>(curr1,curr2)].second=prev2;
    prevPt.first=prev1;
    prevPt.second=prev2;
  }
      }
    }
    
    curr1=_c1Map[i];
    for (curr2=curr2s;curr2<curr2e;curr2++){
      _currCostC2[curr2]=DP_VERY_LARGE_COST;
      prev2=_c2Map[i-1];
      for (prev1=prev1s;(prev1<curr1 && prev1<prev1e); prev1++){
  float incCost=computeIntervalCost(prev1,curr1,prev2,curr2);
  cost =_prevCostC1[prev1]+incCost;

  if (cost < _currCostC2[curr2]){
    _currCostC2[curr2]=cost;

    _map[vcl_pair<int,int>(curr1,curr2)].first=prev1;
    _map[vcl_pair<int,int>(curr1,curr2)].second=prev2;
    prevPt.first=prev1;
    prevPt.second=prev2;
  }
      }
      prev1=_c1Map[i-1];
      for (prev2=prev2s;(prev2<curr2 && prev2<prev2e); prev2++){
  float incCost=computeIntervalCost(prev1,curr1,prev2,curr2);
  cost =_prevCostC2[prev2]+incCost;

  if (cost < _currCostC2[curr2]){
    _currCostC2[curr2]=cost;

    _map[vcl_pair<int,int>(curr1,curr2)].first=prev1;
    _map[vcl_pair<int,int>(curr1,curr2)].second=prev2;
    prevPt.first=prev1;
    prevPt.second=prev2;
  }
      }
    }

    _finalMap.push_back(prevPt);

    _prevCostC1.clear();
    _prevCostC2.clear();
    

    for ( curr1=curr1s;curr1<curr1e; curr1++)
      _prevCostC1[curr1]=_currCostC1[curr1];
    for ( curr2=curr2s;curr2<curr2e; curr2++)
      _prevCostC2[curr2]=_currCostC2[curr2];
  }
   
  //Last point in the alignment curve
  _finalCost=DP_VERY_LARGE_COST;
  i=numSamples-1;
  prev1s= _c1Map[i-2];
  prev1e= _c1Map[i];
  prev2s= _c2Map[i-2];
  prev2e= _c2Map[i];
  
  curr1=_c1Map[i];
  curr2=_c2Map[i];
  
  prev2=_c2Map[i-1];
  for ( prev1=prev1s;prev1<prev1e; prev1++){
    cost=_prevCostC1[prev1]+computeIntervalCost(prev1,curr1,prev2,curr2);
    if (cost < _finalCost){
      _finalCost=cost;

      _map[vcl_pair<int,int>(curr1,curr2)].first=prev1;
      _map[vcl_pair<int,int>(curr1,curr2)].second=prev2;
       prevPt.first=prev1;
       prevPt.second=prev2;
    }
  }
  
  prev1=_c1Map[i-1];
  for ( prev2=prev2s;prev2<prev2e; prev2++){
    cost=_prevCostC2[prev2]+computeIntervalCost(prev1,curr1,prev2,curr2);
    if (cost < _finalCost){
       _finalCost=cost;
       
       _map[vcl_pair<int,int>(curr1,curr2)].first=prev1;
       _map[vcl_pair<int,int>(curr1,curr2)].second=prev2;
       prevPt.first=prev1;
       prevPt.second=prev2;
    }
  }

  _map[vcl_pair<int,int>(_c1Map[i],_c2Map[i])].first=prevPt.first;
  _map[vcl_pair<int,int>(_c1Map[i],_c2Map[i])].second=prevPt.second;

  _finalMap.clear();
  _finalMap.push_back(vcl_pair<int,int>(_c1Map[i],_c2Map[i]));
  int ii=numSamples-1;
  while(ii>0){
    int sz=_finalMap.size();
    int s1=_finalMap[sz-1].first;
    int s2=_finalMap[sz-1].second;
    _finalMap.push_back(_map[vcl_pair<int,int>(s1,s2)]);

    ii--;
  }

}

float dbskr_localize_match::computeIntervalCost(int i, int ip, 
                     int j, int jp){
  float cost,dF=0,dK=0;


  //vcl_vector <float> ds1,ds2,dt1,dt2;
  int k;
  _curve1->stretch_cost(i,ip,_ds1);
  _curve2->stretch_cost(j,jp,_ds2);
  for (k=0;k<_numCostElems;k++)
    dF = float(dF+fabs(_ds1[k]-_lambda[k]*_ds2[k]));
  
  //if (i != ip && j != jp){
  _curve1->bend_cost(i,ip,_dt1);
  _curve2->bend_cost(j,jp,_dt2);
  for (k=0;k<_numCostElems;k++)
    dK = float(dK+fabs(_dt1[k]-_dt2[k]));

  cost = dF+_R*dK;
  
  return cost;
} 

