// This is algo/bcdg/bcdg_frame.cxx
//: \file

#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vsol/vsol_point_2d.h>
#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_episeg_from_curve_converter.h>
#include <dbecl/dbecl_episeg_point.h>

#include "bcdg_globals.h"
#include "bcdg_frame.h"

//: Constructor
bcdg_frame::bcdg_frame(const vcl_vector<vsol_digital_curve_2d_sptr>& v, int t, double neighbor_radius)
: _curves(v), _time(t), _neighbor_radius(neighbor_radius)  {
}

//: misc helper function
double dmod(double a,double b) {
  return ((a / b) - ((int (a/b)))) * b;
}

//: Discretize the points
void bcdg_frame::discretize() {
  // neighbors radius
  double r = _neighbor_radius;
  double r2 = 2 * r;

  // Find Maxes
  double maxX = 0.0, maxY = 0.0;
  for (bcdg_frame::point_iterator it = this->begin_linear();
       !it.is_done(); ++it) {
    maxX = vcl_max(maxX, (*it).x());
    maxY = vcl_max(maxY, (*it).y());
  }

//    _point_buckets = vcl_vector< vcl_vector< vcl_vector<dbecl_episeg_point> > >[(int) maxX/r2][(int) maxY/r2];
    _point_buckets = vcl_vector< vcl_vector <vcl_vector<dbecl_episeg_point> > >();
    _point_buckets.resize((int) vcl_ceil(maxX / r2)); // FIXME- is this +2 okay?
    for(int i = 0; i < _point_buckets.size(); i++) {
      _point_buckets[i] = vcl_vector< vcl_vector <dbecl_episeg_point> >();
      _point_buckets[i].resize((int) vcl_ceil(maxY / r2));
      for(int j = 0; j < _point_buckets[i].size(); j++) {
        _point_buckets[i][j] = vcl_vector< dbecl_episeg_point >();
      }
    }

    int CNT = 0;
    for (point_iterator it = this->begin_linear(); !it.is_done(); ++it) {
//      vgl_point_2d<double> p = it.curve()->point(it.index());
      vgl_point_2d<double> p = *it;
//      _point_buckets[(int) p->x_()/r2][(int) p->y_()/r2].push_back(*it);
      int first = (int) vcl_max(0.,(p.x() / r2) - 1);
      int second = (int) vcl_max(0.,(p.y() / r2) - 1);
//      assert(first < _point_buckets.size());
//      assert(second < _point_buckets[first].size());
      if(first >= _point_buckets.size()) {
        vcl_cerr << "oops: 1st is " << first << " and size is "
          << _point_buckets.size() << "\n";
        vcl_cerr << "\tmaxX = " << maxX << " and maxY = " << maxY << "\n";
      } else if(second >= _point_buckets[first].size()) {
        vcl_cerr << "oops2: 1st = " << first << ", 2nd = " << second 
          << " and size of 2nd is " << _point_buckets[first].size() << "\n";
        vcl_cerr << "\tmaxX = " << maxX << " and maxY = " << maxY << "\n";
      } else {
        CNT++;
        _point_buckets[first][second].push_back(dbecl_episeg_point(it.curve(),it.index()));
      }
    }
    vcl_cerr << "Count was " << CNT << "\n";
//  }
    
}

//: Convert digicurves to episegs
void bcdg_frame::convert(bcdg_algo_params p) {
  _epi_segs.clear();
  dbecl_episeg_from_curve_converter factory(p->epipole());
  for(int i = 0; i < _curves.size(); i++) {
    vsol_digital_curve_2d_sptr curve = _curves[i];
    vcl_vector<dbecl_episeg_sptr> episegs = factory.convert_curve(curve);
    _epi_segs.insert(_epi_segs.end(), episegs.begin(), episegs.end());
  }
  _curves.clear();
  discretize();
}

//: Returns the time associated with this frame.
int bcdg_frame::time() const {
  return _time;
}

//: Returns the neighbors of the specified point
vcl_vector<dbecl_episeg_point> bcdg_frame::neighbors(const vgl_point_2d<double> p) const {
  // neighbor radius
  double r = _neighbor_radius;
  double r2 = 2*r;
  int x = 1, y = 1;
//  if (p.x()%r2 < r) {
  if(dmod(p.x(), r2) < r) {
    x = -1;
  }
//  if (p.y()%r2 < r) {
  if(dmod(p.y(), r2) < r) {
    y = -1;
  }
  vcl_vector<dbecl_episeg_point> ret;
  for (int i = 0; i < 2 && i > -2; i+=x) {
    for (int j = 0; j < 2 && j > -2; j+=y) {
      int ix = (int) (p.x()/r2) + i;
      int jy = (int) (p.y()/r2) + j;
      vcl_vector<dbecl_episeg_point> bucket = _point_buckets[ix][jy];
      for (int k = 0; k < bucket.size(); ++k) {
        if (vgl_distance(p,bucket[k].episeg()->point(bucket[k].index())) <= r) {
          ret.push_back(bucket[k]);
        }
      }
    }
  }
  return ret;
}


bcdg_frame::point_iterator bcdg_frame::begin_linear() const {
  if(_epi_segs.size() == 0) {
    vcl_cerr << "bcdg: WARNING: creating a point iterator BEFORE episeg conversion is pointless.\n";
  }
  return bcdg_frame::point_iterator(*this, true);
}

bcdg_frame::random_point_iterator bcdg_frame::begin_random(int n) const {
  return bcdg_frame::random_point_iterator(n,*this, true);
}


// POINT ITERATOR FUNCTIONS
bcdg_frame::point_iterator::point_iterator(const bcdg_frame& frame, bool begin) :
  _point_idx(0),
  _curve_idx(0),
  _frame(const_cast<bcdg_frame*>(&frame)) // FIXME-dave
{
  if(!begin) {
    _curve_idx = _frame->_epi_segs.size() - 1;
    _point_idx = _frame->_epi_segs[_curve_idx]->curve()->size() - 1;
  }
}

bool bcdg_frame::point_iterator::is_done() const {
  return (_curve_idx >= _frame->_epi_segs.size());
//  (_point_idx >= _frame->_epi_segs[_curve_idx]->curve()->size());
}

dbecl_episeg_sptr bcdg_frame::point_iterator::curve() const {
  return _frame->_epi_segs[_curve_idx];
}

int bcdg_frame::point_iterator::index() const {
  return _point_idx;
}

bcdg_frame::point_iterator& bcdg_frame::point_iterator::operator++() {
  if(++_point_idx >= _frame->_epi_segs[_curve_idx]->curve()->size()) {
    _curve_idx++;
    _point_idx = 0;
  }
  return *this;
}

bool bcdg_frame::point_iterator::operator==(const point_iterator& rhs) const {
  return _frame == rhs._frame && 
         _point_idx == rhs._point_idx &&
         _curve_idx == rhs._curve_idx;
}
bool bcdg_frame::point_iterator::operator!=(const point_iterator& rhs) const {
  return !(*this == rhs);
}
vgl_point_2d<double> bcdg_frame::point_iterator::operator*() const {
  return _frame->_epi_segs[_curve_idx]->curve()->interp((double)_point_idx);
}


bcdg_frame::random_point_iterator::random_point_iterator(int numSamples, 
    const bcdg_frame& frame, bool begin) :
  bcdg_frame::point_iterator(frame,begin),
  _samples(numSamples)
{}

bcdg_frame::point_iterator& bcdg_frame::random_point_iterator::operator++() {
  _curve_idx = rand() % _frame->_epi_segs.size();
  _point_idx = rand() % _frame->_epi_segs[_curve_idx]->curve()->size();
  --_samples;
  return *this;
}

bool bcdg_frame::random_point_iterator::is_done() const {
  return _samples <= 0 || point_iterator::is_done();
}
