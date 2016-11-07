//: Sep 12, 2005 MingChing Chang
//  Goedesic DT Wavefront Propagation Algorithm

#ifndef gdt_path_h_
#define gdt_path_h_

#include <vcl_utility.h>
#include <vcl_vector.h>

#include <gdt/gdt_interval.h>
#include <gdt/gdt_mesh.h>
#include <gdt/gdt_edge.h>

class gdt_path
{
protected:
  //: the <interval, tau> pair as a sample point of back-tracking of the geodesic path.
  vcl_vector<vcl_pair<gdt_interval*, double> > I_tau_pairs_;

public:
  //: ====== Constructor/Destructor ======
  gdt_path () {
  }
  ~gdt_path () {
    I_tau_pairs_.clear ();
  }

  //: ====== Data access functions ======
  vcl_vector<vcl_pair<gdt_interval*, double> >* I_tau_pairs() {
    return &I_tau_pairs_;
  }

  //: ====== Modification functions ======
  void _add_point (gdt_interval* I, double tau) {
    //fix the boundary values
    if (_eqT(tau, 0))
      tau = 0;
    else if (_eqT(tau, I->edge()->len()))
      tau = I->edge()->len();

    vcl_pair<gdt_interval*, double> I_tau (I, tau);

    //assert the I_tau is unique
    if (I_tau_pairs_.size() !=0) {
      //vcl_pair<gdt_interval*, double> prev = I_tau_pairs_[I_tau_pairs_.size()-1];
      //assert (I != prev.first || !_eqT(tau, prev.second) );
      vgl_point_3d<double> prev_pt = get_point (I_tau_pairs_.size()-1);
      vgl_point_3d<double> cur_pt = I->_point_from_tau (tau);
      assert (prev_pt != cur_pt);
    }

    I_tau_pairs_.push_back (I_tau);
  }

  //: ====== Query functions ======

  //: return path length via the first interval
  double get_length () {
    assert (I_tau_pairs_.size() != 0);
    vcl_vector<vcl_pair<gdt_interval*, double> >::iterator it = I_tau_pairs_.begin();
    gdt_interval* I = (*it).first;
    double tau = (*it).second;
    return I->get_dist_at_tau (tau);
  }

  //: the closest source is the vertex at the last interval.
  dbmsh3d_gdt_vertex_3d* get_closest_source () {
    assert (I_tau_pairs_.size() != 0);
    vcl_vector<vcl_pair<gdt_interval*, double> >::iterator it = I_tau_pairs_.end();
    it--;
    gdt_interval* I = (*it).first;
    double tau = (*it).second;

    dbmsh3d_gdt_vertex_3d* source = NULL;
    if (_eqT (0, tau)) {
      assert (I->stau() == 0);
      source = (dbmsh3d_gdt_vertex_3d*) I->edge()->sV();
    }
    else {
      assert (I->etau() == I->edge()->len());
      source = (dbmsh3d_gdt_vertex_3d*) I->edge()->eV();
    }
    return source;
  }

  //: Given a query_dist, return the <i, s> representation
  //  i: the i-th interval in the back-tracking
  //  s: the local arc-length from i-th interval to (i+1) th interval.
  int iter_from_query_dist (double query_dist, double& local_s);

  vgl_point_3d<double> get_query_point (int iteration, double local_s);

  vgl_point_3d<double> get_point (unsigned int iter);

  dbmsh3d_face* get_face_from_I_iter (int i);

  void print_path ();
};

#endif



