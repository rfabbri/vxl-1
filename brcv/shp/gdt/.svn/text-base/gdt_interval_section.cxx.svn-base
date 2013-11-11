//: Aug 19, 2005 MingChing Chang
//  

#include <assert.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <gdt/gdt_solve_intersect.h>
#include <gdt/gdt_interval_section.h>
#include <gdt/gdt_edge.h>

//: remove and release memory of all I's of this map,
//  Don't release memory of the one with i_invalid_on_front!=0,
//  Instead, set the flag to be 2 (it will be deleted when poped from the front).
//  It will be deleted when poped up from the front.
//
void gdt_interval_section::clear_I_map ()
{
  vcl_map<double, gdt_ibase*>::iterator it = I_map_.begin();
  while (it != I_map_.end()) {
    gdt_ibase* I = (*it).second;
    
    if (I->type() == ITYPE_DUMMY) {
      delete I;
    }
    else { //special handling.
      gdt_interval* II = (gdt_interval*) I;
#if GDT_ALGO_F
      assert (!II->b_attach_to_edge());
#endif
      _delete_interval_handle_front (II);
    }

    I_map_.erase (it);
    it = I_map_.begin();
  }
}

//: Brute-forcely go through each interval and check for possible overlap.
bool gdt_interval_section::_is_I_overlap (const gdt_ibase* input_I)
{
  vcl_map<double, gdt_ibase*>::iterator it = I_map_.begin();
  for (; it != I_map_.end(); it++) {
    gdt_ibase* I = (*it).second;
    if (_is_I_overlapping (I, input_I))
      return true;
  }

  return false;
}

//: Brute-forcely go through each interval and check for possible overlap.
//  Ignore input_I itself.
bool gdt_interval_section::_is_I_overlap2 (const gdt_ibase* input_I)
{
  vcl_map<double, gdt_ibase*>::iterator it = I_map_.begin();
  for (; it != I_map_.end(); it++) {
    gdt_ibase* I = (*it).second;
    if (I == input_I)
      continue;
    if (_is_I_overlapping (I, input_I))
      return true;
  }

  return false;
}

bool gdt_interval_section::is_a_coverage ()
{
  if (I_map()->size() == 0)
    return false;

  //:1) the first interval
  vcl_map<double, gdt_ibase*>::iterator fit = I_map_.begin();
  gdt_ibase* I_1st = (*fit).second;
  if (I_1st->stau() != 0)
    return false;

  //:2) the last I
  vcl_map<double, gdt_ibase*>::reverse_iterator rit = I_map_.rbegin();
  gdt_ibase* I_last = (*rit).second;
  if (I_last->etau() != len_)
    return false;

  //:3) if size > 1
  if (I_map()->size() > 1) {
    vcl_map<double, gdt_ibase*>::iterator fitn = fit;
    fitn++;
    gdt_ibase* I_2nd = (*fitn).second;
    if (I_1st->etau() != I_2nd->stau())
      return false;

    vcl_map<double, gdt_ibase*>::reverse_iterator ritn = rit;
    ritn++;
    gdt_ibase* I_r2nd = (*ritn).second;
    if (I_last->stau() != I_r2nd->etau())
      return false;
  }

  //:3) all the others
  if (I_map_.size() > 2) {
    vcl_map<double, gdt_ibase*>::iterator pit = I_map_.begin();
    vcl_map<double, gdt_ibase*>::iterator it = pit;
    it++;
    vcl_map<double, gdt_ibase*>::iterator nit = it;
    nit++;
  
    for (; nit != I_map_.end(); pit++, it++, nit++) {
      gdt_ibase* prev = (*pit).second;
      gdt_ibase* cur = (*it).second;
      gdt_ibase* next = (*nit).second;

      if (cur->stau() != prev->etau())
        return false;
      if (cur->etau() != next->stau())
        return false;
    }
  }

  return true;
}

//: if all correct, return true
bool gdt_interval_section::assert_coverage_no_gap_overlap ()
{
  if (I_map()->size() == 0)
    return true;

  //:1) the first I
  vcl_map<double, gdt_ibase*>::iterator fit = I_map()->begin();
  gdt_ibase* I_1st = (*fit).second;
  assert (I_1st->stau() == 0);

  //:2) the last I
  vcl_map<double, gdt_ibase*>::reverse_iterator rit = I_map()->rbegin();
  gdt_ibase* I_last = (*rit).second;
  assert (I_last->etau() == len_);

  //:3) if size > 1
  if (I_map()->size() > 1) {
    vcl_map<double, gdt_ibase*>::iterator fitn = fit;
    fitn++;
    gdt_ibase* I_2nd = (*fitn).second;
    assert (I_1st->etau() == I_2nd->stau());

    vcl_map<double, gdt_ibase*>::reverse_iterator ritn = rit;
    ritn++;
    gdt_ibase* I_r2nd = (*ritn).second;
    assert (I_last->stau() == I_r2nd->etau());
  }

  //:3) all the others
  if (I_map()->size() > 2) {
    vcl_map<double, gdt_ibase*>::iterator pit = I_map()->begin();
    vcl_map<double, gdt_ibase*>::iterator it = pit;
    it++;
    vcl_map<double, gdt_ibase*>::iterator nit = it;
    nit++;
  
    for (; nit != I_map()->end(); pit++, it++, nit++) {
      gdt_ibase* prev = (*pit).second;
      gdt_ibase* cur = (*it).second;
      gdt_ibase* next = (*nit).second;

      assert (cur->stau() == prev->etau());
      assert (cur->etau() == next->stau());
    }
  }
  return true;
}

void gdt_interval_section::debug_print ()
{
  vul_printf (vcl_cerr, "\n IntervalSection: \n");
  vcl_map<double, gdt_ibase*>::iterator it = I_map_.begin();
  for (int i=0; it != I_map_.end(); i++, it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    vul_printf (vcl_cerr, "    I %d: (%.16f - %.16f) vs: %d", 
                i, I->stau(), I->etau(), I->psrc()->id());
    if (I->is_dege())
      vul_printf (vcl_cerr, " dege");
    vul_printf (vcl_cerr, "\n");
  }
}

// #################  Modification Functions  #################

//: remove all intervals whose stau and etau < input_stau
//  if the interval I < input_tau, delete it directly.
//  else, input_tau is between its (stau, etau), trim it
void gdt_interval_section::delete_tau_less_than (double input_tau) 
{
  //: delete each interval I if I(stau, etau) < input_tau
  gdt_interval* I=NULL;
  vcl_map<double, gdt_ibase*>::iterator it = I_map_.begin();
  while (it != I_map_.end()) {
    I = (gdt_interval*) (*it).second;

    if (I->stau() < input_tau && I->etau() <= input_tau) {
      I_map_.erase (it);
      it = I_map_.begin();
      delete I;
    }
    else
      break;
  }

  //: if trimming needed for the last interval I covering input_tau
  if (I->stau() < input_tau) {
    if (_eqT(input_tau, I->etau()) == false) {
      I_map_.erase (it);
      I->_set_stau (input_tau);
      _add_interval (I);
    }
    else {
      //the I is too small to trim, should delete I and modify the next_I
      vcl_map<double, gdt_ibase*>::iterator nit = it;
      nit++;
      I_map_.erase (it);
      gdt_interval* next_I = (gdt_interval*) (*nit).second;
      I_map_.erase (nit);
      next_I->_set_stau (input_tau);
      _add_interval (next_I);
    }
  }
}

//: remove all intervals whose stau and etau > input_etau
//  if the interval I > input_tau, delete it directly.
//  else, input_tau is between its (stau, etau), trim it.
void gdt_interval_section::delete_tau_greater_than (double input_tau) 
{
  //: delete all I > input_tau
  gdt_interval* I=NULL;
  vcl_map<double, gdt_ibase*>::reverse_iterator rit = I_map_.rbegin();
  while (rit != I_map_.rend()) {
    I = (gdt_interval*) (*rit).second;

    if (I->stau() >= input_tau && I->etau() > input_tau) {
      I_map_.erase (I->stau());
      rit = I_map_.rbegin();
      delete I;
    }
    else
      break;
  }

  //: if trimming needed for the last interval I covering input_tau
  if (I->etau() > input_tau) {
    if (_eqT(I->stau(), input_tau) == false) {
      I->_set_etau (input_tau);
    }
    else {
      // the I is too small to trim, should delete I and modify the next_I
      vcl_map<double, gdt_ibase*>::reverse_iterator rnit = rit;
      rnit++;
      I_map_.erase (I->stau());
      gdt_interval* next_I = (gdt_interval*) (*rnit).second;
      next_I->_set_etau (input_tau);
      assert (!_eqT (next_I->stau(), next_I->etau()));
    }
  }
}

//: do a brute-force numerical check and add to I_map
void gdt_interval_section::add_interval_numfix (gdt_ibase* input_I)
{
  //: if any existing I's etau == input_I's stau, fix it
  vcl_map<double, gdt_ibase*>::iterator it = I_map_.begin();
  for (; it != I_map_.end(); it++) {
    gdt_ibase* I = (*it).second;
    //: if I->stau() greater than input_I's stau, enough
    if (I->stau() > input_I->stau())
      break;

    if (_eqT (I->etau(), input_I->stau())) {
      I->_set_etau (input_I->stau());
      assert (!_eqT (I->stau(), I->etau()));
      break;
    }
  }

  //: if any existing I's stau == input_I's etau, fix it
  vcl_map<double, gdt_ibase*>::reverse_iterator rit = I_map_.rbegin();
  for (; rit != I_map_.rend(); rit++) {
    gdt_ibase* I = (*rit).second;
    //: if I->stau() greater than input_I's stau, enough
    if (I->stau() < input_I->stau())
      break;

    if (_eqT (I->stau(), input_I->etau())) {
      input_I->_set_etau (I->stau());
      break;
    }
  }
  assert (!_eqT (input_I->stau(), input_I->etau()));

  //assert (I_map->_find_interval(input_I->stau()) == NULL);
  I_map_.insert (vcl_pair<double, gdt_ibase*>(input_I->stau(), input_I));
}

// #################  Intersection Functions  #################

//: fill the wff_map with dummy intervals to form a coverage on it.
void gdt_interval_section::fill_dummy_intervals ()
{
  //:1) if no existing interval.
  if (I_map_.size() == 0) {
    //: create one full dummy interval.
    gdt_ibase* dum = new gdt_ibase (ITYPE_DUMMY, 0, len_);
    I_map_.insert (vcl_pair<double, gdt_ibase*>(dum->stau(), dum));
    return;
  }

  //:2) fill the middle gaps
  vcl_map<double, gdt_ibase*>::iterator it = I_map_.begin();
  for (int i=0; i < (int) I_map_.size()-1; i++, it++) {
    vcl_map<double, gdt_ibase*>::iterator nit = it;
    nit++;
    gdt_ibase* cur_I = (*it).second;
    gdt_ibase* next_I = (*nit).second;

    if (cur_I->etau() < next_I->stau()) {
      gdt_ibase* dum = new gdt_ibase (ITYPE_DUMMY, cur_I->etau(), next_I->stau());
      I_map_.insert (vcl_pair<double, gdt_ibase*>(dum->stau(), dum));
    }
  }

  //: fill the first gap: (0 - 1st stau)
  it = I_map_.begin();
  gdt_ibase* I = (*it).second;
  if (I->stau() != 0) {
    gdt_ibase* dum = new gdt_ibase (ITYPE_DUMMY, 0, I->stau());
    I_map_.insert (vcl_pair<double, gdt_ibase*>(dum->stau(), dum));
  }

  //: fill the last gap: (last etau - len_)
  vcl_map<double, gdt_ibase*>::reverse_iterator rit = I_map_.rbegin();
  I = (*rit).second;
  if (I->etau() != len_) {
    gdt_ibase* dum = new gdt_ibase (ITYPE_DUMMY, I->etau(), len_);
    I_map_.insert (vcl_pair<double, gdt_ibase*>(dum->stau(), dum));
  }
}

//: the graph of the distance function on each I is a hyperbola.
//  this function tries to find the min of two possibly intersecting hyperbolas
//  on the interval of (itau_min, itau_max), H1 for I1 and H2 for I2.
//  3 cases are possible:
//  1) no intersection: either H1 or H2 is minimal
//                      test distance of (itau_min+itau_max)/2 to decide which one is min.
//  2) one intersection (ptau):
//                      either H1(itau_min, ptau) + H2(ptau, itau_max) is min, or
//                             H2(itau_min, ptau) + H1(ptau, itau_max) is min
//                      test distance of (itau_min+ptau)/2 to decide which case.
//  3) two intersections (ptau1, ptau2):
//                      either H1(itau_min, ptau1) + H2(ptau1, ptau2) + H1(ptau2, itau_max) or
//                      either H2(itau_min, ptau1) + H1(ptau1, ptau2) + H2(ptau2, itau_max)
//
//  Note: 1) the min of H1 and H2 interchanges between intersections.
//           So we can test any point (with some mid_tau) on both I to see which one is min.
//        2) If intersection happens, need to create additional intervals on the I_map.
//        3) The default b_wss1_min and b_wss2_min is true,
//           only need to modify one of them for case 1) no intersection.
//        4) degenerate I can be handled in the same way, too.
//
void gdt_interval_section::clone_intersect_overlap (gdt_interval* I1, gdt_interval* I2, 
                                                    double itau_min, double itau_max,
                                                    bool& b_wss1_min, bool& b_wss2_min)
{
  assert (itau_min+GDT_TAU_EPSILON_DIV < itau_max);

  //:I) solve the intersection of two hyperbolas for ptau
  double l1 = I1->L();
  double l2 = I2->L();
  double h1 = I1->H();
  double h2 = I2->H();
  
  //: solve for ptau that satisfying itau_min < ptau < itau_max
  //  At most two solutions, assuming the two solutions: ptau1 <= ptau2
  double ptau1, ptau2;
  bool   b_ptau1_valid = true;
  bool   b_ptau2_valid = true;

  if (_eqT(l1,l2) && _eqT(h1, h2)) { //: if h1==h2 and l1==l2, no solution
    ///assert (beta==0); ???? need to check!!
    b_ptau1_valid = false;
    b_ptau2_valid = false;
  }
  else { //: try to solve for up to two solution.   
    double alpha = l2 - l1;
    double beta = I2->mu() - I1->mu();
    double gamma = l1*l1 + h1*h1 - l2*l2 - h2*h2 - beta*beta;

    //: if beta==0, B*B-4AC is 0.
    double A = alpha*alpha - beta*beta;
    double B = gamma*alpha + l2*beta*beta*2;
    double C = gamma*gamma*0.25 - (l2*l2+h2*h2)*beta*beta;

    if (_solve_quad (A, B, C, ptau1, ptau2)) {
      //: if ptau1 == ptau2 (one solution), make one of them -1
      if (_eqT (ptau1, ptau2)) {
        if (itau_min<=ptau1 && ptau1<=itau_max) {
          b_ptau2_valid = false;
          if (itau_min<=ptau2 && ptau2<=itau_max)
            ptau1 = (ptau1 + ptau2)/2;
        }
        else {
          b_ptau1_valid = false;
          if (itau_min>ptau2 || ptau2>itau_max)
            b_ptau2_valid = false;
        }
      }

      //: test the two sqrt-root conditions, if beta !=0 and the solution is still valid
      //  In beta==0 case, the constaints do not apply.
      if (beta!=0) {
        if (b_ptau1_valid) {
          if ((gamma+2*alpha*ptau1)/beta < 0)
            b_ptau1_valid = false;
          if ((gamma+2*beta*beta+2*alpha*ptau1)/beta <0)
            b_ptau1_valid = false;
        }

        if (b_ptau2_valid) {
          if ((gamma+2*alpha*ptau2)/beta < 0)
            b_ptau2_valid = false;
          if ((gamma+2*beta*beta+2*alpha*ptau2)/beta <0)
            b_ptau2_valid = false;
        }
      }

      //: test if ptau1 not in the valid range
      if (itau_min+GDT_TAU_EPSILON > ptau1 || ptau1+GDT_TAU_EPSILON > itau_max)
        b_ptau1_valid = false;

      //: test if ptau2 not in the valid range
      if (itau_min+GDT_TAU_EPSILON > ptau2 || ptau2+GDT_TAU_EPSILON > itau_max)
        b_ptau2_valid = false;       

      //: test distance of ptau1, if test fail, the solution is invalid.
      if (b_ptau1_valid) {
        double d1 = I1->mu() + sqrt( (l1-ptau1)*(l1-ptau1)+h1*h1 );
        double d2 = I2->mu() + sqrt( (l2-ptau1)*(l2-ptau1)+h2*h2 );
        if (_eqD (d1, d2) == false) {
          b_ptau1_valid = false;
          ////assert (0);
        }
      }

      //: test distance of ptau2, if test fail, the solution is invalid.
      if (b_ptau2_valid) {
        double d1 = I1->mu() + sqrt( (l1-ptau2)*(l1-ptau2)+h1*h1 );
        double d2 = I2->mu() + sqrt( (l2-ptau2)*(l2-ptau2)+h2*h2 );
        if (_eqD (d1, d2) == false) {
          b_ptau2_valid = false;
          ////assert (0);
        }
      }
    }
    else { //no solution
      b_ptau1_valid = false;
      b_ptau2_valid = false;
    }
  }

  //////////////////////////////////////////////////////////////////////////
  //:II) decide case and handle degeneracy and numerical issues for each case
  if (b_ptau1_valid) {
    assert (_lessT(itau_min, ptau1) && _lessT(ptau1, itau_max));
    if (b_ptau2_valid) {
      assert (_lessT(itau_min, ptau2) && _lessT(ptau2, itau_max));
      //: Case A) both solutions ptau1 and ptau2 exist
      //          assume ptau1 is the smaller solution.
      //          test distance of (itau_min+ptau1)/2 to decide which one is min.
      if (ptau1 > ptau2)
        _swap (ptau1, ptau2);

      double a =  (itau_min+ptau1)/2;
      double b =  (ptau1+ptau2)/2;
      double c =  (ptau2+itau_max)/2;
      double dista1 = I1->_get_d_at_tau (a) + I1->mu();
      double dista2 = I2->_get_d_at_tau (a) + I2->mu();
      double distb1 = I1->_get_d_at_tau (b) + I1->mu();
      double distb2 = I2->_get_d_at_tau (b) + I2->mu();
      double distc1 = I1->_get_d_at_tau (c) + I1->mu();
      double distc2 = I2->_get_d_at_tau (c) + I2->mu();

      if (_eqD (dista1, dista2) &&
          _eqD (distb1, distb2) &&
          _eqD (distc1, distc2)) {
        //: the same as the 'all solution' case, dista1==dista2
        b_wss1_min = false;
        b_wss2_min = false;
        clone_add_partial (I2, itau_min, itau_max);
        return;
      }
      else
        //: not sure if this is needed!
        assert (0);
        return;

      /*if (dista1 < dista2) {
        clone_add_partial (I1, itau_min, ptau1);
        clone_add_partial (I2, ptau1, ptau2);
        clone_add_partial (I1, ptau2, itau_max);
      }
      else {
        clone_add_partial (I2, itau_min, ptau1);
        clone_add_partial (I1, ptau1, ptau2);
        clone_add_partial (I2, ptau2, itau_max);
      }*/
    }
    else {
      //: Case B) only solution ptau1 exists. Be careful on the numerical issues.
      //          test distance of a=(itau_min+ptau1)/2 and b=(ptau1+itau_max)/2 to decide which one is min.
      double a =  (itau_min+ptau1)/2;
      double dista1 = I1->_get_d_at_tau (a) + I1->mu();
      double dista2 = I2->_get_d_at_tau (a) + I2->mu();

      //: to double check
      double b =  (ptau1+itau_max)/2;
      double distb1 = I1->_get_d_at_tau (b) + I1->mu();
      double distb2 = I2->_get_d_at_tau (b) + I2->mu();

      if (_eqD (dista1, dista2)) {
        //: test distb1 and distb2
        if (_eqD (distb1, distb2)) {
          //: the same as the 'all solution' case, dista1==dista2
          b_wss1_min = false;
          b_wss2_min = false;
          clone_add_partial (I2, itau_min, itau_max);
          return;
        }
        else if (distb1 < distb2) {
          clone_add_partial (I1, ptau1, itau_max); 
          clone_add_partial (I2, itau_min, ptau1); //I2 more accurate in most cases
          return;
        }
        else {
          clone_add_partial (I2, itau_min, itau_max);
          b_wss1_min = false;
          return;
        }
      }
      else if (dista1 < dista2) {
        if (_eqD (distb1, distb2)) {
          clone_add_partial (I1, itau_min, ptau1);
          clone_add_partial (I2, ptau1, itau_max); //I2 more accurate in most cases
          return;
        }
        else if (distb1 > distb2) {
          //: the most happened case.
          clone_add_partial (I1, itau_min, ptau1);
          clone_add_partial (I2, ptau1, itau_max);
          return;
        }
        else {
          clone_add_partial (I1, itau_min, itau_max);  //the rare tangent case.
          b_wss2_min = false;
          return;
        }
      }
      else { //dista1 > dista2

        if (_eqD (distb1, distb2)) {
          clone_add_partial (I2, itau_min, itau_max); //I1 more accurate, but keep I2
          b_wss1_min = false;
          return;
        }
        else if (distb1 < distb2) {
          //: the most happened case.
          clone_add_partial (I1, ptau1, itau_max);
          clone_add_partial (I2, itau_min, ptau1);
          return;
        }
        else {
          clone_add_partial (I2, itau_min, itau_max);  //the rare tangent case.
          b_wss1_min = false;
          return;
        }
      }
    }
  }
  else {
    if (b_ptau2_valid) {
      assert (_lessT(itau_min, ptau2) && _lessT(ptau2, itau_max));
      //: Case C) only solution ptau2 exists. Be careful on the numerical issues.
      //          test distance of a=(itau_min+ptau2)/2 and b=(ptau1+itau_max)/2 to decide which one is min.
      double a =  (itau_min+ptau2)/2;
      double dista1 = I1->_get_d_at_tau (a) + I1->mu();
      double dista2 = I2->_get_d_at_tau (a) + I2->mu();

      //: to double check
      double b =  (ptau2+itau_max)/2;
      double distb1 = I1->_get_d_at_tau (b) + I1->mu();
      double distb2 = I2->_get_d_at_tau (b) + I2->mu();

      if (_eqD (dista1, dista2)) {
        if (_eqD (distb1, distb2)) {
          //: the same as the 'all solution' case, dista1==dista2
          b_wss1_min = false;
          b_wss2_min = false;
          clone_add_partial (I2, itau_min, itau_max);
          return;
        }
        else if (distb1 < distb2) {
          clone_add_partial (I1, ptau2, itau_max);
          clone_add_partial (I2, itau_min, ptau2); //I2 more accurate in most cases
          return;
        }
        else {
          clone_add_partial (I2, itau_min, itau_max); //I1 more accurate, but keep I2
          b_wss1_min = false;
          return;
        }
      }
      else if (dista1 < dista2) {

        if (_eqD (distb1, distb2)) {
          clone_add_partial (I1, itau_min, ptau2);
          clone_add_partial (I2, ptau2, itau_max); //I2 more accurate in most cases
          return;
        }
        else if (distb1 > distb2) {
          //: the most happened case.
          clone_add_partial (I1, itau_min, ptau2);
          clone_add_partial (I2, ptau2, itau_max);
          return;
        }
        else {
          clone_add_partial (I1, itau_min, itau_max); //the rare tangent case. 
          b_wss2_min = false;
          return;
        }
      }
      else { //dista1 > dista2
        if (_eqD (distb1, distb2)) {
          clone_add_partial (I2, itau_min, itau_max); //I1 more accurate, but keep I2
          b_wss1_min = false;
          return;
        }
        else if (distb1 < distb2) {
          //: the most happened case.
          clone_add_partial (I1, ptau2, itau_max);
          clone_add_partial (I2, itau_min, ptau2);
          return;
        }
        else {
          clone_add_partial (I2, itau_min, itau_max); //the rare tangent case.
          b_wss1_min = false;
          return;
        }
      }
    }
    else {
      //: Case D) no solution exists.
      //          test distance of (itau_min+itau_max)/2 to decide which one is min.
      double a =  (itau_min+itau_max)/2;
      double dista1 = I1->_get_d_at_tau (a) + I1->mu();
      double dista2 = I2->_get_d_at_tau (a) + I2->mu();

      if (_eqD (dista1, dista2)) { 
        //: the 'all solution' case, dista1==dista2,
        //  degenerate case of two hyperbolas coinside, dista1==dista2
        b_wss1_min = false;
        b_wss2_min = false;
        clone_add_partial (I2, itau_min, itau_max);
        return;
      }
      else if (dista1 < dista2) {
        b_wss2_min = false;
        clone_add_partial (I1, itau_min, itau_max);
        return;
      }
      else {
        b_wss1_min = false;
        clone_add_partial (I2, itau_min, itau_max);
        return;
      }
    }
  }
}

//: loop through each I of I_map and do
//  1) merge adjacent I's if they are the same 
//     (by exact comparison of their pseudo-source and h_ and l_)
//  2) fix numerical gap
void gdt_interval_section::merge_same_intervals ()
{
  if (I_map_.size() < 2)
    return; //no need to merge

  vcl_map<double, gdt_ibase*>::iterator cit = I_map_.begin();
  vcl_map<double, gdt_ibase*>::iterator nit = cit;
  nit++;

  gdt_interval* cur_I = (gdt_interval*) (*cit).second;
  gdt_interval* next_I = (gdt_interval*) (*nit).second;

  //: if the first one is too small merge with the next
  if (_eqT (cur_I->stau(), cur_I->etau())) {
    I_map_.erase (cit);
    I_map_.erase (nit);

    next_I->_set_stau (cur_I->stau());
    delete cur_I;
    _add_interval (next_I);

    if (I_map_.size()<2)
      return;

    cit = I_map_.begin();
    nit = cit;
    nit++;
  }

  //: compare and try to merge
  while (nit!=I_map_.end()) {
    cur_I = (gdt_interval*) (*cit).second;
    next_I = (gdt_interval*) (*nit).second;

    //: fix numerical gap, here the epsilon should be 2* GDT_TAU_EPSILON
    if (vcl_fabs (cur_I->etau()-next_I->stau()) < GDT_TAU_EPSILON*2) {
      cur_I->_set_etau (next_I->stau());
    }

    //:1) if next_I too small, merge with cur_I
    if (_eqT (next_I->stau(), next_I->etau())) {
      //: extend cur_I
      assert (cur_I->etau() < next_I->etau());
      cur_I->_set_etau (next_I->etau());
      delete next_I;
      vcl_map<double, gdt_ibase*>::iterator temp = nit;
      temp--;
      I_map_.erase (nit);
      temp++;
      nit = temp;
    }
    //:2) if cur_I and next_I should be merged
    else if (cur_I->prevI() == next_I->prevI() &&
             cur_I->psrc() == next_I->psrc() &&
             cur_I->L() == next_I->L() &&
             cur_I->H() == next_I->H()) {
      //: extend cur_I
      assert (cur_I->etau() < next_I->etau());
      cur_I->_set_etau (next_I->etau());

      //: delete next_I
      delete next_I;
      vcl_map<double, gdt_ibase*>::iterator temp = nit;
      temp--;
      I_map_.erase (nit);
      temp++;
      nit = temp;
    }
    else {
      //: advance the cur_I and next_I
      cit = nit;
      nit++;
    }
  }

}

void gdt_interval_section::fix_boundary_intervals (gdt_interval_section* IS1, 
                                                   gdt_interval_section* IS2)
{
  //:1) fix the beginning tau
  vcl_map<double, gdt_ibase*>::iterator it1 = IS1->I_map()->begin();
  gdt_ibase* I1 = (*it1).second;
  vcl_map<double, gdt_ibase*>::iterator it2 = IS2->I_map()->begin();
  gdt_ibase* I2 = (*it1).second;
  double tau_min = vcl_min (I1->stau(), I2->stau());

  vcl_map<double, gdt_ibase*>::iterator it = I_map_.begin();
  gdt_ibase* result_I = (*it).second;
  if (result_I->stau() != tau_min && _eqT(result_I->stau(), tau_min)) {
    //stick stau to tau_min
    I_map_.erase (it);
    result_I->_set_stau (tau_min);
    _add_interval (result_I);
  }

  //:2) fix the ending tau
  vcl_map<double, gdt_ibase*>::reverse_iterator rit1 = IS1->I_map()->rbegin();
  I1 = (*rit1).second;
  vcl_map<double, gdt_ibase*>::reverse_iterator rit2 = IS2->I_map()->rbegin();
  I2 = (*rit2).second;
  double tau_max = vcl_max (I1->etau(), I2->etau());

  vcl_map<double, gdt_ibase*>::reverse_iterator rit = I_map_.rbegin();
  result_I = (*rit).second;
  if (result_I->etau() != tau_max && _eqT(result_I->etau(), tau_max)) {
    //stick etau to tau_max
    result_I->_set_etau (tau_max);
  }
}

// ########################################################################

//: return the result of intersection of two I's (numerically with epsilon)
//  0: both I's are dummy or no intersection
//  1: I1 is the non-dummy interval after intersection
//  2: I2 is the non-dummy interval after intersection
//  3: both are non-dummy
//  the intersecting interval is (itau_min, itau_max)
//
int _intersection_tau (gdt_ibase* I1, gdt_ibase* I2, 
                       double& itau_min, double& itau_max)
{
  //: check overlap first
  itau_min = vcl_max (I1->stau(), I2->stau());
  itau_max = vcl_min (I1->etau(), I2->etau());

  //: if the overlap is too small, ignore it.
  //  the epsilon here has to be smaller
  if (itau_min+GDT_TAU_EPSILON_DIV > itau_max)
    return 0;

  //: if intersection exists, get the non-dummy intersection
  if (I1->is_dummy()) {
    if (I2->is_dummy())
      return 0;
    else
      return 2;
  }
  else {
    if (I2->is_dummy())
      return 1;
    else
      return 3;
  }
}

//: merge the propagation of interval sections IS1 and IS2 to result_IS.
//  return 1: only wss_map1 contrib to result_IS (no merge)
//  return 2: only wss_map2 contrib to result_IS (no merge)
//  return 3: both maps contrib to result_IS but just moving is required (both inpputs are mutual exclusive).
//  result 4: both maps contrib to result_IS and a merge/intersection is required.
//  return 0: (degenerate) neither IS1 nor IS2 contribs.
//  Note: 1) degenerate I can be handled in the same way, too.
//        2) dummy intervals should be added to form a coverage of each map,
//           so all merges can be done in one big loop.
//
INTERSECT_RESULT _intersect_IS (gdt_interval_section* IS1,
                                gdt_interval_section* IS2,
                                gdt_interval_section* result_IS)
{
  bool b_map1_contrib = false;
  bool b_map2_contrib = false;
  bool b_merge_performed = false;

  //:1) loop through each wss_i on wss_map1 and wss_j on wss_map1
  //    focus on the intersection of wss_i and wss_j,
  //    find the minimal of d(wss_i) and d(wss_j)
  //    the result may be at most 3 I's.
  vcl_map<double, gdt_ibase*>::iterator it1 = IS1->I_map()->begin();
  for (; it1 != IS1->I_map()->end(); it1++) {
    gdt_ibase* I1 = (*it1).second;

    vcl_map<double, gdt_ibase*>::iterator it2 = IS2->I_map()->begin();
    for (; it2 != IS2->I_map()->end(); it2++) {
      gdt_ibase* I2 = (*it2).second;
      
      //: 1-1) check intersection
      double itau_min, itau_max;
      int intersect = _intersection_tau (I1, I2, itau_min, itau_max);
      switch (intersect) {
      case 0:
        //: no intersection, skip.
      break;
      case 1:
        //: clone the intersection interval to the result_wss_map
        result_IS->clone_add_partial (I1, itau_min, itau_max);
        b_map1_contrib = true;
      break;
      case 2:
        //: clone the intersection interval to the result_wss_map
        result_IS->clone_add_partial (I2, itau_min, itau_max);
        b_map2_contrib = true;
      break;
      case 3:
      {
        //: merge intersection of I1 and I2 at the interval of (itau_min, itau_max)
        //  and store resulting I(s) on result_IS->I_map()
        //  also change the flag if I1 or I2 contribs to the minimal.
        bool b_wss1_min = true;
        bool b_wss2_min = true;
        result_IS->clone_intersect_overlap ((gdt_interval*) I1, (gdt_interval*) I2, 
                                            itau_min, itau_max, b_wss1_min, b_wss2_min);
        b_map1_contrib = b_wss1_min || b_map1_contrib;
        b_map2_contrib = b_wss2_min || b_map2_contrib;
        b_merge_performed = true;
      }
      break;
      default:
        assert (0);
      break;
      }
    }
  }

  //:2) Go through result_edge and try to merge the same invervals
  //    Can improve efficiency by reducing fragment of intervals.
  result_IS->merge_same_intervals ();

  //:3) If the result_IS's boundary is very close to IS1 or IS2, stick to it
  result_IS->fix_boundary_intervals (IS1, IS2);

  if (b_map1_contrib) {
    if (b_map2_contrib) {
      if (b_merge_performed)
        return INTERSECT_RESULT_3;
      else
        return INTERSECT_RESULT_4;
    }
    else
      return INTERSECT_RESULT_1;
  }
  else {
    if (b_map2_contrib)
      return INTERSECT_RESULT_2;
    else {
      return INTERSECT_RESULT_0;
    }
  }
}

// #####################################################################

//: Intersect an interval to an interval_section
//  Here we are trying to get the minumum of two hyperbolas min(h1, h2)
//  The result is an interval_section, result_IS.
//  return 1: only the input_I contribs to the min (no merge)
//  return 2: only the existing interval_section contribs (no merge)
//  return 3: both contrib to the min (a merge)
//  return 0: the two are the same hyperbola (degenerate)

INTERSECT_RESULT _intersect_I_IS (gdt_interval* input_I,
                                  gdt_interval_section* input_IS2,
                                  gdt_interval_section* result_IS)
{
#if GDT_DEBUG_MSG
  
  #if GDT_ALGO_F
  assert (input_I->b_attach_to_edge() == false);
  assert (input_I->prev_flag() == false);
  assert (input_I->nextIs().size() == 0);
  #endif

  gdt_interval* prev_I = NULL;
  vcl_map<double, gdt_ibase*>::iterator it = input_IS2->I_map()->begin();
  for (; it != input_IS2->I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;
    if (prev_I && _eqT(prev_I->etau(), I->stau()))
      assert (prev_I->etau() == I->stau());
    prev_I = I;

    #if GDT_ALGO_F
    assert (I->prev_flag() == false);
    assert (I->nextIs().size() == 0);
    #endif
  }
  assert (result_IS->I_map()->size()==0);
#endif

  //: create a temporary IS1 to store the input_I and fill gaps with dummy intervals.
  gdt_interval_section IS1 (input_IS2->len());
  IS1._add_interval (input_I);
  IS1.fill_dummy_intervals ();

  input_IS2->fill_dummy_intervals ();

  //: intersect the two maps and store the result back to I_map_
  INTERSECT_RESULT result = _intersect_IS (&IS1, input_IS2, result_IS);
  return result;
}


//: Use the above one with input_IS2 = result_IS.
INTERSECT_RESULT intersect_I_with_IS (gdt_interval* I, gdt_interval_section* IS)
{
  gdt_interval_section IS_existing (IS->len());
  move_intervals (IS, &IS_existing);
  return _intersect_I_IS (I, &IS_existing, IS);
}

//: intersect two interval_sections, input_IS1, input_IS2.
//  Here we are trying to get the minumum of two hyperbola sections min(h1, h2)
//  The result is an interval_section in result_IS.
//  Assumption: 1) all intervals in input_IS1 and input_IS2 are indep. of both-way pointers.
//                 since some dummy intervals will be added and temp will be deteled.
//              2) input_IS1 != input_IS2 != result_IS
//  return 1: only the first one contribs to the min (no merge)
//  return 2: only the second one contribs to the min (no merge)
//  return 3: both contrib to the min (a merge)
//  return 0: the two are the same hyperbola (degenerate)
//
INTERSECT_RESULT intersect_interval_sections (gdt_interval_section* input_IS1,
                                              gdt_interval_section* input_IS2,
                                              gdt_interval_section* result_IS)
{
#if GDT_DEBUG_MSG
  //: debug
  assert (input_IS2 != result_IS);
  gdt_interval* prev_I = NULL;
  vcl_map<double, gdt_ibase*>::iterator it = input_IS1->I_map()->begin();
  for (; it != input_IS1->I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;
    if (prev_I && _eqT(prev_I->etau(), I->stau()))
      assert (prev_I->etau() == I->stau());
    prev_I = I;

    #if GDT_ALGO_F
    assert (I->prev_flag() == false);
    assert (I->nextIs().size() == 0);
    #endif
  }
  prev_I = NULL;
  it = input_IS2->I_map()->begin();
  for (; it != input_IS2->I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;
    if (prev_I && _eqT(prev_I->etau(), I->stau()))
      assert (prev_I->etau() == I->stau());
    prev_I = I;

    #if GDT_ALGO_F
    assert (I->prev_flag() == false);
    assert (I->nextIs().size() == 0);
    #endif
  }
  assert (result_IS->I_map()->size()==0);
#endif

  //  Fill both IS's gaps with dummy intervals.
  input_IS1->fill_dummy_intervals ();
  input_IS2->fill_dummy_intervals ();

  //: intersect the two maps and store the result back to I_map_
  return _intersect_IS (input_IS1, input_IS2, result_IS);
}



