// This is brcv/rec/dbru/pro/dbru_print_rcor_process.cxx

//:
// \file

#include "dbru_print_rcor_process.h"

//#include <vcl_ctime.h>
//#include <vcl_cmath.h>
//#include <vcl_algorithm.h>
//#include <vcl_cstdio.h>
//#include <vul/vul_timer.h>

//#include <vidpro1/storage/vidpro1_image_storage.h>
//#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
//#include <vsol/vsol_polygon_2d.h>

//#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
//#include <dbsk2d/dbsk2d_shock_graph.h>
//#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
//#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
//#include <dbsk2d/pro/dbsk2d_shock_storage.h>
//#include <dbsk2d/algo/dbsk2d_sample_ishock.h>
//#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_sm_cor_sptr.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbcvr/dbcvr_cv_cor.h>
#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>

//#include <dbinfo/dbinfo_observation.h>
//#include <dbinfo/dbinfo_observation_matcher.h>
//#include <dbinfo/dbinfo_observation_sptr.h>

#include <dbru/pro/dbru_rcor_storage_sptr.h>
#include <dbru/pro/dbru_rcor_storage.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>

//#include <dbru/algo/dbru_object_matcher.h>

//#define MARGIN  (10)

dbru_print_rcor_process::dbru_print_rcor_process()
{  
  if (
    !parameters()->add( "Use region correspondence to get sm_cor: ", "-rshock", false) ||
    !parameters()->add( "Use shock match storage to get sm_cor: ", "-shock", true) ||
    !parameters()->add( "Print boundary points too?: ", "-shockb", true) ||
    !parameters()->add( "Use curve correspondence: ", "-curve", false) ||
    !parameters()->add( "Use interpolated curve correspondence: ", "-curveint", false) ||
    !parameters()->add( "Output file <filename...>" , "-outfile" ,bpro1_filepath("","*.out") ) ||
    !parameters()->add( "increment: " , "-increment" , 20.0f) 
    ) {
      vcl_cerr << "ERROR: Adding parameters in dbru_print_rcor_process::dbru_print_rcor_process()" << vcl_endl;
    }
}


//: Clone the process
bpro1_process*
dbru_print_rcor_process::clone() const
{
  return new dbru_print_rcor_process(*this);
}

void dbru_print_rcor_process::get_parameters()
{
    //pick cor
  parameters()->get_value( "-shock" , shock_match_storage );
  parameters()->get_value( "-rshock" , rcor_storage );
  parameters()->get_value( "-shockb" , print_boundary );
  parameters()->get_value( "-curve" , curve_matching );
  parameters()->get_value( "-curveint" , curve_cont_matching );
  parameters()->get_value("-increment", increment);

  //shock matching parameters
  bpro1_filepath output_path;
  parameters()->get_value( "-outfile" , output_path);
  out_file = output_path.path;
}

bool dbru_print_rcor_process::execute()
{
  
  //first get all the parameters
  this->get_parameters();

  dbcvr_cv_cor_sptr sil_cor;
  dbskr_sm_cor_sptr shock_cor;
  dbru_rcor_sptr input_rcor;
  if (rcor_storage) {
    dbru_rcor_storage_sptr rcor_storage;
    rcor_storage.vertical_cast(input_data_[0][0]);
    input_rcor = rcor_storage->get_rcor();
    if (!curve_matching)
      shock_cor = input_rcor->get_sm_cor();
  }
  if (shock_match_storage) {
    dbskr_shock_match_storage_sptr shock_storage;
    shock_storage.vertical_cast(input_data_[0][1]);
    shock_cor = shock_storage->get_sm_cor();
  }
  
  vcl_vector<vsol_spatial_object_2d_sptr> pts1, pts2, pts1_b, pts2_b;

  if (shock_cor) {
    
    vcl_vector<dbskr_scurve_sptr>& curve_list1 = shock_cor->get_curve_list1();
    vcl_vector<dbskr_scurve_sptr>& curve_list2 = shock_cor->get_curve_list2();
    vcl_vector<vcl_vector < vcl_pair <int,int> > >& map_list = shock_cor->get_map_list();

    //: go along shock curves from the lists one by one
    if ((curve_list1.size() != curve_list2.size()) || (curve_list1.size() != map_list.size())) {
      vcl_cout << "different sizes in shock curve correspondence, not able to find region correspondence!\n";
    } else {  // corresponding shock curves

      //double step_size = 1.0;
      int inc = int(increment);
      for (unsigned int i = 0; i<curve_list1.size(); i++) {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];
        vcl_vector< vcl_pair<int, int> > mapl = map_list[i];
        //inc = (mapl.size()-1)/2;
        for (unsigned int j = 1; j<mapl.size(); j+=inc) {
          int k = mapl[j].first;
          int m = mapl[j].second;
          vgl_point_2d<double> r1_pt = sc1->sh_pt(k);
          vgl_point_2d<double> r2_pt = sc2->sh_pt(m);

          vgl_point_2d<double> r1_pt_p = sc1->bdry_plus_pt(k);
          vgl_point_2d<double> r2_pt_p = sc2->bdry_plus_pt(m);

          vgl_point_2d<double> r1_pt_m = sc1->bdry_minus_pt(k);
          vgl_point_2d<double> r2_pt_m = sc2->bdry_minus_pt(m);
        
          vsol_point_2d_sptr p1 = new vsol_point_2d(r1_pt);
          vsol_point_2d_sptr p2 = new vsol_point_2d(r2_pt);
          pts1.push_back(p1->cast_to_spatial_object());
          pts2.push_back(p2->cast_to_spatial_object());

          vsol_point_2d_sptr p1p = new vsol_point_2d(r1_pt_p);
          vsol_point_2d_sptr p2p = new vsol_point_2d(r2_pt_p);
          pts1_b.push_back(p1p->cast_to_spatial_object());
          pts2_b.push_back(p2p->cast_to_spatial_object());

          vsol_point_2d_sptr p1m = new vsol_point_2d(r1_pt_m);
          vsol_point_2d_sptr p2m = new vsol_point_2d(r2_pt_m);
          pts1_b.push_back(p1m->cast_to_spatial_object());
          pts2_b.push_back(p2m->cast_to_spatial_object());
        } 
      }
    }
  

  } else if (rcor_storage && curve_matching) { 
    
    sil_cor = input_rcor->get_sil_cor();
    vcl_vector<vgl_point_2d<double> > &vgl_pts1 = sil_cor->get_contour_pts1();
    vcl_vector<vgl_point_2d<double> > &vgl_pts2 = sil_cor->get_contour_pts2();

    if (vgl_pts1.size() != vgl_pts2.size()) {
      vcl_cout << "different sizes in curve correspondence, not able to find region correspondence!\n";
    } else {
      int inc = int(increment);
      for (unsigned i = 0; i<vgl_pts1.size(); i+=inc) {
        vsol_point_2d_sptr p1 = new vsol_point_2d(vgl_pts1[i]);
        vsol_point_2d_sptr p2 = new vsol_point_2d(vgl_pts2[i]);
        pts1.push_back(p1->cast_to_spatial_object());
        pts2.push_back(p2->cast_to_spatial_object());
      }
    }
      
  }

  //----------------------------------
  // print the output correspondences
  //----------------------------------
  vcl_ofstream outf(out_file.c_str());
  if (outf) {
    if (print_boundary)
      outf << pts1.size()+pts1_b.size() << vcl_endl;
    else
      outf << pts1.size() << vcl_endl;
   for (unsigned i = 0; i<pts1.size(); i++) {
      vsol_point_2d_sptr p = pts1[i]->cast_to_point();
      outf << p->x() << " " << p->y() << vcl_endl;
   }
   if (print_boundary)
     for (unsigned i = 0; i<pts1_b.size(); i++) {
      vsol_point_2d_sptr p = pts1_b[i]->cast_to_point();
      outf << p->x() << " " << p->y() << vcl_endl;
     }

   if (print_boundary)
      outf << pts2.size()+pts2_b.size() << vcl_endl;
   else 
      outf << pts2.size() << vcl_endl;
   for (unsigned i = 0; i<pts2.size(); i++) {
      vsol_point_2d_sptr p = pts2[i]->cast_to_point();
      outf << p->x() << " " << p->y() << vcl_endl;
   }
   if (print_boundary)
     for (unsigned i = 0; i<pts2_b.size(); i++) {
      vsol_point_2d_sptr p = pts2_b[i]->cast_to_point();
      outf << p->x() << " " << p->y() << vcl_endl;
     }
  }
  outf.close();

  //----------------------------------
  // create the output storage class
  //----------------------------------
  clear_output();
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(pts1, "pts1");
  output_data_[0].push_back(output_vsol);

  vidpro1_vsol2D_storage_sptr output_vsol2 = vidpro1_vsol2D_storage_new();
  output_vsol2->add_objects(pts2, "pts2");
  output_data_[0].push_back(output_vsol2);

  return true;
}

