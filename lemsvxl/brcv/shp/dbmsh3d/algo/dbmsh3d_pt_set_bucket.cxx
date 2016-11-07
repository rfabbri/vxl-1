// This is dbmsh3d/algo/dbmsh3d_pt_set_bucket.cxx
//:
// \file

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_vector_fixed.h>

#include <dbnl/dbnl_cbrt.h>
#include <dbgl/dbgl_dist.h>
#include <dbnl/dbnl_min_max_3.h>
#include <dbmsh3d/dbmsh3d_edge.h>
#include <dbmsh3d/algo/dbmsh3d_pt_set_bucket.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_bnd.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>

// ###########################################################################
//     3D Space-Partition Bucketing Class Member functions 
// ###########################################################################

void dbmsh3d_pt_bucket::get_pts_outside_reduced_box (const vgl_box_3d<double>& reduce_box, 
          vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts)
{
  //Check all points of B and add the ones inside the extbox.
  for (unsigned int i=0; i<idpt_list_.size(); i++) {
    vgl_point_3d<double> P = idpt_list_[i].second;
    if (reduce_box.contains (P) == false)
      idpts.push_back (idpt_list_[i]);
  }
}

//Generate bucket list (for generating run files and list files).
void dbmsh3d_pt_bktstr::gen_bucket_list (const vcl_string prefix, 
                                         vcl_vector<vcl_string>& bucket_list) 
{
  //Loop through the slices, rows, and buckets.
  for (unsigned int s=0; s<slice_list_.size(); s++) {
    dbmsh3d_pt_slice* S = slice_list_[s];
    for (unsigned int r=0; r<S->row_list().size(); r++) {
      dbmsh3d_pt_row* R = S->row_list(r);
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(b);

        //Save each bucket B to file prefix_zz_yy_xx.p3d
        char buf[128];
        vcl_string fileprefix = prefix;
        fileprefix += "_";
        sprintf (buf, "%02d", s);
        fileprefix += buf;
        fileprefix += "_";
        sprintf (buf, "%02d", r);
        fileprefix += buf;
        fileprefix += "_";
        sprintf (buf, "%02d", b);
        fileprefix += buf;

        //Put this fileprefix into the bucket list.
        bucket_list.push_back (fileprefix);
      }
    }
  }
}

void dbmsh3d_pt_bktstr::save_bucket_p3d (const vcl_string prefix)
{
  //Loop through the slices, rows, and buckets.
  for (unsigned int s=0; s<slice_list_.size(); s++) {
    dbmsh3d_pt_slice* S = slice_list_[s];
    for (unsigned int r=0; r<S->row_list().size(); r++) {
      dbmsh3d_pt_row* R = S->row_list(r);
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(b);

        //Save each bucket B to file prefix_zz_yy_xx.p3d
        char buf[128];
        vcl_string filename = prefix;
        filename += "_";
        sprintf (buf, "%02d", s);
        filename += buf;
        filename += "_";
        sprintf (buf, "%02d", r);
        filename += buf;
        filename += "_";
        sprintf (buf, "%02d", b);
        filename += buf;

        filename += ".p3d";
        dbmsh3d_save_p3d (B->idpt_list(), filename.c_str());
      }
    }
  }
}

void dbmsh3d_pt_bktstr::save_extend_bkt_p3d (const vcl_string prefix, const float extr)
{
  vgl_box_3d<double> box;

  //Loop through the slices, rows, and buckets.
  for (unsigned int s=0; s<slice_list_.size(); s++) {
    dbmsh3d_pt_slice* S = slice_list_[s];
    box.set_min_z (S->min_z());
    box.set_max_z (S->max_z());
    for (unsigned int r=0; r<S->row_list().size(); r++) {
      dbmsh3d_pt_row* R = S->row_list(r);
      box.set_min_y (R->min_y());
      box.set_max_y (R->max_y());
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(b);
        box.set_min_x (B->min_x());
        box.set_max_x (B->max_x());

        //Compute the extended bounding box of B.
        double ext = extr * dbnl_max3 (box.width(), box.height(), box.depth());
        vgl_box_3d<double> extbox = dbgl_extend_box (box, ext);

        vcl_vector<vcl_pair<int, vgl_point_3d<double> > > idpts;
        idpts.clear();
        get_ext_bucket_idpts (int(s), int(r), int(b), extbox, idpts);

        //Save each extended bucket to file prefix_zz_yy_xx.p3d
        char buf[128];
        vcl_string filename = prefix;
        filename += "_";
        sprintf (buf, "%02d", s);
        filename += buf;
        filename += "_";
        sprintf (buf, "%02d", r);
        filename += buf;
        filename += "_";
        sprintf (buf, "%02d", b);
        filename += buf;

        filename += ".p3d";
        dbmsh3d_save_p3d (idpts, filename.c_str());
      }
    }
  }
}

void dbmsh3d_pt_bktstr::get_ext_bucket_idpts (const int s, const int r, const int b, 
                                              const vgl_box_3d<double>& extbox, 
                                              vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts)
{
  //Go through all buckets in the s-1, s, s+1 slices.
  for (int ss=s-1; ss<=s+1; ss++) {
    if (ss<0 || ss>=int(slice_list_.size()))
      continue;
    dbmsh3d_pt_slice* S = slice_list_[ss];

    for (int rr=0; rr<int(S->row_list().size()); rr++) {
      dbmsh3d_pt_row* R = S->row_list(rr);
      //Skip the rows outside the extbox.
      if (R->max_y() < extbox.min_y() || R->min_y() > extbox.max_y())
        continue;

      for (int bb=0; bb<int(R->bucket_list().size()); bb++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(bb);
        //Skip the bucket outside the extbox.
        if (B->max_x() < extbox.min_x() || B->min_x() > extbox.max_x())
        continue;

        //For the bucket (s, r, b) itself.
        if (ss==s && rr==r && bb==b) {          
          //Add all points of bucket (s, r, b) into idpts.
          for (unsigned int i=0; i<B->idpt_list().size(); i++)
            idpts.push_back (B->idpt_list(i));
        }
        else {
          //Check all points of B and add the ones inside the extbox.
          for (unsigned int i=0; i<B->idpt_list().size(); i++) {
            vgl_point_3d<double> P = B->idpt_list(i).second;
            if (extbox.contains (P))
              idpts.push_back (B->idpt_list(i));
          }
        }
      }
    }
  }
}

void dbmsh3d_pt_bktstr::get_sausage_idpts (const int s, const int r, const int b, 
                                           const vgl_box_3d<double>& sboxin, 
                                           const vgl_box_3d<double>& sboxout, 
                                           vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts)
{
  //Check all surrounding buckets and put all points inside 
  //the stitching sausage into S_idpts.

  //Go through all buckets in the s-1, s, s+1 slices.
  for (int ss=s-1; ss<=s+1; ss++) {
    if (ss<0 || ss>=int(slice_list_.size()))
      continue;
    dbmsh3d_pt_slice* S = slice_list_[ss];

    for (int rr=0; rr<int(S->row_list().size()); rr++) {
      dbmsh3d_pt_row* R = S->row_list(rr);
      //Skip the rows outside the extbox.
      if (R->max_y() < sboxout.min_y() || R->min_y() > sboxout.max_y())
        continue;

      for (int bb=0; bb<int(R->bucket_list().size()); bb++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(bb);
        //Skip the bucket outside the extbox.
        if (B->max_x() < sboxout.min_x() || B->min_x() > sboxout.max_x())
          continue;

        //Now we have to examine all point of the bucket B (s, r, b).
        //Skip any point outside sboxout or inside sboxin.
        for (unsigned int i=0; i<B->idpt_list().size(); i++) {
          vgl_point_3d<double> P = B->idpt_list(i).second;
          int id = B->idpt_list(i).first; //for debug.
          if (sboxout.contains (P) == false)
            continue;
          if (sboxin.contains (P))
            continue;
          //Now P is inside the sausage.
          idpts.push_back (B->idpt_list(i));
        }
      }
    }
  }
}

// ###########################################################################
//     Functions for 3D Space-Division Bucketing
// ###########################################################################

void run_adpt_bucketing (vcl_vector<vgl_point_3d<double> >& pts,
                         const int npbkt, const vcl_string prefix,
                         const bool b_check_dup)
{
  //The bucketing structure
  dbmsh3d_pt_bktstr* BktStruct = adpt_bucketing_pts (pts, npbkt, b_check_dup);

  //Generate bucket list (for generating run files and list files).
  vcl_vector<vcl_string> bucket_list;
  BktStruct->gen_bucket_list (prefix, bucket_list);
  
  //Save points in each bucket to a P3D file.
  BktStruct->save_bucket_p3d (prefix);

  //Generate the bucket list file of .P3D files (one for each bucket).
  gen_bktlst_txt (prefix, bucket_list);
  //Generate the bucket info file.
  gen_bktinfo_txt (prefix, bucket_list, BktStruct);
  //Generate the run file to view bucketing results.
  gen_bktlst_view_bat (prefix);
  
  delete BktStruct;
}

void run_cell_bucketing (dbmsh3d_pt_set* pts, const vcl_string prefix,
                         const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ,
                         const float msr)
{
  //The bucketing structure
  cell_bucketing (pts, prefix, BUCKET_NX, BUCKET_NY, BUCKET_NZ);
  
  //Generate the bucket list file of .P3D files (one for each bucket).
  vcl_vector<vcl_string> bucket_list;
  gen_bktlst_txt (prefix, bucket_list);

  //Generate the run file to view bucketing results.
  gen_bktlst_view_bat (prefix);
}



// ###########################################################################
//     The bucketing functions
// ###########################################################################

#define BUCKET_FUZINESS_SEARCH    5
#define BUCKET_FUZINESS           DBGL_MID_EPSILON //1E-10

#define CHECK_DUP_BKTPT           18

bool vgl_pt_z_less (const vgl_point_3d<double>& P1, const vgl_point_3d<double>& P2)
{
  return P1.z() < P2.z();
}

bool points_z_less (const vcl_pair<int, vgl_point_3d<double> > & P1, 
                    const vcl_pair<int, vgl_point_3d<double> > & P2)
{
  return P1.second.z() < P2.second.z();
}

bool points_y_less (const vcl_pair<int, vgl_point_3d<double> > & P1, 
                    const vcl_pair<int, vgl_point_3d<double> > & P2)
{
  return P1.second.y() < P2.second.y();
}

bool points_x_less (const vcl_pair<int, vgl_point_3d<double> > & P1, 
                    const vcl_pair<int, vgl_point_3d<double> > & P2)
{
  return P1.second.x() < P2.second.x();
}


//: Performing the adpative bucketing
//  return the bucketing structure.
//    input M: # points per bucket.
dbmsh3d_pt_bktstr* adpt_bucketing_idpts (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& all_pts,
                                         const int M, const bool b_check_dup)
{
  unsigned int x, idx, s, r, b;
  double border;

  vul_printf (vcl_cout, "adpt_bucketing(): %u points.\n", all_pts.size());
  const unsigned int N = all_pts.size();
  const unsigned int U = (int) vcl_ceil (dbnl_cbrt ((double)N/M));
  vul_printf (vcl_cout, "  %u points per bucket, %u buckets in one dimension.\n", M, U);
  const unsigned int npt_slice = U*U*M;
  const unsigned int npt_row = U*M;
  unsigned int total_num_pts = 0;

  dbmsh3d_pt_bktstr* BktStruct = new dbmsh3d_pt_bktstr ();  

  //1) put points into slices.
  vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Slices;
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > S;
  
  //sort all points by z-coord
  vcl_sort (all_pts.begin(), all_pts.end(), points_z_less);  

  //determine at most U+1 slice border values.
  vcl_vector<double> Sborder;
  Sborder.push_back (all_pts[0].second.z());
  for (s=1; s<U; s++) {
    idx = npt_slice*s;
    if (idx >= all_pts.size())
      continue;
    border = all_pts[idx].second.z();
    //handle the rare case of putting very closeby points into a single bucket.
    for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
      if (idx+x < all_pts.size() && all_pts[idx+x].second.z()-border < BUCKET_FUZINESS)
        border = all_pts[idx+x].second.z();
    }
    if (border > Sborder[Sborder.size()-1])
      Sborder.push_back (border);
  }
  //the last border value.
  Sborder.push_back (all_pts[all_pts.size()-1].second.z() + BUCKET_FUZINESS);

  //put all points into at most U slices in order.
  for (s=1, x=0; x<all_pts.size(); x++) {
    if (all_pts[x].second.z() >= Sborder[s]) {
      //add this slice S to Slices and make S a new empty slice.
      s++;
      Slices.push_back (S);
      S.clear();
    }

    //add current point to slice S.
    S.push_back (vcl_pair<int, vgl_point_3d<double> > (all_pts[x].first, all_pts[x].second));
  }
  //add the final slice S
  Slices.push_back (S);
  all_pts.clear();

  //2) For each slice S, put points into rows.
  for (s=0; s<Slices.size(); s++) {
    S = Slices[s];
    
    //Build BktSlice and add to BktStruct.
    dbmsh3d_pt_slice* BktSlice = new dbmsh3d_pt_slice (Sborder[s], Sborder[s+1]);
    BktStruct->insert_slice (BktSlice);

    //sort the points in slice S by y-coord and put them into U rows in order.
    vcl_sort (S.begin(), S.end(), points_y_less);  

    vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Rows;
    Rows.clear();
    vcl_vector<vcl_pair<int, vgl_point_3d<double> > > R;
    R.clear();

    //determine at most U row border values.
    vcl_vector<double> Rborder;
    Rborder.push_back (S[0].second.y());
    for (r=1; r<U; r++) {
      idx = npt_row*r;
      if (idx >= S.size())
        continue;
      border = S[idx].second.y();
      //handle the rare case of putting very closeby points into a single bucket.
      for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
        if (idx+x < S.size() && S[idx+x].second.y()-border < BUCKET_FUZINESS)
          border = S[idx+x].second.y();
      }
      if (border > Rborder[Rborder.size()-1])
        Rborder.push_back (border);
    }
    //the last border value.
    Rborder.push_back (S[S.size()-1].second.y() + BUCKET_FUZINESS);
    
    //put all points of this slice S into at most U rows in order.
    for (r=1, x=0; x<S.size(); x++) {
      if (S[x].second.y() >= Rborder[r]) {
        //add this row R to Rows and make R a new empty row.
        r++;
        Rows.push_back (R);
        R.clear();
      }

      //add current point to row R.
      R.push_back (vcl_pair<int, vgl_point_3d<double> > (S[x].first, S[x].second));
    }
    //add the final row R
    Rows.push_back (R);
    S.clear();

    //3) For each row R of slice S, put points into buckets.
    for (r=0; r<Rows.size(); r++) {
      R = Rows[r];

      //Build BktRow and add to BktSlice.
      dbmsh3d_pt_row* BktRow = new dbmsh3d_pt_row (Rborder[r], Rborder[r+1]);
      BktSlice->insert_row (BktRow);

      //sort the points in row R of slice S by x-coord and put them into U buckets in order.
      vcl_sort (R.begin(), R.end(), points_x_less);  

      //uniformly distribute points into buckets.
      int Ml = (int) vcl_ceil ((double) R.size() / U);

      vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Buckets;
      Buckets.clear();
      vcl_vector<vcl_pair<int, vgl_point_3d<double> > > B;
      B.clear();

      //determine at most U bucket border values.
      vcl_vector<double> Bborder;
      Bborder.push_back (R[0].second.x());
      for (b=1; b<U; b++) {
        idx = Ml*b;
        if (idx >= R.size())
          continue;
        border = R[idx].second.x();
        //handle the rare case of putting very closeby points into a single bucket.
        for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
          if (idx+x < R.size() && R[idx+x].second.x()-border < BUCKET_FUZINESS)
            border = R[idx+x].second.x();
        }
        if (border > Bborder[Bborder.size()-1])
          Bborder.push_back (border);
      }
      //the last border value.
      Bborder.push_back (R[R.size()-1].second.x() + BUCKET_FUZINESS);

      //put all points of this row R into at most U buckets in order.
      for (b=1, x=0; x<R.size(); x++) {
        if (R[x].second.x() >= Bborder[b]) {
          //add this bucket B to Buckets and make B a new empty bucket.
          b++;
          Buckets.push_back (B);
          B.clear();
        }

        //add current point to bucket B.
        B.push_back (vcl_pair<int, vgl_point_3d<double> > (R[x].first, R[x].second));
      }
      //add the final bucket B
      Buckets.push_back (B);
      R.clear();

      //4) Go through each bucket B of row R of slice S.
      for (b=0; b<Buckets.size(); b++) {
        B = Buckets[b];
        total_num_pts += B.size();
        
        //Build BktB and add to BktRow.
        dbmsh3d_pt_bucket* BktB = new dbmsh3d_pt_bucket (Bborder[b], Bborder[b+1]);
        BktRow->insert_bucket (BktB);

        //Remove duplicate points inside bucket B
        if (b_check_dup)
          check_dup_adpt_bucketing (B, CHECK_DUP_BKTPT);

        BktB->insert_idpts (B);
        
        B.clear();
      }
    }
  }

  assert (total_num_pts == N);
  vul_printf (vcl_cout, "\tTotally %u buckets created.\n", BktStruct->n_buckets());
  return BktStruct;
}

//: Performing the adpative bucketing
//  return the bucketing structure.
//    input M: # points per bucket.
dbmsh3d_pt_bktstr* adpt_bucketing_pts (vcl_vector<vgl_point_3d<double> >& input_pts,
                                       const int M, const bool b_check_dup)
{
  unsigned int x, idx, s, r, b;
  double border;

  vul_printf (vcl_cout, "adpt_bucketing(): %u points.\n", input_pts.size());
  const unsigned int N = input_pts.size();
  const unsigned int U = (int) vcl_ceil (dbnl_cbrt ((double)N/M));
  vul_printf (vcl_cout, "  %u points per bucket, %u buckets in one dimension.\n", M, U);
  const unsigned int npt_slice = U*U*M;
  const unsigned int npt_row = U*M;
  unsigned int total_num_pts = 0;

  dbmsh3d_pt_bktstr* BktStruct = new dbmsh3d_pt_bktstr ();  

  //1) put points into slices.
  vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Slices;
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > S;

  //sort all points by z-coord
  vcl_sort (input_pts.begin(), input_pts.end(), vgl_pt_z_less);  

  //determine at most U+1 slice border values.
  vcl_vector<double> Sborder;
  Sborder.push_back (input_pts[0].z());
  for (s=1; s<U; s++) {
    idx = npt_slice*s;
    if (idx >= input_pts.size())
      continue;
    border = input_pts[idx].z();
    //handle the rare case of putting very closeby points into a single bucket.
    for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
      if (idx+x < input_pts.size() && input_pts[idx+x].z()-border < BUCKET_FUZINESS)
        border = input_pts[idx+x].z();
    }
    if (border > Sborder[Sborder.size()-1])
      Sborder.push_back (border);
  }
  //the last border value.
  Sborder.push_back (input_pts[input_pts.size()-1].z() + BUCKET_FUZINESS);

  //put all points into at most U slices in order.
  for (s=1, x=0; x<input_pts.size(); x++) {
    if (input_pts[x].z() >= Sborder[s]) {
      //add this slice S to Slices and make S a new empty slice.
      s++;
      Slices.push_back (S);
      S.clear();
    }

    //add current point to slice S.
    S.push_back (vcl_pair<int, vgl_point_3d<double> > (x, input_pts[x]));
  }
  //add the final slice S
  Slices.push_back (S);
  input_pts.clear();

  //2) For each slice S, put points into rows.
  for (s=0; s<Slices.size(); s++) {
    S = Slices[s];
    
    //Build BktSlice and add to BktStruct.
    dbmsh3d_pt_slice* BktSlice = new dbmsh3d_pt_slice (Sborder[s], Sborder[s+1]);
    BktStruct->insert_slice (BktSlice);

    //sort the points in slice S by y-coord and put them into U rows in order.
    vcl_sort (S.begin(), S.end(), points_y_less);  

    vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Rows;
    Rows.clear();
    vcl_vector<vcl_pair<int, vgl_point_3d<double> > > R;
    R.clear();

    //determine at most U row border values.
    vcl_vector<double> Rborder;
    Rborder.push_back (S[0].second.y());
    for (r=1; r<U; r++) {
      idx = npt_row*r;
      if (idx >= S.size())
        continue;
      border = S[idx].second.y();
      //handle the rare case of putting very closeby points into a single bucket.
      for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
        if (idx+x < S.size() && S[idx+x].second.y()-border < BUCKET_FUZINESS)
          border = S[idx+x].second.y();
      }
      if (border > Rborder[Rborder.size()-1])
        Rborder.push_back (border);
    }
    //the last border value.
    Rborder.push_back (S[S.size()-1].second.y() + BUCKET_FUZINESS);
    
    //put all points of this slice S into at most U rows in order.
    for (r=1, x=0; x<S.size(); x++) {
      if (S[x].second.y() >= Rborder[r]) {
        //add this row R to Rows and make R a new empty row.
        r++;
        Rows.push_back (R);
        R.clear();
      }

      //add current point to row R.
      R.push_back (vcl_pair<int, vgl_point_3d<double> > (S[x].first, S[x].second));
    }
    //add the final row R
    Rows.push_back (R);
    S.clear();

    //3) For each row R of slice S, put points into buckets.
    for (r=0; r<Rows.size(); r++) {
      R = Rows[r];

      //Build BktRow and add to BktSlice.
      dbmsh3d_pt_row* BktRow = new dbmsh3d_pt_row (Rborder[r], Rborder[r+1]);
      BktSlice->insert_row (BktRow);

      //sort the points in row R of slice S by x-coord and put them into U buckets in order.
      vcl_sort (R.begin(), R.end(), points_x_less);  

      //uniformly distribute points into buckets.
      int Ml = (int) vcl_ceil ((double) R.size() / U);

      vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Buckets;
      Buckets.clear();
      vcl_vector<vcl_pair<int, vgl_point_3d<double> > > B;
      B.clear();

      //determine at most U bucket border values.
      vcl_vector<double> Bborder;
      Bborder.push_back (R[0].second.x());
      for (b=1; b<U; b++) {
        idx = Ml*b;
        if (idx >= R.size())
          continue;
        border = R[idx].second.x();
        //handle the rare case of putting very closeby points into a single bucket.
        for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
          if (idx+x < R.size() && R[idx+x].second.x()-border < BUCKET_FUZINESS)
            border = R[idx+x].second.x();
        }
        if (border > Bborder[Bborder.size()-1])
          Bborder.push_back (border);
      }
      //the last border value.
      Bborder.push_back (R[R.size()-1].second.x() + BUCKET_FUZINESS);

      //put all points of this row R into at most U buckets in order.
      for (b=1, x=0; x<R.size(); x++) {
        if (R[x].second.x() >= Bborder[b]) {
          //add this bucket B to Buckets and make B a new empty bucket.
          b++;
          Buckets.push_back (B);
          B.clear();
        }

        //add current point to bucket B.
        B.push_back (vcl_pair<int, vgl_point_3d<double> > (R[x].first, R[x].second));
      }
      //add the final bucket B
      Buckets.push_back (B);
      R.clear();

      //4) Go through each bucket B of row R of slice S.
      for (b=0; b<Buckets.size(); b++) {
        B = Buckets[b];
        total_num_pts += B.size();
        
        //Build BktB and add to BktRow.
        dbmsh3d_pt_bucket* BktB = new dbmsh3d_pt_bucket (Bborder[b], Bborder[b+1]);
        BktRow->insert_bucket (BktB);

        //Remove duplicate points inside bucket B
        if (b_check_dup)
          check_dup_adpt_bucketing (B, CHECK_DUP_BKTPT);

        BktB->insert_idpts (B);
        
        B.clear();
      }
    }
  }

  assert (total_num_pts == N);
  vul_printf (vcl_cout, "\tTotally %u buckets created.\n", BktStruct->n_buckets());
  return BktStruct;
}

// #####################################################################################

//: M: # points per bucket.
//  return # duplicate point removed.
int check_dup_adpt_bucketing (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts,
                              const int M, const double epsilon)
{
  unsigned int x, idx, s, r, b;
  double border;

  vul_printf (vcl_cout, "  check_dup_adpt_bucketing(): %u points.\n", idpts.size());
  const unsigned int N = idpts.size();
  const unsigned int U = (int) vcl_ceil (dbnl_cbrt ((double)N/M));
  vul_printf (vcl_cout, "    %u points per bucket, %u buckets in one dimension.\n", M, U);
  vul_printf (vcl_cout, "    duplicate points: ");
  const unsigned int npt_slice = U*U*M;
  const unsigned int npt_row = U*M;
  unsigned int total_num_pts = 0;
  unsigned int dup_count = 0;

  //1) put points into U slices.
  vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Slices;
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > S;

  //sort all points by z-coord
  vcl_sort (idpts.begin(), idpts.end(), points_z_less);  

  //determine at most U-1 slice border value.
  vcl_vector<double> Sborder;
  Sborder.push_back (idpts[0].second.z());
  for (s=1; s<U; s++) {
    idx = npt_slice*s;
    if (idx >= idpts.size())
      continue;
    border = idpts[idx].second.z();
    //handle the rare case of putting very closeby points into a single bucket.
    for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
      if (idx+x < idpts.size() && idpts[idx+x].second.z()-border < BUCKET_FUZINESS)
        border = idpts[idx+x].second.z();
    }
    if (border > Sborder[Sborder.size()-1])
      Sborder.push_back (border);
  }
  //the last border value.
  Sborder.push_back (idpts[idpts.size()-1].second.z() + BUCKET_FUZINESS);

  //put all points into at most U slices in order.
  for (s=1, x=0; x<idpts.size(); x++) {
    if (idpts[x].second.z() >= Sborder[s]) {
      //add this slice S to Slices and make S a new empty slice.
      s++;
      Slices.push_back (S);
      S.clear();
    }

    //add current point to slice S.
    S.push_back (vcl_pair<int, vgl_point_3d<double> > (idpts[x].first, idpts[x].second));
  }
  //add the final slice S
  Slices.push_back (S);
  idpts.clear();

  //2) For each slice S, put points into rows.
  for (s=0; s<Slices.size(); s++) {
    S = Slices[s];

    //sort the points in slice S by y-coord and put them into U rows in order.
    vcl_sort (S.begin(), S.end(), points_y_less);  

    vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Rows;
    Rows.clear();
    vcl_vector<vcl_pair<int, vgl_point_3d<double> > > R;
    R.clear();

    //determine at most U row border values.
    vcl_vector<double> Rborder;
    Rborder.push_back (S[0].second.y());
    for (r=1; r<U; r++) {
      idx = npt_row*r;
      if (idx >= S.size())
        continue;
      border = S[idx].second.y();
      //handle the rare case of putting very closeby points into a single bucket.
      for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
        if (idx+x < S.size() && S[idx+x].second.y()-border < BUCKET_FUZINESS)
          border = S[idx+x].second.y();
      }
      if (border > Rborder[Rborder.size()-1])
        Rborder.push_back (border);
    }
    //the last border value.
    Rborder.push_back (S[S.size()-1].second.y() + BUCKET_FUZINESS);
    
    //put all points of this slice S into at most U rows in order.
    for (r=1, x=0; x<S.size(); x++) {
      if (S[x].second.y() >= Rborder[r]) {
        //add this row R to Rows and make R a new empty row.
        r++;
        Rows.push_back (R);
        R.clear();
      }

      //add current point to row R.
      R.push_back (vcl_pair<int, vgl_point_3d<double> > (S[x].first, S[x].second));
    }
    //add the final row R
    Rows.push_back (R);
    S.clear();

    //3) For each row R of slice S, put points into buckets.
    //   Here uniformly distribute points into buckets.
    for (r=0; r<Rows.size(); r++) {
      R = Rows[r];

      //sort the points in row R of slice S by x-coord and put them into U buckets in order.
      vcl_sort (R.begin(), R.end(), points_x_less);  

      //uniformly distribute points into buckets.
      int Ml = (int) vcl_ceil ((double) R.size() / U);

      vcl_vector<vcl_vector<vcl_pair<int, vgl_point_3d<double> > > > Buckets;
      Buckets.clear();
      vcl_vector<vcl_pair<int, vgl_point_3d<double> > > B;
      B.clear();

      //determine at most U bucket border values.
      vcl_vector<double> Bborder;
      Bborder.push_back (R[0].second.x());
      for (b=1; b<U; b++) {
        idx = Ml*b;
        if (idx >= R.size())
          continue;
        border = R[idx].second.x();
        //handle the rare case of putting very closeby points into a single bucket.
        for (x=1; x<BUCKET_FUZINESS_SEARCH; x++) {
          if (idx+x < R.size() && R[idx+x].second.x()-border < BUCKET_FUZINESS)
            border = R[idx+x].second.x();
        }
        if (border > Bborder[Bborder.size()-1])
          Bborder.push_back (border);
      }
      //the last border value.
      Bborder.push_back (R[R.size()-1].second.x() + BUCKET_FUZINESS);

      //put all points of this row R into at most U buckets in order.
      for (b=1, x=0; x<R.size(); x++) {
        if (R[x].second.x() >= Bborder[b]) {
          //add this bucket B to Buckets and make B a new empty bucket.
          b++;
          Buckets.push_back (B);
          B.clear();
        }

        //add current point to bucket B.
        B.push_back (vcl_pair<int, vgl_point_3d<double> > (R[x].first, R[x].second));
      }
      //add the final bucket B
      Buckets.push_back (B);
      R.clear();

      //4) Go through each bucket B of row R of slice S.
      for (b=0; b<Buckets.size(); b++) {
        B = Buckets[b];
        total_num_pts += B.size();

        //Remove duplicate points inside bucket B
        unsigned int dup;
        if (epsilon == 0) 
          dup = check_dup_pts (B);
        else
          dup = check_dup_pts (B, epsilon);

        dup_count += dup;
        idpts.insert (idpts.end(), B.begin(), B.end());

        B.clear();
      }
    }
  }

  assert (total_num_pts == N);
  assert (dup_count + idpts.size() == N);
  vul_printf (vcl_cout, "\n  total duplicate points found: %d\n", dup_count);

  return dup_count;
}

int check_dup_pts (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts)
{
  unsigned int dup = 0;

  vcl_vector<vcl_pair<int, vgl_point_3d<double> > >::iterator it1 = idpts.begin(); 
  for (; it1 != idpts.end(); ++it1) {
    int id1 = (*it1).first;
    vgl_point_3d<double> v1 = (*it1).second;
    
    vcl_vector<vcl_pair<int, vgl_point_3d<double> > >::iterator it2 = it1;
    it2++;
    while (it2 != idpts.end()) {
      vgl_point_3d<double> v2 = (*it2).second;

      if (v1.x() == v2.x() && v1.y() == v2.y() && v1.z() == v2.z()) {
        //found, delete it2.
        vcl_vector<vcl_pair<int, vgl_point_3d<double> > >::iterator tmp = it2;
        tmp--;
        /////vcl_cout<< (*it2).first << " ";
        idpts.erase (it2);
        dup++;
        //can't break the loop if multiple duplication is possible.
        //break; //break y loop. go to the next x.
        tmp++;
        it2 = tmp;
      }
      else
        it2++;
    }    
  }

  return dup;
}

int check_dup_pts (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts,
                   const double epsilon)
{
  unsigned int dup = 0;

  vcl_vector<vcl_pair<int, vgl_point_3d<double> > >::iterator it1 = idpts.begin(); 
  for (; it1 != idpts.end(); ++it1) {
    int id1 = (*it1).first;
    vgl_point_3d<double> v1 = (*it1).second;
    
    vcl_vector<vcl_pair<int, vgl_point_3d<double> > >::iterator it2 = it1;
    it2++;
    while (it2 != idpts.end()) {
      vgl_point_3d<double> v2 = (*it2).second;

      if (dbgl_eq (v1.x(), v2.x(), epsilon) && 
          dbgl_eq (v1.y(), v2.y(), epsilon) && 
          dbgl_eq (v1.z(), v2.z(), epsilon)) {
        //found, delete it2.
        vcl_vector<vcl_pair<int, vgl_point_3d<double> > >::iterator tmp = it2;
        tmp--;
        /////vcl_cout<< (*it2).first << " ";
        idpts.erase (it2);
        dup++;
        //can't break the loop if multiple duplication is possible.
        //break; //break y loop. go to the next x.
        tmp++;
        it2 = tmp;
      }
      else
        it2++;
    }    
  }

  return dup;
}

// #####################################################################################

//: Bucketing to partition point set into buckets.
bool cell_bucketing (dbmsh3d_pt_set* pt_set, const vcl_string prefix,
                     const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ)
{
  char buf[64];
  int x, y, z;
  vul_printf (vcl_cout, "run_cell_bucketing(): %u points.\n", pt_set->vertexmap().size());

  vgl_box_3d<double> bbox;
  detect_bounding_box (pt_set, bbox);
  vul_printf (vcl_cout, "\n Bounding Box: \n");
  vul_printf (vcl_cout, "     (minx, maxx) = (%lf, %lf)\n", bbox.min_x(), bbox.max_x());
  vul_printf (vcl_cout, "     (miny, maxy) = (%lf, %lf)\n", bbox.min_y(), bbox.max_y());
  vul_printf (vcl_cout, "     (minz, maxz) = (%lf, %lf)\n", bbox.min_z(), bbox.max_z());
  double BOX_X = bbox.max_x() - bbox.min_x();
  double BOX_Y = bbox.max_y() - bbox.min_y();
  double BOX_Z = bbox.max_z() - bbox.min_z();
  double BUCKET_LX = BOX_X / BUCKET_NX;
  double BUCKET_LY = BOX_Y / BUCKET_NY;
  double BUCKET_LZ = BOX_Z / BUCKET_NZ;
    
  //Prepare bucket number list for generating run files and list files.
  vcl_vector<vcl_string> bucket_list;

  //Go through each bucket
  unsigned int count = 0;
  unsigned int total_num = 0;
  unsigned int max_points_bucket = 0;
  for (x=0; x<BUCKET_NX; x++) {
    for (y=0; y<BUCKET_NY; y++) {
      for (z=0; z<BUCKET_NZ; z++) {
        //Put pts inside the bucket into a separate pt_set PS.
        dbmsh3d_pt_set* PS = new dbmsh3d_pt_set ();
        vgl_box_3d<double> bucket;
        bucket.set_min_x (bbox.min_x() + BUCKET_LX*x);
        bucket.set_min_y (bbox.min_y() + BUCKET_LY*y);
        bucket.set_min_z (bbox.min_z() + BUCKET_LZ*z);
        bucket.set_max_x (bbox.min_x() + BUCKET_LX*(x+1));
        if (dbgl_eq_c (bucket.max_x(), bbox.max_x())) {
          bucket.set_max_x (bbox.max_x() + 1E-5);
        }
        bucket.set_max_y (bbox.min_y() + BUCKET_LY*(y+1));
        if (dbgl_eq_c (bucket.max_y(), bbox.max_y())) {
          bucket.set_max_y (bbox.max_y() + 1E-5);
        }
        bucket.set_max_z (bbox.min_z() + BUCKET_LZ*(z+1));
        if (dbgl_eq_c (bucket.max_z(), bbox.max_z())) {
          bucket.set_max_z (bbox.max_z() + 1E-5);
        }
        int num = put_pts_into_bucket (pt_set,
                                       bucket.min_x(), bucket.max_x(),
                                       bucket.min_y(), bucket.max_y(),
                                       bucket.min_z(), bucket.max_z(), PS);
        if (num == 0) {
          //no point in this bucket.
        }
        else {
          //Save PS into a .p3d file.
          vcl_string fileprefix = prefix;
          sprintf (buf, "%02d", x);
          fileprefix += "_";
          fileprefix += buf;
          sprintf (buf, "%02d", y);
          fileprefix += "_";
          fileprefix += buf;
          sprintf (buf, "%02d", z);
          fileprefix += "_";
          fileprefix += buf;

          //Put this fileprefix into the bucket list.
          bucket_list.push_back (fileprefix);

          fileprefix += ".p3d";
          dbmsh3d_save_p3d (PS, fileprefix.c_str());
          count++;          
        }

        total_num += num;
        if (max_points_bucket < PS->vertexmap().size())
          max_points_bucket = PS->vertexmap().size();
        delete PS;
      } 
    }
  }
  assert (total_num == pt_set->vertexmap().size());

  vul_printf (vcl_cout, "cell_bucketing(): %d point cloud files (*.P3D) saved.\n", count);
  vul_printf (vcl_cout, "\tmax_points_bucket: %d.\n\n", max_points_bucket);

  return true;
}

//: return # points put into this bucket.
int put_pts_into_bucket (dbmsh3d_pt_set* pt_set,
                         const double minx, const double maxx,
                         const double miny, const double maxy,
                         const double minz, const double maxz,
                         dbmsh3d_pt_set* PS)
{
  assert (PS->vertexmap().size() == 0);

  //Clone all points of pt_set inside bucket and add to PS
  vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
  for (; it != pt_set->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    if (V->pt().x() >= minx && V->pt().x() < maxx &&
        V->pt().y() >= miny && V->pt().y() < maxy &&
        V->pt().z() >= minz && V->pt().z() < maxz) {

      dbmsh3d_vertex* Vn = PS->_new_vertex ();
      Vn->set_pt (V->pt());
      PS->_add_vertex (Vn);
    }
  }

  return PS->vertexmap().size();
}

//#####################################################

//Generate the bucketing list file.
void gen_bktlst_txt (const vcl_string& prefix,
                     const vcl_vector<vcl_string>& bucket_list)
{  
  vcl_string listfile = prefix;
  listfile += "_bktlst.txt";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile.c_str());
    return;
  }
  for (int x=0; x<int(bucket_list.size()); x++) {
    vcl_string fileprefix = bucket_list[x];
    fileprefix += ".p3d";
    vcl_fprintf (fp, "%s\n", fileprefix.c_str());
  }
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate list file for points in buckets: %s.\n", listfile.c_str());
}

void gen_bktinfo_txt (const vcl_string& prefix,
                      const vcl_vector<vcl_string>& bucket_list,
                      dbmsh3d_pt_bktstr* BktStruct)
{
  vgl_box_3d<double> box;
  vcl_string listfile = prefix;
  listfile += "_bktinfo.txt";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile.c_str());
    return;
  }
  unsigned int i=0;    
  for (unsigned int s=0; s<BktStruct->slice_list().size(); s++) {
    dbmsh3d_pt_slice* S = BktStruct->slice_list(s);
    box.set_min_z (S->min_z());
    box.set_max_z (S->max_z());
    for (unsigned int r=0; r<S->row_list().size(); r++) {
      dbmsh3d_pt_row* R = S->row_list(r);
      box.set_min_y (R->min_y());
      box.set_max_y (R->max_y());
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(b);
        box.set_min_x (B->min_x());
        box.set_max_x (B->max_x());

        vcl_string fileprefix = bucket_list[i];
        vcl_fprintf (fp, "%s", fileprefix.c_str());
        vcl_fprintf (fp, " (%lf, %lf, %lf) - (%lf, %lf, %lf)\n", 
                     box.min_x(), box.min_y(), box.min_z(), 
                     box.max_x(), box.max_y(), box.max_z());
        i++;
      }
    }
  }
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate bucket info file: %s.\n", listfile.c_str());
}

//Generate the run file to view bucketing results.
void gen_bktlst_view_bat (const vcl_string& prefix)
{
  vcl_string runfile = prefix + "_bktlst_view.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -list ";
  run_cmd += prefix;
  run_cmd += "_bktlst";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view bucketing results: %s.\n", runfile.c_str());
}
