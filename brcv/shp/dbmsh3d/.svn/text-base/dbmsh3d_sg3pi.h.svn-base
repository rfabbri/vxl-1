#ifndef dbmsh3d_sg3pi_h_
#define dbmsh3d_sg3pi_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/dbmsh3d_sg3pi.h
//:
// \file
// \brief Shape Grabber raw scan file I/O and processing.
//  See /vision/docs/kimia/shock3d/3DScanner/3PIFormat.PDF.
//
//
// \author
//  MingChing Chang  Feb 04, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <vgl/vgl_point_3d.h>

//: Shape grabber raw scan point
//  See /vision/docs/kimia/shock3d/3DScanner/3PIFormat.PDF.
class dbmsh3d_sg3pi_pt
{
protected:
  //: Point number in each scan line.
  int     pn_;
  //: Intensity: low for dark surfaces and high for bright surfaces.
  //  Typical value between 1200 to 14000.
  int     intensity_;
  int     id_;
  vgl_point_3d<double>  pt_;
  float depth_;

public:
  //: ====== Constructor/Destructor ======
  dbmsh3d_sg3pi_pt (const double& x, const double& y, const double& z, 
                    const int& intensity, const int& pn) {
    pt_.set (x, y, z);
    intensity_ = intensity;
    pn_ = pn;
    id_ = -1;
  }

  //: ====== Data access functions ======
  const int id () const {
    return id_;
  }
  void set_id (const int id) {
    id_ = id;
  }
  const int intensity () {
    return intensity_;
  }
  const int pn () const {
    return pn_;
  }
  vgl_point_3d<double>& pt () {
    return pt_;
  }
  const vgl_point_3d<double>& pt() const {
    return  pt_;
  }
  void set_pt (const vgl_point_3d<double>& pt) {
    pt_ = pt;
  }
  const float depth () const {
    return depth_;
  }
  void set_depth (const float d) {
    depth_ = d;
  }
};

//: Shape grabber raw scan data 
class dbmsh3d_sg3pi
{
protected:
  vcl_vector <vcl_vector<dbmsh3d_sg3pi_pt*> > data_;

  //: average intra-scanline sample distance.
  float intra_sl_dist_;

  //: average inter-scanline sample distance.
  float inter_sl_dist_;

  //: the range coordinate system. 
  //  Vx is along each scanline, Vy is perpendicular to each scanline,
  //  Vz reflects the range z(x,y).
  vgl_vector_3d<double> Vx_, Vy_, Vz_;

public:
  //: ====== Constructor/Destructor ======
  dbmsh3d_sg3pi () {
    intra_sl_dist_ = -1.0f;
    inter_sl_dist_ = -1.0f;
  }
  ~dbmsh3d_sg3pi () {
    for (unsigned int i=0; i<data_.size(); i++) {
      for (unsigned int j=0; j<data_[i].size(); j++)
        delete data_[i][j];
      data_[i].clear();
    }
    data_.clear();
  }
  
  //: ====== Data access functions ======
  const vcl_vector <vcl_vector<dbmsh3d_sg3pi_pt*> >& data () const {
    return data_;
  }
  vcl_vector <vcl_vector<dbmsh3d_sg3pi_pt*> >& get_data () {
    return data_;
  }
  const vcl_vector<dbmsh3d_sg3pi_pt*>& data (const int i) const {
    return data_[i];
  }
  const dbmsh3d_sg3pi_pt* data (const int i, const int j) const {
    return data_[i][j];
  }
  dbmsh3d_sg3pi_pt* get_data (const int i, const int j) {
    return data_[i][j];
  }
  const float intra_sl_dist () const {
    return intra_sl_dist_;
  }
  const float inter_sl_dist () const {
    return inter_sl_dist_;
  }
  const vgl_vector_3d<double>& Vx () const {
    return Vx_;
  }
  const vgl_vector_3d<double>& Vy () const {
    return Vy_;
  }
  const vgl_vector_3d<double>& Vz () const {
    return Vz_;
  }

  //: ====== Query Functions ======
  int get_num_points () const;
  int get_max_pn () const;

  //: Estimate intra- and inter-scanline sample distance.
  void get_sl_sample_dist ();

  //: Estimate range coordinate.
  void estimate_range_coord ();

  void compute_range ();

  //: ====== Data Modification Functions ======
  void add_scanline (vcl_vector<dbmsh3d_sg3pi_pt*>& scanline) {
    data_.push_back (scanline);
  }

  void add_scanpt (dbmsh3d_sg3pi_pt* scanpt) {
    data_[data_.size()-1].push_back (scanpt);
  }
};

double _min_sqd_between_scanlines (vcl_vector<dbmsh3d_sg3pi_pt*>& scanline0, 
                                   vcl_vector<dbmsh3d_sg3pi_pt*>& scanline1);

vgl_vector_3d<double> _compute_scanline_normal (const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline);

bool _get_closest_pts (const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline, 
                       const dbmsh3d_sg3pi_pt* SP, const double& intra_scanline_th,
                       int& m, int& n);

void _add_to_kernel_1 (const dbmsh3d_sg3pi_pt* SP, const double& dist_th,
                       const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline,                     
                       vcl_vector<float>& kernel);

void _add_to_kernel_2 (const dbmsh3d_sg3pi_pt* SP, const double& dist_th,
                       const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline,                     
                       vcl_vector<float>& kernel);

#endif

