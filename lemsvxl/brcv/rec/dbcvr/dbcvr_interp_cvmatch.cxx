
#include "dbcvr_interp_cvmatch.h"
#include "dbcvr_cvmatch.h"
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>

#include <dbsol/dbsol_interp_curve_2d.h>

#define EPSILON         (1.0E-8)
#define SMALL           (1.0E-5)
#define SMALL_ENOUGH    (1.0E-1)
#define ROUND(X)        ((int)((X)+0.5))
#define APPROX_INT(X)   ( vcl_fabs((X)-ROUND(X)) < SMALL ? ROUND(X) : (X) )


dbcvr_interp_cvmatch::dbcvr_interp_cvmatch(dbsol_interp_curve_2d_sptr c1, 
                                           dbsol_interp_curve_2d_sptr c2, 
                                           double R, 
                                           double delta_ksi, double delta_eta) {
    
  L1 = c1->length();  
  L2 = c2->length();

  _curve1 = c1;
  _curve2 = c2;

  //       c2
  //       ^
  //       |
  // L2    -----|
  //       |    |
  //       |    |
  //       |  / |
  //       |/   |
  //       -----'-->  c1           
  //            L1


  _R = R;
  _cost_flag = false;

  // Given delta_eta find k
  _delta_eta = delta_eta;
  _delta = delta_ksi;
  
  //vcl_cout << "delta_eta: " << delta_eta << vcl_endl;
  //vcl_cout << "delta_ksi: " << _delta << vcl_endl;  

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
  vcl_cout << "L1: " << L1 << " L2: " << L2 << vcl_endl;

}

//: construct the grid 
void dbcvr_interp_cvmatch::construct()
{ 
  double eta, ksi;
  double L_bar = vcl_sqrt(vcl_pow(L1,2)+vcl_pow(L2,2));
  double max_diagonal = L_bar;

  // determine max i
  max_i_ = static_cast<int>(vcl_ceil(APPROX_INT(max_diagonal/_delta))+1);
  vcl_cout << "max_i: " << max_i_ << vcl_endl;

  // build cells up CU, main diagonal starting from (0,0) will be in CD 
  // find max_j for cells up
  max_j_up_ = static_cast<int>(vcl_floor(APPROX_INT(L2*ca/_delta_eta)));
  vcl_cout << "max_j_up: " << max_j_up_ << vcl_endl;
  
  // initialize grid with max_i and max_j so we have CU[i][0] for each i
  for (int i = 0; i<max_i_; i++) {
      vcl_vector<dpt_cell> tmp;
      CU.push_back(tmp);
  }

  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_up_-1; j++) {   // -1 since (0,0) will go to CD
      dpt_cell c;
      c.cost = _cost_threshold;
      c.s1 = -1;   // if negative values than invalid cell
      c.s2 = -1;
      //c.s1 = i*_delta*ca-(j+1)*_delta_eta*sa;
      //c.s2 = i*_delta*sa+(j+1)*_delta_eta*ca;
      c.prev_cell_coord.first = -1;
      c.valid = false;
      CU[i].push_back(c);
    }
  }

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
    CU[i_start][j].valid = true;

    // find ksi that lies on s2 = L2 for given j (ending point of this row of j)
    ksi = (L2-eta*ca)/sa;
    temp = ksi/_delta;
    //int i_end = vcl_ceil(ksi/_delta);
    int i_end = static_cast<int>(vcl_ceil(APPROX_INT(temp)));
    CU[i_end][j].s1 = ksi*ca-eta*sa;
    CU[i_end][j].s2 = L2;
    CU[i_end][j].valid = true;

    CU[i_start][j].theta1 = _curve1->tangent_angle_at(CU[i_start][j].s1);
    CU[i_start][j].theta2 = _curve2->tangent_angle_at(CU[i_start][j].s2);

    CU[i_end][j].theta1 = _curve1->tangent_angle_at(CU[i_end][j].s1);
    CU[i_end][j].theta2 = _curve2->tangent_angle_at(CU[i_end][j].s2);
    
    // all other cells (i,j) have regular s1 and s2 values
    for (int i = i_start+1; i<i_end; i++) {
      CU[i][j].s1 = i*_delta*ca-(j+1)*_delta_eta*sa;
      CU[i][j].s2 = i*_delta*sa+(j+1)*_delta_eta*ca;
      CU[i][j].valid = true;

      CU[i][j].theta1 = _curve1->tangent_angle_at(CU[i][j].s1);
      CU[i][j].theta2 = _curve2->tangent_angle_at(CU[i][j].s2);
    }
  }


  // build cells down CD, all the starting points including (0,0) will lie on this down grid
  // find max_j for cells down
  double temp = L1*sa/_delta_eta;
  //max_j_down_ = vcl_floor(APPROX_INT(L1*sa/_delta_eta));
  max_j_down_ = static_cast<int>(vcl_floor(APPROX_INT(temp)));
  vcl_cout << "max_j_down for cells down: " << max_j_down_ << vcl_endl;
  
  // initialize grid with max_i and max_j so we have CD[i][0] for each i
  for (int i = 0; i<max_i_; i++) {
      vcl_vector<dpt_cell> tmp;
      CD.push_back(tmp);
  }

  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_down_; j++) {
      dpt_cell c;
      c.cost = _cost_threshold;
      c.s1 = -1;   // if negative values then invalid cell
      c.s2 = -1;
      //c.s1 = i*_delta*ca-(-j)*_delta_eta*sa;
      //c.s2 = i*_delta*sa+(-j)*_delta_eta*ca;
      c.prev_cell_coord.first = -1;
      c.valid = false;
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
    CD[i_start][j].valid = true;
    
    int i_end;
    // end point is for s1 = L1
    ksi = (L1-eta*sa)/ca;   // here eta is negative so + is made into -
    temp = ksi/_delta;
    //i_end = vcl_ceil(ksi/_delta);
    i_end = static_cast<int>(vcl_ceil(APPROX_INT(temp)));
    CD[i_end][j].s1 = L1;
    temp = ksi*sa-eta*ca;    // since eta is negative, sign is changed to - for s2
    //CD[i_end][j].s2 = ksi*sa-eta*ca;   // since eta is negative, sign is changed to - for s2  
    CD[i_end][j].s2 = (temp < L2 ? temp : L2);     
    CD[i_end][j].valid = true;
    
    CD[i_start][j].theta1 = _curve1->tangent_angle_at(CD[i_start][j].s1);
    CD[i_start][j].theta2 = _curve2->tangent_angle_at(CD[i_start][j].s2);

    CD[i_end][j].theta1 = _curve1->tangent_angle_at(CD[i_end][j].s1);
    CD[i_end][j].theta2 = _curve2->tangent_angle_at(CD[i_end][j].s2);

    // all other cells (i,j) have regular s1 and s2 values
    for (int i = i_start+1; i<i_end; i++) {
      CD[i][j].s1 = i*_delta*ca+eta*sa;
      CD[i][j].s2 = i*_delta*sa-eta*ca;
      CD[i][j].valid = true;

      CD[i][j].theta1 = _curve1->tangent_angle_at(CD[i][j].s1);
      CD[i][j].theta2 = _curve2->tangent_angle_at(CD[i][j].s2);
    }
  }

  // pick some arbitrary j and print s1 and s2 values on it on CD
  /*
  int j = 50;
  for (int i = 0; i<max_i; i++) {
    vcl_cout << "CD["<<i<<"]["<<j<<"].s1: " << CD[i][j].s1;
    vcl_cout << " CD["<<i<<"]["<<j<<"].s2: " << CD[i][j].s2 << vcl_endl;
  }*/
                            
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

//: update the cost of the cell, as if coming from cell: c_prev 
void dbcvr_interp_cvmatch::update_cost(dpt_cell *c, dpt_cell *c_prev, int i_prev, int j_prev) {
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

//: find optimum path 
void dbcvr_interp_cvmatch::find_path() {

  int i_start = 0;
  int j_start = 0;

  int i_end = max_i_-1;
  // j_end equals j_start always

  // check if the initialization was done appropriately for this 
  // starting point
  assert(CD[i_start][j_start].valid);
  assert(CD[i_end][j_start].valid);
  assert(CD[i_start][j_start].cost == 0);

  // try coming from upper irregular grid cells on s1 = 0 line
  for (int j = 0; j < max_j_up_-1; j++) {
    if (!CU[i_start][j].valid) break;
    come_previous(&(CU[i_start][j]), i_start, j+1);
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

  
 
}

//: the condition for being a valid previous point:
//  c_prev->s1 <= c->s1 
//  c_prev->s2 <= c->s2
void dbcvr_interp_cvmatch::come_previous(dpt_cell *c, int i, int j) {

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
    
    } else if (c->s1 == L1) {  // check down neighbor on s1 = L1 line on row i, others will be checked later below
                                 // only possible for negative j values
      if (CD[i][(-j)+1].s2 == L2 && CD[i][(-j)+1].valid) {
        come_previous(&CD[i][(-j)-1], i, j-1);
        update_cost(c, &CD[i][(-j)+1], i, j-1); 
      }
    }

    // here j might well be negative, or positive 
    // depending on the position of the cell in the grid (up cells, or down cells)
    float eta_up = static_cast<float>(j*_delta_eta + _delta*ta);
    float eta_down = static_cast<float>(j*_delta_eta - _delta/ta);

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

double dbcvr_interp_cvmatch::finalCost() {
  return CD[max_i_-1][0].cost;
}

void dbcvr_interp_cvmatch::Match() {
  
  // construct the grid, initialize the cost and valid fields of cells
  construct();

  //: initialize cost of starting cell to 0
  CD[0][0].cost = 0;

  //vcl_cout << "checking cost of CD[i_start][j_start], should be 0: " << 
  //              CD[start_cells_[0].first][start_cells_[0].second].cost << vcl_endl;
  
  find_path();
  vcl_cout << "Cost of best path for 0 is: " << finalCost() << vcl_endl;

  int m = 0;
  int s = max_i_-1;
  int count = 0;
  while (s > 0) {
    int s_prev, m_prev;
    if (m <= 0) {
      s_prev = CD[s][(-m)].prev_cell_coord.first;
      m_prev = CD[s][(-m)].prev_cell_coord.second;
      count++;
      out_curve1_.push_back(_curve1->point_at(CD[s][(-m)].s1));
      out_curve2_.push_back(_curve2->point_at(CD[s][(-m)].s2));
    } else {
      s_prev = CU[s][m-1].prev_cell_coord.first;
      m_prev = CU[s][m-1].prev_cell_coord.second;
      count++;
      out_curve1_.push_back(_curve1->point_at(CU[s][m-1].s1));
      out_curve2_.push_back(_curve2->point_at(CU[s][m-1].s2));
    }

    if (s_prev < 0) break;
    
    s = s_prev;
    m = m_prev;
  }

#if 1   // output to a file for debugging

  int count_saved = count;
 

  vcl_ofstream fpoo; 
  fpoo.open("D:\\contours\\Mpeg-7\\temp.out", vcl_ios::app);

  // find number of valid cells for this starting point
  count = 0;
  for (int i = 0; i<max_i_; i++) 
    for (int j = 0; j<max_j_up_-1; j++) 
      if (CU[i][j].valid) 
        count++;

  fpoo << count << "\n";
  for (int i = 0; i<max_i_; i++) {
    for (int j = 0; j<max_j_up_-1; j++) {   // -1 since (0,0) will go to CD
      if (CU[i][j].valid) {
        fpoo << CU[i][j].s1 << " ";
        fpoo << CU[i][j].s2 << vcl_endl;
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
        fpoo << CD[i][j].s2 << vcl_endl;
      }
    }
  }

  fpoo << count_saved << "\n";

  m = 0;
  s = max_i_-1;
  while (s > 0) {
    int s_prev, m_prev;
    if (m <= 0) {
      s_prev = CD[s][(-m)].prev_cell_coord.first;
      m_prev = CD[s][(-m)].prev_cell_coord.second;
      fpoo << CD[s][(-m)].s1 << " ";
      fpoo << CD[s][(-m)].s2 << vcl_endl;
    } else {
      s_prev = CU[s][m-1].prev_cell_coord.first;
      m_prev = CU[s][m-1].prev_cell_coord.second;
      fpoo << CU[s][m-1].s1 << " ";
      fpoo << CU[s][m-1].s2 << vcl_endl;
    }

    if (s_prev < 0) break;
    
    s = s_prev;
    m = m_prev;
  }

  fpoo.close();
#endif
}

