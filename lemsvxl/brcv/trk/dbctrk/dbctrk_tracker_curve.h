// \file
// \brief basic data structures to represent curves and store information about their matches
// \author vj (vj@lems.brown.edu)
// \date   8/20/2003
// \verbatim
// Modifications
// \endverbatim

#ifndef dbctrk_tracker_curve_h_
#define dbctrk_tracker_curve_h_

#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <dbctrk/dbctrk_curve_description.h>
#include <vcl_map.h>
#include <vbl/vbl_ref_count.h>
class dbctrk_tracker_curve;

#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vnl/vnl_double_2x2.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

//: Binary save dbctrk_tracker_curve_sptr to stream.
void vsl_b_write(vsl_b_ostream & os, const match_data * p);
//: Binary load dbctrk_tracker_curve_sptr to stream
void vsl_b_read(vsl_b_istream &is,match_data * &p);
//: Binary save dbctrk_tracker_curve_sptr to stream.
void vsl_b_write(vsl_b_ostream & os, const dbctrk_tracker_curve* p);
//: Binary load dbctrk_tracker_curve_sptr to stream
void vsl_b_read(vsl_b_istream &is, dbctrk_tracker_curve* &p);

class match_data : public vbl_ref_count
{
 public:
  match_data(dbctrk_tracker_curve_sptr c,match_data_sptr m);
  match_data()
  {
    curve_set=0;
    cost_=0;
    euc_=0;
    energy_=0;
  }
  ~match_data(){}

  //holds the set of matched curves(can be one or more than one)
  vcl_vector<dbctrk_tracker_curve_sptr> match_curve_set;

  //the representative
  dbctrk_tracker_curve_sptr curve_set;

  //transformation of the matched_curve
  vnl_double_2x2 R_;
  vnl_matrix<double> T_;
  double scale_;
  vnl_matrix<double> Tbar;
  //cost and energy of the match
  double energy_;
  double euc_;
  double cost_;
  vcl_map<int,int> mapping_;
  vcl_vector<int> tail1_;
  vcl_vector<int> tail2_;
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;
  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(vcl_string const&) const;
};



class dbctrk_tracker_curve  : public vbl_ref_count
{
 public :

   dbctrk_tracker_curve();
  ~dbctrk_tracker_curve(){}
  //initialize the curve
  void init_set( vtol_edge_2d_sptr const &c,int id);
  // initialize the curve using a set of points
  void init_set(vcl_vector<vgl_point_2d<double> > p,int id);
  //intializing tails as curves
  void init_set(dbctrk_tracker_curve_sptr c,vcl_vector<int> ks,int id);

  void set_curve(vtol_edge_2d_sptr c) { c_=c; }

  int get_id(){return id_;}
  void set_id(int id){id_=id;}

  void set_best_match_next(match_data_sptr bmn) { best_match_next_=bmn; }
  void set_best_match_prev(match_data_sptr bmp) { best_match_prev_=bmp; }

  match_data_sptr get_best_match_prev() {return best_match_prev_;}
  match_data_sptr get_best_match_next() {return best_match_next_;}
 
  //: Access the curve
  vtol_edge_2d_sptr get_curve() { return c_; }
  double compute_euclidean_distance (vnl_matrix<double> R, vnl_matrix<double> T,double s);
  double compute_euclidean_distance_next (vnl_matrix<double> R, vnl_matrix<double> T,double s);
  void compute_transformation(vcl_vector<vgl_point_2d<double> > curve,
                              vcl_vector<vgl_point_2d<double> > & transformed_curve,
                              vnl_matrix<double> R,vnl_matrix<double> T);
  double compute_mean(vcl_vector<double> t);
  
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(vcl_string const&) const;

  // contains some basic level information about curve

  dbctrk_curve_description * desc;

  vcl_vector<match_data_sptr> next_;
  vcl_vector<match_data_sptr> prev_;
  vcl_vector<match_data_sptr> seg_prev_;
  vcl_map<double,int> neighbors_;

  int match_id_;
  int group_id_;
  int track_id_;
  int frame_number;
  
  bool ismatchedprev_;
  bool ismatchednext_;
  int isnextclosure_;
  int isprevclosure_;

  int grouped_;
  bool isreliable_;
  bool isreal_;
  bool ismovingobject_;
  vdgl_edgel_chain_sptr ec_;
  vcl_vector<double> spatialsig;
  vsol_digital_curve_2d_sptr dc_;

  
 protected :
  vtol_edge_2d_sptr c_;
  int id_;
  match_data_sptr best_match_next_;
  match_data_sptr best_match_prev_;
};




#endif
