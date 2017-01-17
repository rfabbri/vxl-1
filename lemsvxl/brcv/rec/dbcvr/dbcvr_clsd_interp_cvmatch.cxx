
#include "dbcvr_clsd_interp_cvmatch.h"
#include "dbcvr_cvmatch.h"
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>

#include <dbsol/dbsol_interp_curve_2d.h>

#define EPSILON         (1.0E-8)
#define SMALL           (1.0E-5)
#define SMALL_ENOUGH    (1.0E-1)
#define ROUND(X)        ((int)((X)+0.5))
#define APPROX_INT(X)   ( vcl_fabs((X)-ROUND(X)) < SMALL ? ROUND(X) : (X) )

// curves are closed and dbsol_interp_curve_2d assumes that the angle of the point
// for s = 0 is read from s = L, due to the interval structure used in that class.

//: return real arclength along second curve, s2 should never exceed L2 in fact
double dbcvr_clsd_interp_cvmatch::get_s2(double s) {
  s = (vcl_abs(s-L2) < SMALL ? L2 : s);
  s = vcl_fmod(s, L2);
  return (vcl_abs(s) < SMALL ? L2 : s);
}

//: return real arclength along first curve
double dbcvr_clsd_interp_cvmatch::get_s1(double s) {
  s = (vcl_abs(s-L1) < SMALL ? L1 : s);
  s = vcl_fmod(s, L1);
  return (vcl_abs(s) < SMALL ? L1 : s);
}

dbcvr_clsd_interp_cvmatch::dbcvr_clsd_interp_cvmatch(dbsol_interp_curve_2d_sptr c1, 
                                                     dbsol_interp_curve_2d_sptr c2, 
                                                     double R, 
                                                     double delta_ksi, double delta_eta) {
    
  L1 = c1->length();  
  L2 = c2->length();

  // TODO using original curves might cause problems --> check!!
  _curve1 = c1;
  _curve2 = c2;

  // assure shorter curve is _curve1
  if (L1 > L2) {   // swap
    // TODO using original curves might cause problems --> check!!
    _curve1 = c2;
    _curve2 = c1;
    double temp = L1;
    L1 = L2;
    L2 = temp;
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

  // inputs should be closed curves, last interval is the preceding interval of first data point
  // i.e. the interval between last and first data points
  //assert(*(_curve1->point_at(0)) == *(_curve1->point_at(L1)));
  //assert(*(_curve2->point_at(0)) == *(_curve2->point_at(L2)));

  _R = R;
  _cost_flag = false;

  // Given delta_eta find k
  _delta_eta = delta_eta;
  _delta = delta_ksi;
  
  // k_: number of starting points is determined in construct()
  //     so it will be initialized there, together with paths_ vector

  //double L_bar = vcl_sqrt(vcl_pow(L1,2)+vcl_pow(L2,2));
  //k_ = vcl_floor(L1*L2/(delta_eta*L_bar));

  //k_ = vcl_ceil( L1/ ((L1+L2)/(n+m)) );   // number of starting points
  //vcl_cout << "number of starting points: " << k_ << vcl_endl;

  //_delta_eta = (L1*L2)/( vcl_pow(L1*L1 + L2*L2, 0.5) * k_ );
  
  vcl_cout << "delta_eta: " << delta_eta << vcl_endl;
 
  //double ratio = 10.0f;  // number of discrete orientations that alignment curve can have
  //_delta = (vcl_pow(L1*L2,2) / ( vcl_pow(vcl_pow(L1*L1 + L2*L2, 0.5), 3) ) * (ratio/k_);
  vcl_cout << "delta_ksi: " << _delta << vcl_endl;
  
  

  _cost_threshold = DP_VERY_LARGE_COST;
  _alpha = vcl_atan2(L2, L1);  

  ca = cos(_alpha);
  sa = sin(_alpha);
  ta = tan(_alpha);
  if (vcl_fabs(_alpha - vnl_math::pi/4.0f) < SMALL) {
    sa = ca;
    ta = 1.0f;
  }

  // ratio (number of discrete orientations that alignment curve can have) 
  // with these delta_ksi and delta_eta values is
  vcl_cout << "ratio is: " << (_delta/_delta_eta)*(ta + 1/ta);

}

//: alignment curves starting at k_left and k_right on curve 2, find middle alignment curve
// half L/_delta times
// assuming that best path starting from k_left is already computed and is in paths_[k_left]
// also best path starting from k_right is computed and is in paths_[k_right];

void dbcvr_clsd_interp_cvmatch::computeMiddlePaths(int k_left, int k_right) 
{
  int k = (k_left + k_right)/2;

  if (k_left < k) {
    // initialize all costs to MAX
    initialize(k, k_left, k_right);

    //vcl_cout << "k: " << k << vcl_endl;
    //vcl_cout << "checking cost of CD[i_start][j_start], should be 0: " << 
    //            CD[start_cells_[k].first][start_cells_[k].second].cost << vcl_endl;
    //vcl_cout << "checking cost of CD[i_end][j_start], should be HIGH: " << 
    //            CD[end_cells_[k].first][end_cells_[k].second].cost << vcl_endl;


    // find path starting from this cell location and write the path to paths_[k]
    find_path(k);
    
    //vcl_cout << "Cost of best path for " << k << " is: " << finalCost(k) << vcl_endl;
    if (_cost_threshold > final_cost_[k]) _cost_threshold = final_cost_[k]+SMALL_ENOUGH;
    
    computeMiddlePaths(k_left, k);
    computeMiddlePaths(k, k_right);
  }
}


//: construct the grid 
void dbcvr_clsd_interp_cvmatch::construct()
{ 
  double eta, ksi;
  double L_bar = vcl_sqrt(vcl_pow(L1,2)+vcl_pow(L2,2));
  double max_diagonal = L_bar + L1*ca;

  // determine max i
  max_i_ = static_cast<int>(vcl_ceil(APPROX_INT(max_diagonal/_delta))+1);
  vcl_cout << "max_i: " << max_i_ << vcl_endl;

  // build cells up CU, main diagonal starting from (0,0) will be in CD 
  // find max_j for cells up
  max_j_up_ = static_cast<int>(vcl_floor(APPROX_INT(L2*ca/_delta_eta)));
  vcl_cout << "max_j_up: " << max_j_up_ << vcl_endl;
  
  // initialize grid with max_i and max_j so we have CU[i][0] for each i
  for (int i = 0; i<max_i_; i++) {
      vcl_vector<dptcell> tmp;
      CU.push_back(tmp);
  }

  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_up_-1; j++) {   // -1 since (0,0) will go to CD
      dptcell c;
      c.cost = _cost_threshold;
      c.s1 = -1;   // if negative values than invalid cell
      c.s2 = -1;
      //c.s1 = i*_delta*ca-(j+1)*_delta_eta*sa;
      //c.s2 = i*_delta*sa+(j+1)*_delta_eta*ca;
      c.valid = false;
      c.prev_cell_coord.first = -1;
      CU[i].push_back(c);
    }
  }

  //for (int i = 0; i<max_i; i++) {
  //  vcl_cout << "cells up size of i: " << i << " column is " << CU[i].size() << vcl_endl;
    //for (int j = 0; j<CU[i].size(); j++)
      //vcl_cout << "CU["<<i<<"]["<<j<<"]: " << CU[i][j].s1 << vcl_endl;
  //}

  for (int j = 0; j<max_j_up_-1; j++) 
  {
    // find eta that corresponds to this j
    // float since I don't want to have precision errors due to double
    eta = (j+1)*_delta_eta;     // I'll put j=0 into CD
    // for each j, there exists a starting i and ending i
    
    // find ksi that lies on s1 = 0 for given j
    ksi = eta*sa/ca;
    double temp = ksi/_delta;
    //int i_start = vcl_floor(ksi/_delta);
    int i_start = static_cast<int>(vcl_floor(APPROX_INT(temp)));

    // write to CU(i = i_start,j) irregular grid values for s1 and s2
    CU[i_start][j].s1 = 0;
    CU[i_start][j].s2 = eta/ca;
    
    // find ksi that lies on s2 = L2 for given j (ending point of this row of j)
    ksi = (L2-eta*ca)/sa;
    temp = ksi/_delta;
    //int i_end = vcl_ceil(ksi/_delta);
    int i_end = static_cast<int>(vcl_ceil(APPROX_INT(temp)));
    CU[i_end][j].s1 = ksi*ca-eta*sa;
    CU[i_end][j].s2 = L2;

    CU[i_start][j].theta1 = _curve1->tangent_angle_at(get_s1(CU[i_start][j].s1));
    CU[i_start][j].theta2 = _curve2->tangent_angle_at(get_s2(CU[i_start][j].s2));

    CU[i_end][j].theta1 = _curve1->tangent_angle_at(get_s1(CU[i_end][j].s1));
    CU[i_end][j].theta2 = _curve2->tangent_angle_at(get_s2(CU[i_end][j].s2));
    
    // all other cells (i,j) have regular s1 and s2 values
    for (int i = i_start+1; i<i_end; i++) {
      CU[i][j].s1 = i*_delta*ca-(j+1)*_delta_eta*sa;
      CU[i][j].s2 = i*_delta*sa+(j+1)*_delta_eta*ca;

      CU[i][j].theta1 = _curve1->tangent_angle_at(get_s1(CU[i][j].s1));
      CU[i][j].theta2 = _curve2->tangent_angle_at(get_s2(CU[i][j].s2));
    }
  }


  // build cells down CD, all the starting points including (0,0) will lie on this down grid
  // find max_j for cells down
  max_j_down_ = static_cast<int>(vcl_floor(APPROX_INT(2*L1*sa/_delta_eta)));
  vcl_cout << "max_j_down for cells down: " << max_j_down_ << vcl_endl;
  
  // initialize grid with max_i and max_j so we have CD[i][0] for each i
  for (int i = 0; i<max_i_; i++) {
      vcl_vector<dptcell> tmp;
      CD.push_back(tmp);
  }

  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_down_; j++) {
      dptcell c;
      c.cost = _cost_threshold;
      c.s1 = -1;   // if negative values then invalid cell
      c.s2 = -1;
      //c.s1 = i*_delta*ca-(-j)*_delta_eta*sa;
      //c.s2 = i*_delta*sa+(-j)*_delta_eta*ca;
      c.valid = false;
      c.prev_cell_coord.first = -1;
      CD[i].push_back(c);
    }
  }

  for (int j = 0; j<max_j_down_; j++) 
  {
    // find eta that corresponds to this j 
    eta = j*_delta_eta;   // negative actually for j >= 1
    // for each j, there exists a starting i and ending i
    
    // find ksi for s2 = 0 for given j
    ksi = eta*ca/sa;
    double temp = ksi/_delta;
    //int temp2 = vcl_floor(temp);
    //int i_start = vcl_floor((double)(ksi/_delta));
    int i_start = static_cast<int>(vcl_floor(APPROX_INT(temp)));

    // write to CD(i = i_start,j) irregular grid values for s1 and s2
    CD[i_start][j].s1 = eta/sa;
    CD[i_start][j].s2 = 0;
    
    int i_end;
    if (CD[i_start][j].s1 < L1) {  // end point is for s2 = L2
      ksi = L_bar + eta*ca/sa;
      double temp = ksi/_delta;
      //i_end = vcl_ceil(ksi/_delta);
      i_end = static_cast<int>(vcl_ceil(APPROX_INT(temp)));
      CD[i_end][j].s1 = ksi*ca + eta*sa;   // since eta is negative actually 
                                           // here s1 is greater than L1
      CD[i_end][j].s2 = L2; 
      vcl_pair<int, int> p(i_start, j); 
      start_cells_.push_back(p);
      vcl_pair<int, int> p2(i_end, j);
      end_cells_.push_back(p2);
    } else { // end point is for s1 = L1
      ksi = (2*L1-eta*sa)/ca;   // here eta is negative so + is made into -
      double temp = ksi/_delta;
      //i_end = vcl_ceil(ksi/_delta);
      i_end = static_cast<int>(vcl_ceil(APPROX_INT(temp)));
      CD[i_end][j].s1 = 2*L1;
      CD[i_end][j].s2 = ksi*sa-eta*ca;     // since eta is negative, sign is changed to - for s2
    }

    
    CD[i_start][j].theta1 = _curve1->tangent_angle_at(get_s1(CD[i_start][j].s1));
    CD[i_start][j].theta2 = _curve2->tangent_angle_at(get_s2(CD[i_start][j].s2));

    CD[i_end][j].theta1 = _curve1->tangent_angle_at(get_s1(CD[i_end][j].s1));
    CD[i_end][j].theta2 = _curve2->tangent_angle_at(get_s2(CD[i_end][j].s2));

    // all other cells (i,j) have regular s1 and s2 values
    for (int i = i_start+1; i<i_end; i++) {
      CD[i][j].s1 = i*_delta*ca+eta*sa;
      CD[i][j].s2 = i*_delta*sa-eta*ca;

      CD[i][j].theta1 = _curve1->tangent_angle_at(get_s1(CD[i][j].s1));
      CD[i][j].theta2 = _curve2->tangent_angle_at(get_s2(CD[i][j].s2));
    }
  }

  // pick some arbitrary j and print s1 and s2 values on it on CD
  /*
  int j = 50;
  for (int i = 0; i<max_i; i++) {
    vcl_cout << "CD["<<i<<"]["<<j<<"].s1: " << CD[i][j].s1;
    vcl_cout << " CD["<<i<<"]["<<j<<"].s2: " << CD[i][j].s2 << vcl_endl;
  }*/

  // set k to the number of starting cells + 1
  k_ = start_cells_.size();   // including (0,0) starting point

  for (int i = 0; i<k_; i++) {
    vcl_vector<int> tmp(max_i_, 0);   // i runs to max_i, initialize with value 0 
    paths_.push_back(tmp);
    final_cost_.push_back(-1);
  }
                              
  //vcl_cout << "start cells size: " << start_cells_.size() << vcl_endl;
  //vcl_cout << "end cells size: " << end_cells_.size() << vcl_endl;

  /*for (int k = 0; k<start_cells_.size(); k++) {
    int i = start_cells_[k].first;
    int j = start_cells_[k].second;
    vcl_cout << "k: " << k << "CD("<<i<<","<<j<<") ";
    vcl_cout << "s1: " << CD[i][j].s1 << " s2: " << CD[i][j].s2 << vcl_endl;
  }

  for (int k = 0; k<end_cells_.size(); k++) {
    int i = end_cells_[k].first;
    int j = end_cells_[k].second;
    vcl_cout << "k: " << k << " CD("<<i<<","<<j<<") ";
    vcl_cout << "s1: " << CD[i][j].s1 << " s2: " << CD[i][j].s2 << vcl_endl;
  }*/
/*
  vcl_ofstream fpoo; 
  fpoo.open("D:\\contours\\Mpeg-7\\temp.out", vcl_ios::app);

  fpoo << max_i_ << " " << max_j_up_-1 << "\n";
  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_up_-1; j++) {   // -1 since (0,0) will go to CD
      fpoo << CU[i][j].s1 << " ";
      fpoo << CU[i][j].s2 << vcl_endl;
    }
  }

  fpoo << max_i_ << " " << max_j_down_ << "\n";
  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_down_; j++) {   
      fpoo << CD[i][j].s1 << " ";
      fpoo << CD[i][j].s2 << vcl_endl;
    }
  }

  fpoo.close();
*/

}

//: initialize all the costs in the grid to MAX
//  initialize valid field of the cells to true if they are in the search space 
void dbcvr_clsd_interp_cvmatch::initialize(int k, int k_left, int k_right) {

  double eta, ksi;
  //vcl_cout << "initializing for k: " << k << " k_left: " << k_left << " k_right: " << k_right << vcl_endl;

  // invalidate all cells first
  for (int i = 0; i<max_i_; i++) 
    for (int j = 0; j<max_j_up_-1; j++) {
      CU[i][j].cost = _cost_threshold;
      CU[i][j].valid = false;
      CU[i][j].prev_cell_coord.first = -1;
    }

  for (int i = 0; i<max_i_; i++) 
    for (int j = 0; j<max_j_down_; j++) {
      CD[i][j].cost = _cost_threshold;
      CD[i][j].valid = false;
      CD[i][j].prev_cell_coord.first = -1;
    }

  int i_start = start_cells_[k].first;
  int j_start = start_cells_[k].second;

  int i_end = end_cells_[k].first;
  int j_end = end_cells_[k].second;

  //vcl_cout << "i_start: " << i_start << " i_end: " << i_end << " j_start: " << j_start << " j_end: " << j_end << vcl_endl;

  // initialize cost of the starting cell to zero
  CD[i_start][j_start].cost = 0;

  double ss1 = CD[i_start][j_start].s1;
  double es1 = CD[i_end][j_end].s1;

  // validate cells in the rectangle induced by this starting point
  // first for the cells to the left of starting point diagonal in cells down
  for (int j = j_start; j>=0; j--) 
  {
    // find eta that corresponds to this j
    eta = j*_delta_eta;     // negative actually
    
    // for each j, there exists a starting i and ending i
    // find ksi that lies on s1 = ss1 for given j
    ksi = (ss1-eta*sa)/ca;
    double temp = ksi/_delta;
    //int i_start = vcl_floor((double)(ksi/_delta));
    int i_start = static_cast<int>(vcl_floor(APPROX_INT(temp)));
    
    // find ksi that lies on s2 = L2 for given j (ending point of this row of j)
    ksi = (L2+eta*ca)/sa;    // sign change due to negative eta
    temp = ksi/_delta;
    //int i_end = vcl_ceil(ksi/_delta);
    int i_end = static_cast<int>(vcl_ceil(APPROX_INT(temp)));

    // if there is no path defined for k_left
    if (k_left < 0) {   
      // all these cells are valid for this j
      for (int i = i_start; i<=i_end; i++) 
        if (CD[i][j].s1 >= ss1 && CD[i][j].s1 <= es1) 
          CD[i][j].valid = true;
    } else {
      for (int i = i_start; i<=i_end; i++) 
        if (paths_[k_left][i] >= -j)
          if (CD[i][j].s1 >= ss1 && CD[i][j].s1 <= es1) 
            CD[i][j].valid = true;
    }
  }

  // for the cells in CU
  for (int j = 0; j<max_j_up_-1; j++) 
  {
    // find eta that corresponds to this j
    eta = (j+1)*_delta_eta;     
    // for each j, there exists a starting i and ending i
    
    // find ksi that lies on s1 = ss1 for given j
    ksi = (ss1+eta*sa)/ca;   // eta is positive this time so use original sign of the equation
    double temp = ksi/_delta;
    //int i_start = vcl_floor(ksi/_delta);
    int i_start = static_cast<int>(vcl_floor(APPROX_INT(temp)));
    
    // find ksi that lies on s2 = L2 for given j (ending point of this row of j)
    ksi = (L2-eta*ca)/sa;
    temp = ksi/_delta;
    //int i_end = vcl_ceil(ksi/_delta);
    int i_end = static_cast<int>(vcl_ceil(APPROX_INT(temp)));

    // if there is no path defined for k_left
    if (k_left < 0) {   
      // all these cells are valid for this j
      for (int i = i_start; i<=i_end; i++)   // for some j's it may never go into this loop
        if (CU[i][j].s1 >= ss1 && CU[i][j].s1 <= es1) CU[i][j].valid = true;
    } else {
      for (int i = i_start; i<=i_end; i++) 
        if (paths_[k_left][i] >= (j+1))
          if (CU[i][j].s1 >= ss1 && CU[i][j].s1 <= es1) CU[i][j].valid = true;
    }
  }

  // now for the cells to the right of starting point diagonal in cells down
  for (int j = j_start+1; j<max_j_down_; j++) 
  {
    // find eta that corresponds to this j
    eta = j*_delta_eta;     // negative actually
    
    // for each j, there exists a starting i and ending i
    // find ksi that lies on s2 = 0 for given j
    ksi = eta*ca/sa;
    double temp = ksi/_delta;
    //int i_start = vcl_floor(ksi/_delta);
    int i_start = static_cast<int>(vcl_floor(APPROX_INT(temp)));
    
    // find ksi that lies on s1 = es1 for given j (ending point of this row of j)
    ksi = (es1-eta*sa)/ca;    // sign change due to negative eta
    temp = ksi/_delta;
    //int i_end = vcl_floor(ksi/_delta);
    int i_end = static_cast<int>(vcl_floor(APPROX_INT(temp)));

    // if there is no path defined for k_right
    if (k_right < 0) {   
      // all these cells are valid for this j
      for (int i = i_start; i<=i_end; i++)   // for some j's it may never go into this loop
        CD[i][j].valid = true;
    } else {
      for (int i = i_start; i<=i_end; i++) 
        if (i <= start_cells_[k_right].first || paths_[k_right][i] <= -j)
          CD[i][j].valid = true;
    }
  }

  // check validity of CD[i_end][j_end]
  //vcl_cout << "CD[i_end][j_end].valid: " << CD[i_end][j_end].valid << vcl_endl;
  //vcl_cout << "CD[i_start][j_start].valid: " << CD[i_start][j_start].valid << vcl_endl;

}

//: update the cost of the cell, as if coming from cell: c_prev 
void dbcvr_clsd_interp_cvmatch::update_cost(dptcell *c, dptcell *c_prev, int i_prev, int j_prev) {
  double ds1, ds2, dt1, dt2;

  ds1 = c->s1 - c_prev->s1;
  ds2 = c->s2 - c_prev->s2;

  dt1 = curve_angleDiff (c->theta1, c_prev->theta1);
  dt2 = curve_angleDiff (c->theta2, c_prev->theta2);
  
  double step_cost = vcl_fabs(ds2-ds1)+_R*vcl_fabs(dt1-dt2);
       
  if (c->cost > c_prev->cost + step_cost) {
    c->cost = c_prev->cost + step_cost;
    (c->prev_cell_coord).first = i_prev;
    (c->prev_cell_coord).second = j_prev;
  }
}

//: find optimum path starting from given start point and in the space
//  restricted to the region in between paths_[k_left] and paths_[k_right]
//  this restriction is achieved via initialization of the valid fields in the grid
//  via initialize function, so this function should be called
//  appropriately a priori to the find_path function
void dbcvr_clsd_interp_cvmatch::find_path(int k) {

  int i_start = start_cells_[k].first;
  int j_start = start_cells_[k].second;

  int i_end = end_cells_[k].first;
  // j_end equals j_start always

  // check if the initialization was done appropriately for this 
  // starting point
  assert(CD[i_start][j_start].valid);
  assert(CD[i_end][j_start].valid);

  if (j_start == 0) {  // try coming from upper irregular grid cells on s1 = 0 line
    for (int j = 0; j < max_j_up_-1; j++) {
      if (!CU[0][j].valid) break;
      come_previous(&(CU[0][j]), 0, j+1);
    }
  }

  //: try coming from left irregular grid cells on s2 = 0 line of the starting point
  //  which are on the same i_start row
  for (int j = j_start+1; j<max_j_down_; j++) {
    if (!CD[i_start][j].valid) break;
    come_previous(&(CD[i_start][j]), i_start, -j);
  }

  // now compute the costs of all cells in the valid region
  for (int i = i_start+1; i<i_end+1; i++) {
    for (int j = j_start; j>= 0; j--) {
      if (!CD[i][j].valid) break;  // once invalid no need to go up more, since all are invalid
      //: this function updates the cost of this cell
      //  by exploring all possible previous cells in the valid region
      come_previous(&(CD[i][j]), i, -j);
    }
    for (int j = 0; j< max_j_up_-1; j++) {
      if (!CU[i][j].valid) break;
      come_previous(&(CU[i][j]), i, j+1);
    }
    for (int j = j_start+1; j<max_j_down_; j++) {
      if (!CD[i][j].valid) break;
      come_previous(&(CD[i][j]), i, -j);
    }
  }

  //: sometimes cost is very small but not zero due to precision, correct it to zero
  final_cost_[k] = (vcl_fabs(CD[i_end][j_start].cost) < SMALL) ? 0 : CD[i_end][j_start].cost;

  paths_[k][i_end] = -j_start;

  int m = -j_start;
  int s = i_end;
  
  //: if optimum alignment curve passes through same i twice (only possible for irregular grid cells)
  //  then only one of them will be in paths_.
  //  The cost values are correct, only alignment lines will be missing for the missing irregular grid cells.
  //  In the initialization part, 
  //  leave j with larger absolute value in that row i, so that all of them remain valid.
  
  //while (i > i_start) {
  while (s >= 0) {
    double tag = false; // tag will become positive if i_prev == i and s1 = 2*L1
                        // only then paths should contain the smallest of j's 
    int s_prev, m_prev;
    if (m <= 0) {
      s_prev = CD[s][(-m)].prev_cell_coord.first;
      m_prev = CD[s][(-m)].prev_cell_coord.second;
      if (s_prev == s) 
        if (CD[s][(-m)].s1 == 2*L1)
          tag = true;
    } else {
      s_prev = CU[s][m-1].prev_cell_coord.first;
      m_prev = CU[s][m-1].prev_cell_coord.second;
    }

    if (s_prev < 0) break;

    if (s_prev == s) {  // possible only if s1 = 0, s2 = 0, s2 = L2 or s1 = 2*L1
      if (!tag) { // for the cases s1 = 0, s2 = 0, s2 = L2, keep larger value
        if (m_prev > paths_[s][s_prev])
          paths_[s][s_prev] = m_prev;
      } else { 
        if (m_prev < paths_[s][s_prev])
          paths_[s][s_prev] = m_prev;
      }

    } else {
      paths_[s][s_prev] = m_prev;
    }
    
    s = s_prev;
    m = m_prev;
  }
}

//: the condition for being a valid previous point:
//  c_prev->s1 <= c->s1 
//  c_prev->s2 <= c->s2
void dbcvr_clsd_interp_cvmatch::come_previous(dptcell *c, int i, int j) {

  // it might be possible to come from a cell on the same row i
  // if this current cell is an irregular grid location, i.e.
  // either on s1 = 0, s2 = 0, s2 = L2 or s1 = 2*L1 lines,
  // First find if this is the case:
  if (c->s1 == 0) {
    if (j == 1) { 
      if (CD[0][0].valid && 
          CD[0][0].cost < _cost_threshold) 
        update_cost(c, &CD[0][0], 0, 0); 
    }
    else if (CU[i][j-2].s1 == 0) { 
      if (CU[i][j-2].valid && 
          CU[i][j-2].cost < _cost_threshold) 
        update_cost(c, &CU[i][j-2], i, j-1); 
    }
    else { 
      if (CU[i-1][j-2].valid &&
          CU[i-1][j-2].cost < _cost_threshold) 
        update_cost(c, &CU[i-1][j-2], i-1, j-1); 
    }

  } else if (c->s2 == 0) {
      if (CD[i][(-j)-1].s2 == 0) { 
        if (CD[i][(-j)-1].valid &&
            CD[i][(-j)-1].cost < _cost_threshold) 
          update_cost(c, &CD[i][(-j)-1], i, j+1); 
      } else { 
        if (CD[i-1][(-j)-1].valid &&
            CD[i-1][(-j)-1].cost < _cost_threshold) 
          update_cost(c, &CD[i-1][(-j)-1], i-1, j+1); 
      }
 
  } else {

  // Secondly, find possible previous locations on the row i-1, 
  // For irregular grid cells s2 = L2 or s1 = 2*L1, we need to check previous row i-1
  // and current row i 
    if (c->s2 == L2) {  // check left neighbor on s2 = L2 line on row i, others will be checked later below
    
      if (j == 0) { 
        if (CU[i][0].s2 == L2 && CU[i][0].valid) {
          // actually in this row, the left neighbor comes "after" this grid cell
          // so it hasn't been visited yet, first update its cost (this will cause some redundancy!!)
          come_previous(&CU[i][0], i, 1);
          update_cost(c, &CU[i][0], i, 1); }
      }
      else if (j > 0) { 
        if (CU[i][j-2].s2 == L2 && CU[i][j-2].valid) {
          come_previous(&CU[i][j-1], i, j-1);
          update_cost(c, &CU[i][j-2], i, j-1); }
      }
      else { 
        if (CD[i][(-j)-1].s2 == L2 && CD[i][(-j)-1].valid) {
          come_previous(&CD[i][(-j)-1], i, j+1);
          update_cost(c, &CD[i][(-j)-1], i, j+1); 
        }
      }
    
    } else if (c->s1 == 2*L1) {  // check down neighbor on s1 = 2*L1 line on row i, others will be checked later below
                                 // only possible for negative j values
      if (CD[i][(-j)+1].s2 == L2 && CD[i][(-j)+1].valid) {
        come_previous(&CD[i][(-j)-1], i, j-1);
        update_cost(c, &CD[i][(-j)+1], i, j-1); 
      }
    }

    // here j might well be negative, or positive 
    // depending on the position of the cell in the grid (up cells, or down cells)
    double eta_up = j*_delta_eta + _delta*ta;
    double eta_down = j*_delta_eta - _delta/ta;

    int up_max = static_cast<int>(vcl_floor(eta_up/_delta_eta));
    int down_min = static_cast<int>(vcl_ceil(eta_down/_delta_eta));

    if (eta_up > 0)  {// then in upper cells
      up_max = (up_max > max_j_up_-1) ? max_j_up_-1 : up_max;
      if (eta_down <= 0) { 
        down_min = 1;   // it will start from down_min-1
      } 
    
      for (int k = down_min-1; k < up_max; k++) {
        if (!CU[i-1][k].valid) continue;
        if (CU[i-1][k].cost < _cost_threshold) 
          update_cost(c, &CU[i-1][k], i-1, k+1);
      }
    } 

    if (eta_down < 0) {  // then in lower cells
      down_min = static_cast<int>(-vcl_ceil(eta_down/_delta_eta));
      down_min = (down_min > max_j_down_-1) ? max_j_down_-1 : down_min;
      if (eta_up >= 0)
        up_max = 0;
      else 
        up_max = -up_max;
    
      for (int k = up_max; k <= down_min; k++) {
        if (!CD[i-1][k].valid) continue;
        if (CD[i-1][k].cost < _cost_threshold) 
          update_cost(c, &CD[i-1][k], i-1, -k);
      }

    }
  }

}

double dbcvr_clsd_interp_cvmatch::finalCost(int k) {
  return final_cost_[k];
}

void dbcvr_clsd_interp_cvmatch::Match() {
  
  // construct the grid
  construct();

  //initialize the grid for the match
  initialize(0, -1, -1); 
  //vcl_cout << "checking cost of CD[i_start][j_start], should be 0: " << 
  //              CD[start_cells_[0].first][start_cells_[0].second].cost << vcl_endl;
  find_path(0);
  //vcl_cout << "Cost of best path for 0 is: " << finalCost(0) << vcl_endl;
  
  if (_cost_threshold > final_cost_[0]) _cost_threshold = final_cost_[0];

  initialize(k_-1, 0, -1);
  find_path(k_-1);

  //vcl_cout << "Cost of best path for " << k_-1 << " is: " << finalCost(k_-1) << vcl_endl;
  if (_cost_threshold > final_cost_[k_-1]) _cost_threshold = final_cost_[k_-1];

  computeMiddlePaths(0, k_-1);
  double min = final_cost_[0];  
  k_min_ = 0;
  for (int k = 1; k<k_; k++)
    if (min > final_cost_[k]) {
      min = final_cost_[k];
      k_min_ = k;
    }

/*  vcl_ofstream fpoo; 
  fpoo.open("D:\\contours\\Mpeg-7\\temp.out", vcl_ios::app);

  // find number of valid cells for this starting point
  int count = 0;
  for (int i = 0; i<max_i_; i++) 
    for (int j = 0; j<max_j_up_-1; j++) 
      if (CU[i][j].valid) 
        count++;

  fpoo << count << "\n";
  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_up_-1; j++) {   // -1 since (0,0) will go to CD
      if (CU[i][j].valid) {
        fpoo << CU[i][j].s1 << " ";
        fpoo << CU[i][j].s2 << " " << CU[i][j].cost << vcl_endl;
      }
    }
  }

  count = 0;
  for (int i = 0; i<max_i_; i++) 
    for (int j = 0; j<max_j_down_; j++) 
      if (CD[i][j].valid) 
        count++;

  vcl_cout << "number of valid cells: " << count << vcl_endl;

  fpoo << count << "\n";
  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_down_; j++) {   
      if (CD[i][j].valid) {
        fpoo << CD[i][j].s1 << " ";
        fpoo << CD[i][j].s2 << " " << CD[i][j].cost << vcl_endl;
      }
    }
  }

  fpoo.close();
  */

  vcl_cout << "Cost of overall best path for " << k_min_ << " is: " << finalCost(k_min_) << vcl_endl;
  //vcl_cout << start_cells_[k_min_].first << " " << start_cells_[k_min_].second <<vcl_endl;
  //vcl_cout << end_cells_[k_min_].first << " " << end_cells_[k_min_].second <<vcl_endl;

  int j;
  for (int i = start_cells_[k_min_].first; i < end_cells_[k_min_].first; i++) {
    j = paths_[k_min_][i];
    if (j <= 0) {
      double s1 = CD[i][(-j)].s1;
      double s2 = CD[i][(-j)].s2;
      s1 = get_s1(s1);
      s2 = get_s2(s2);
      out_curve1_.push_back(_curve1->point_at(get_s1(CD[i][(-j)].s1)));
      out_curve2_.push_back(_curve2->point_at(get_s2(CD[i][(-j)].s2)));
    } else {
      double s1 = CU[i][j-1].s1;
      double s2 = CU[i][j-1].s2;
      s1 = get_s1(s1);
      s2 = get_s2(s2);
      out_curve1_.push_back(_curve1->point_at(get_s1(CU[i][j-1].s1)));
      out_curve2_.push_back(_curve2->point_at(get_s2(CU[i][j-1].s2)));
    }

  }

/*
  vcl_ofstream fpoo; 
  fpoo.open("D:\\contours\\Mpeg-7\\temp.out", vcl_ios::app);

  fpoo << (end_cells_[k_min_].first - start_cells_[k_min_].first + 1) << "\n";
  for (int i = start_cells_[k_min_].first; i <= end_cells_[k_min_].first; i++) {
    j = paths_[k_min_][i];
    if (j <= 0) {
      fpoo << CD[i][(-j)].s1 << " ";
      fpoo << CD[i][(-j)].s2 << vcl_endl;
    } else {
      fpoo << CU[i][j-1].s1 << " ";
      fpoo << CU[i][j-1].s2 << vcl_endl;
    }
  }

  fpoo.close();
  */

}

