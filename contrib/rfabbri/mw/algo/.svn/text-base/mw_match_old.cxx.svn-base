#include <mw/mw_util.h>
#include "mw_match_old.h"
#include <mw/mw_curves.h>
#include <mw/mw_discrete_corresp.h>
#include <mw/mw_intersection_sets.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_distance.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>

//static mw_curves *
//trinocular_match_initialize(const vcl_string &fname);
static mw_curves *
trinocular_match_initialize(const vcl_string &fname, unsigned offset_x, unsigned offset_y);

//: this is basically based on Faugera's 1991 criteria
bool
trinocular_match()
{
   vgl_homg_line_2d<double> el12;

   // Open 3 images and some curves (TODO: make into a function)
   vcl_string fname1("tst1/p1010053.jpg");
   vcl_string fname2("tst1/p1010064.jpg");
   vcl_string fname3("tst1/p1010047.jpg");
   
   mw_curves *cdata1 = trinocular_match_initialize(fname1,96,1170); // TODO: read offsets auto
   vcl_cerr << "Cdata1:\n" << *cdata1 << vcl_endl;
   mw_curves *cdata2 = trinocular_match_initialize(fname2,9,1344);
   vcl_cerr << "Cdata2:\n" << *cdata2 << vcl_endl;
   mw_curves *cdata3 = trinocular_match_initialize(fname3,54,999);
   vcl_cerr << "Cdata3:\n" << *cdata3 << vcl_endl;

   //: TODO: correspondence should start from the view with largest number of
   //points, so that corresp list be well distributed (?)
   mw_discrete_corresp corresp12(cdata1->npts(),cdata2->npts());
   

   //TODO: make function that only takes calibrations and cameras

   vpgl_perspective_camera <double> Pr1,Pr2,Pr3;
 
   if (!read_cam(fname1,fname2,&Pr1,&Pr2)) {
      vcl_cerr << "error reading cam\n";
      return false;
   } if (!read_cam(fname3,&Pr3)) {
      vcl_cerr << "error reading cam\n";
      return false;
   }
 
   vpgl_fundamental_matrix<double> f_12(Pr1,Pr2);
   vpgl_fundamental_matrix<double> f_13(Pr1,Pr3);
   vpgl_fundamental_matrix<double> f_23(Pr2,Pr3);

   // For each point in I1, find putative matches in 2nd image around epipolar
   // line

   //
   // Binocular
   //
   mw_attributed_object pt_attrib;
   pt_attrib.cost_ = 0;
   const double epipolar_distance_threshold = 1;

   for (unsigned long i1=0; i1<cdata1->npts(); ++i1) {
      vgl_homg_point_2d<double> homg_pt(cdata1->pt(i1).get_p());
      vgl_homg_line_2d<double> 
         el_12 = f_12.l_epipolar_line(homg_pt);

      for (unsigned long i2=0; i2<cdata2->npts(); ++i2) {
         double curr_dist;
         vgl_homg_point_2d<double> homg_pt2(cdata2->pt(i2).get_p());

         curr_dist = vgl_distance(el_12, homg_pt2);

         if (curr_dist <= epipolar_distance_threshold) {
            pt_attrib.obj_ = i2;
            pt_attrib.cost_ = curr_dist;
            corresp12.corresp_[i1].push_back(pt_attrib);
         }
      }
   }

   vcl_cout << corresp12 << vcl_endl;

   // post-process:
   //    - e.g. get only best matches and see result

   //
   // Trinocular
   //
   // TODO: analyse only ambiguous

   const double point_transfer_distance_threshold = 5;

   // for each point in 1st image
   for (unsigned long i1=0; i1<cdata1->npts(); ++i1) {
      vcl_list< mw_attributed_object >:: iterator  ptr;
      vcl_list<mw_attributed_object> *lst = &(corresp12.corresp_[i1]);
      vgl_homg_point_2d<double> homg_pt1(cdata1->pt(i1).get_p());
      double distance; unsigned long  cp3;

      // for each correspondence with current point, assess its goodness using 3rd view
      for (ptr=lst->begin(); ptr != lst->end(); ++ptr) { 
         vgl_homg_point_2d<double> homg_pt2(cdata2->pt(ptr->obj_).get_p());

         vgl_homg_point_2d<double> p3
            = mw_epipolar_point_transfer(homg_pt1,homg_pt2,f_13,f_23);

         cdata3->nearest_point_to(p3.x()/p3.w(), p3.y()/p3.w(), &cp3,&distance);

         if (distance > point_transfer_distance_threshold) {
            // remove match (or add to cost field in other approaches)
            ptr = lst->erase(ptr);
         } else {
            // add to cost field
            ptr->cost_ += distance;
         }
      }
   }

   vcl_cout << "\n\n------------------------------------------------=======\n\n";
   vcl_cout << corresp12 << vcl_endl;

  //p3 = mw_epipolar_point_transfer(p1,p2,f13,f23);
   delete cdata1;
   delete cdata2;
   delete cdata3;

   return true;
}

//:
// TODO: get the origin filename automatically
mw_curves *
trinocular_match_initialize(const vcl_string &fname, unsigned offset_x, unsigned offset_y)
{
   vgl_vector_2d<double> offset(offset_x,offset_y);

   vcl_vector<vcl_string> con_fnames;
   if (!con_filenames(fname,con_fnames))
      return false;

   vcl_vector< vcl_vector<vsol_point_2d_sptr> > *pcurvepts;
   pcurvepts = new vcl_vector< vcl_vector<vsol_point_2d_sptr> >;
   vcl_vector< vcl_vector<vsol_point_2d_sptr> > &curvepts=*pcurvepts;

   curvepts.resize(con_fnames.size());
   for (unsigned i=0; i< con_fnames.size(); ++i) {
      bool is_open;

      vcl_cout << "Reading: " <<  con_fnames[i] << vcl_endl;
      if (!load_con_file(con_fnames[i],curvepts[i],&is_open))
         return false;

      for (unsigned j=0; j<curvepts[i].size(); ++j)
         curvepts[i][j]->add_vector(offset);
   }

   return new mw_curves(&curvepts);
}

/*
//: user has to delete mw_curves
static mw_curves *
trinocular_match_initialize(const vcl_string &fname)
{
   vcl_vector<vcl_string> con_fnames;
   if (!con_filenames(fname,con_fnames))
      return false;

   vcl_vector< vcl_vector<vsol_point_2d_sptr> > *pcurvepts;
   pcurvepts = new vcl_vector< vcl_vector<vsol_point_2d_sptr> >;
   vcl_vector< vcl_vector<vsol_point_2d_sptr> > &curvepts=*pcurvepts;

   curvepts.resize(con_fnames.size());
   for (unsigned i=0; i< con_fnames.size(); ++i) {
      bool is_open;

      vcl_cout << "Reading: " <<  con_fnames[i] << vcl_endl;
      if (!load_con_file(con_fnames[i],curvepts[i],&is_open))
         return false;
   }

   return new mw_curves(&curvepts);
}
*/

//---------------------------------------------------------------------------

//: 
// Given a line in an image (e.g. epipolar line), and a curve in that image, 
// and the original point in the other image, this function returns matching
// point that is the intersection of the line with that curve, and which minimizes
// distance to the given point (ie "disparity").
//
//return false if no intersection
// Tries a match that minimizes disparity
//  - THOUGH I think minimizing disparity can lead to serious errors.
//
bool
nearest_match_along_line(
    const vsol_point_2d &pt,
    const vgl_homg_line_2d<double> &l, 
    vsol_polyline_2d_sptr crv, 
    vsol_point_2d *match_pt,
    double maxdist
    )
{
  bool stat;
  vcl_vector<bool> is_close_enough; //:< is_close_enough[i] if crv[i] is close enough to given line

  stat=mw_intersection_sets::mw_line_polyline_intersection_1(&l,crv,is_close_enough,maxdist);
  if (!stat) {
    vcl_cout <<" No intersection!!\n";
    return false;
  }

  unsigned long dmin = vcl_numeric_limits<unsigned long>::max();
  unsigned imin=0;

  for (unsigned k=0; k < crv->size(); ++k) {
    if (is_close_enough[k]) {
      long int dx = static_cast<long>(crv->vertex(k)->x() - pt.x());
      dx*=dx;
      long int dy = static_cast<long>(crv->vertex(k)->y() - pt.y());
      dy*=dy;
        
      // compute distance btw crv[k] and pt
      unsigned d = dx + dy;
      if (d < dmin) {
        dmin = d;
        imin = k;
      }
    }
  }

  *match_pt = *(crv->vertex(imin));

  return true;
}

//---------------------------------------------------------------------------

