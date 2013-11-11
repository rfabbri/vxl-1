// This is dbmsh3d/algo/dbmsh3d_pt_set_bucket.h

#ifndef dbmsh3d_pt_set_bucket_h_
#define dbmsh3d_pt_set_bucket_h_

//--------------------------------------------------------------------------------
//:
// \file
// \brief Functions to partition point set into buckets.
// \author Ming-Ching Chang (mcchang@lems.brown.edu)
// \date Jan 20, 2007
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vgl/vgl_point_3d.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

// ###########################################################################
//     The 3D Space-Partition Bucketing Structure
// ###########################################################################

class dbmsh3d_pt_bucket
{
protected:
  double min_x_, max_x_;
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > idpt_list_;

public:
  dbmsh3d_pt_bucket (const double& minx, const double& maxx) {
    min_x_ = minx;
    max_x_ = maxx;
  }
  ~dbmsh3d_pt_bucket () {
    idpt_list_.clear();
  }
  
  double min_x () const {
    return min_x_;
  }
  double max_x () const {
    return max_x_;
  }

  const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpt_list() const {
    return idpt_list_;
  }  
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpt_list() {
    return idpt_list_;
  }  
  vcl_pair<int, vgl_point_3d<double> > idpt_list (const unsigned int i) {
    return idpt_list_[i];
  }

  void insert_idpts (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts) {
    for (unsigned int i=0; i<idpts.size(); i++) {
      idpt_list_.push_back (idpts[i]);
    }
  }

  void get_pts_outside_reduced_box (const vgl_box_3d<double>& reduce_box, 
                                    vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts);
};

class dbmsh3d_pt_row
{
protected:
  double min_y_, max_y_;
  vcl_vector<dbmsh3d_pt_bucket*> bucket_list_;

public:
  dbmsh3d_pt_row (const double& miny, const double& maxy) {
    min_y_ = miny;
    max_y_ = maxy;
  }
  ~dbmsh3d_pt_row () {
    bucket_list_.clear();
  }

  double min_y () const {
    return min_y_;
  }
  double max_y () const {
    return max_y_;
  }

  vcl_vector<dbmsh3d_pt_bucket*>& bucket_list() {
    return bucket_list_;
  }
  const vcl_vector<dbmsh3d_pt_bucket*>& bucket_list() const {
    return bucket_list_;
  }
  dbmsh3d_pt_bucket* bucket_list (const unsigned int i) {
    return bucket_list_[i];
  }
  const dbmsh3d_pt_bucket* bucket_list (const unsigned int i) const {
    return bucket_list_[i];
  }

  void insert_bucket (dbmsh3d_pt_bucket* B) {
    bucket_list_.push_back (B);
  }

  const unsigned int n_buckets() const {
    return bucket_list_.size();
  }
};

class dbmsh3d_pt_slice
{
protected:
  double min_z_, max_z_;
  vcl_vector<dbmsh3d_pt_row*> row_list_;

public:
  dbmsh3d_pt_slice (const double& minz, const double& maxz) {
    min_z_ = minz;
    max_z_ = maxz;
  }
  ~dbmsh3d_pt_slice () {
    row_list_.clear();
  }

  double min_z () const {
    return min_z_;
  }
  double max_z () const {
    return max_z_;
  }

  vcl_vector<dbmsh3d_pt_row*>& row_list() {
    return row_list_;
  }
  const vcl_vector<dbmsh3d_pt_row*>& row_list() const {
    return row_list_;
  }
  dbmsh3d_pt_row* row_list (const unsigned int i) {
    return row_list_[i];
  }
  const dbmsh3d_pt_row* row_list (const unsigned int i) const {
    return row_list_[i];
  }

  void insert_row (dbmsh3d_pt_row* R) {
    row_list_.push_back (R);
  }

  const unsigned int n_buckets() const {
    unsigned int total = 0;
    for (unsigned int r=0; r<row_list_.size(); r++)
      total += row_list_[r]->n_buckets();
    return total;
  }
};

class dbmsh3d_pt_bktstr
{
protected:
  vcl_vector<dbmsh3d_pt_slice*> slice_list_;

public:
  dbmsh3d_pt_bktstr () {
  }
  ~dbmsh3d_pt_bktstr () {
    slice_list_.clear();
  }

  vcl_vector<dbmsh3d_pt_slice*>& slice_list() {
    return slice_list_;
  }
  const vcl_vector<dbmsh3d_pt_slice*>& slice_list() const {
    return slice_list_;
  }
  dbmsh3d_pt_slice* slice_list (const unsigned int i) {
    return slice_list_[i];
  }
  const dbmsh3d_pt_slice* slice_list (const unsigned int i) const {
    return slice_list_[i];
  }

  void insert_slice (dbmsh3d_pt_slice* S) {
    slice_list_.push_back (S);
  }

  const unsigned int n_buckets() const {
    unsigned int total = 0;
    for (unsigned int s=0; s<slice_list_.size(); s++)
      total += slice_list_[s]->n_buckets();
    return total;
  }

  //Generate bucket list (for generating run files and list files).
  void gen_bucket_list (const vcl_string prefix, 
                        vcl_vector<vcl_string>& bucket_list);

  void save_bucket_p3d (const vcl_string prefix);
  void save_extend_bkt_p3d (const vcl_string prefix, const float ext_ratio); 
    void get_ext_bucket_idpts (const int s, const int r, const int b, 
                               const vgl_box_3d<double>& extbox, 
                               vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts);
    void get_sausage_idpts (const int s, const int r, const int b, 
                            const vgl_box_3d<double>& sboxin, 
                            const vgl_box_3d<double>& sboxout, 
                            vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts);
};

// ###########################################################################
//     Functions for 3D Space-Division Bucketing
// ###########################################################################

void run_adpt_bucketing (vcl_vector<vgl_point_3d<double> >& pts,
                         const int npbkt, const vcl_string prefix,
                         const bool b_check_dup);

void run_cell_bucketing (dbmsh3d_pt_set* pt_set, const vcl_string prefix,
                         const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ,
                         const float msr);

dbmsh3d_pt_bktstr* adpt_bucketing_idpts (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& all_pts,
                                         const int npbkt, const bool b_check_dup);

dbmsh3d_pt_bktstr* adpt_bucketing_pts (vcl_vector<vgl_point_3d<double> >& pts,
                                       const int npbkt, const bool b_check_dup);

//epsilon 0: exact math.
int check_dup_adpt_bucketing (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts,
                              const int M, const double epsilon = 0);

int check_dup_pts (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts);

int check_dup_pts (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts,
                   const double epsilon);

bool cell_bucketing (dbmsh3d_pt_set* pt_set, const vcl_string prefix,
                     const int nx, const int ny, const int nz);

int put_pts_into_bucket (dbmsh3d_pt_set* pt_set,
                         const double minx, const double maxx,
                         const double miny, const double maxy,
                         const double minz, const double maxz,
                         dbmsh3d_pt_set* PS);

//#####################################################

//Generate the bucketing list file.
void gen_bktlst_txt (const vcl_string& prefix,
                     const vcl_vector<vcl_string>& bucket_list);

void gen_bktinfo_txt (const vcl_string& prefix, 
                      const vcl_vector<vcl_string>& bucket_list,
                      dbmsh3d_pt_bktstr* BktStruct);

//Generate the run file to view bucketing results.
void gen_bktlst_view_bat (const vcl_string& prefix);

//#####################################################
// Old: The search order for 3D Lookup Table is
//    First Z: xySlice, then Y: xDimension, then X.
//#define POS_3D_TABLE((x),(y),(z),(xyslicesize),(xdimension)) ((z) * (xyslicesize) + (y) * (xdimension) + (x))
inline int POS_3D_TABLE (int px, int py, int pz, int xySliceSize,  int xDimension)
{
   int iPos = pz * xySliceSize + py * xDimension + px;
   return iPos;
}

#endif //dbmsh3d_pt_set_bucket_h_



