
#include <vcl_cmath.h> // for log(), exp() ..
#include <vul/vul_timer.h> 
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h> // for rand()

#include <dbru/dbru_object.h>
#include <dbru/algo/dbru_object_matcher.h>

#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>
#include <dbru/algo/dbru_rcor_generator.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <dbinfo/dbinfo_match_optimizer.h>
#include <dbinfo/dbinfo_feature_data.h>

#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <bsol/bsol_algs.h>

#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/dbskr_tree_edit_pmi.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbcvr/dbcvr_clsd_cvmatch_sptr.h>
#include <dbcvr/dbcvr_clsd_cvmatch.h>
//#include <dbcvr/dbcvr_cv_cor.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>

//----------------------------------------------------------------------------------------------------
// Mutual info between objects
//----------------------------------------------------------------------------------------------------

//: compute the mutual information between two objects using correspondences induced by curve matching
vil_image_resource_sptr dbru_object_matcher::minfo_thomas_curve_matching(dbru_object_sptr const& obj0,
                                                       dbru_object_sptr const& obji,
                                                       unsigned int obj0_poly_id,
                                                       unsigned int obji_poly_id,
                                                       double &curve_matching_cost,
                                                       double &info_line_cost,
                                                       double &info_dt_cost,
                                                       int increment,
                                                       bool find_and_return_dt_cost,
                                                       double R,
                                                       double rms,   // for line fitting before matching
                                                       bool verbose)
{
  vsol_polygon_2d_sptr poly0 = obj0->get_polygon(obj0_poly_id);
  vsol_polygon_2d_sptr polyi = obji->get_polygon(obji_poly_id);

  if (obj0->n_observations() <= 0) {
    vcl_cout << "Observations of this object are not created, exiting!\n";
    return 0;
  }

  if (obji->n_observations() <= 0) {
    vcl_cout << "Observations of this object are not created, exiting!\n";
    return 0;
  }

  dbinfo_observation_sptr obs0 = obj0->get_observation(obj0_poly_id);
  dbinfo_observation_sptr obsi = obji->get_observation(obji_poly_id);

  vul_timer t;
  dbcvr_clsd_cvmatch_sptr d1 = new dbcvr_clsd_cvmatch(poly0,polyi, R, rms, 3);
  d1->setStretchCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|
  d1->Match();

  int minIndex;                                                           // get normalized cost
  curve_matching_cost = d1->finalBestCostRestrictedStartingPoint(minIndex, 0.25f, true);
  if (verbose)
    vcl_printf("%9.6f\n",curve_matching_cost);

  dbcvr_cv_cor_sptr sil_cor = d1->get_cv_cor(minIndex);
  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi);
  
  if (!dbru_rcor_generator::find_correspondence_line(rcor, sil_cor, increment) ) {
    vcl_cout << "Region correspondence based on line intersections could not be found!\n";
    return 0;
  }

  vcl_vector <vcl_pair< unsigned, unsigned > >& corrs = rcor->get_correspondences();
  
  info_line_cost = dbinfo_observation_matcher::minfo(obs0, obsi, corrs, verbose);

  if (verbose)
    vcl_cout<< "time: "<< (t.real()/1000.0f) << " seconds " <<vcl_endl;

  vil_image_resource_sptr output_sptr = rcor->get_appearance2_on_pixels1();
  /*if (output_image != "") {
    vcl_string output_image_name = output_image+"-line.png";
    vil_image_resource_sptr output_sptr = rcor->get_appearance2_on_pixels1();
    vil_save_image_resource(output_sptr, output_image_name.c_str());
  }*/

  if (find_and_return_dt_cost) {
    //vcl_cout << "Using distance transform to find region correspondences!\n";
    //: use interpolated curves for interpolation based on arclength
    
    dbsol_interp_curve_2d_sptr curve11 = new dbsol_interp_curve_2d();
    dbsol_interp_curve_2d_sptr curve22 = new dbsol_interp_curve_2d();
    dbsol_curve_algs::interpolate_linear(curve11.ptr(), poly0);  // removed closed1, I want closed curves always
    dbsol_curve_algs::interpolate_linear(curve22.ptr(), polyi);  // removed closed2   

    rcor->clear_region_correspondence();

    float scale = (float)((double)rcor->get_upper2_x()/(double)rcor->get_upper1_x());
    if (!dbru_rcor_generator::find_correspondence_dt(rcor, sil_cor, curve11, curve22, scale)) { 
      vcl_cout << "Region correspondence based on distance transform could not be found!\n";
      return 0;
    }

    vcl_vector <vcl_pair< unsigned, unsigned > >& corrs2 = rcor->get_correspondences();
    info_dt_cost = dbinfo_observation_matcher::minfo(obs0, obsi, corrs2, false);

    return rcor->get_appearance2_on_pixels1();
  } else {
    return output_sptr;
  }
}


//: compute the mutual information between two objects using correspondences induced by shock matching
vil_image_resource_sptr dbru_object_matcher::minfo_shock_matching(dbru_object_sptr const& obj0,
                                                dbru_object_sptr const& obji,
                                                unsigned int obj0_poly_id,
                                                unsigned int obji_poly_id,
                                                vbl_array_1d<dbskr_tree_sptr> *obj0_trees,
                                                vbl_array_1d<dbskr_tree_sptr> *obji_trees,
                                                double &shock_matching_cost,
                                                double &info_cost, bool elastic_splice_cost,
                                                double thres,
                                                bool verbose) 
{
  vsol_polygon_2d_sptr poly0 = obj0->get_polygon(obj0_poly_id);
  vsol_polygon_2d_sptr polyi = obji->get_polygon(obji_poly_id);

  if (obj0->n_observations() <= 0) {
    vcl_cout << "Observations of this object are not created, exiting!\n";
    return 0;
  }

  if (obji->n_observations() <= 0) {
    vcl_cout << "Observations of this object are not created, exiting!\n";
    return 0;
  }

  dbinfo_observation_sptr obs0 = obj0->get_observation(obj0_poly_id);
  dbinfo_observation_sptr obsi = obji->get_observation(obji_poly_id);

  vul_timer t;  
  
  // just fill with 0's if this is the first time
  if (!obj0_trees->size()) {
    for (unsigned int k = 0; k<obj0->polygon_cnt_; k++)
      obj0_trees->push_back(0);
  }

  if (!obji_trees->size()) {
    for (unsigned int k = 0; k<obji->polygon_cnt_; k++)
      obji_trees->push_back(0);
  }

  dbskr_tree_sptr tree0 = (*obj0_trees)[obj0_poly_id];
  if (!tree0) {  //create this tree, its not created before
    // CAUTION: currently we're making an UGLY system call
    // this is gonna be replaced with AMir's shock extractor algorithm in dbsk2d
    // once the bugs are cleared
      vcl_ofstream of("tmp_con.con");
      of << "CONTOUR\nCLOSE\n";
      of << poly0->size() << "\n";
      for (unsigned int i = 0; i<poly0->size(); i++) {
        vgl_point_2d<double> p = poly0->vertex(i)->get_p();
        of << p.x() << " " << p.y() << "\n";
      }
      of.close();

      char command[1000];
      //: adjust pruning threshold dynamically
      //  if the outlining bounding box length is 100, use threshold 0.2
      //  if this length halves, use 0.2/4
      //  if this length doubles use 0.2*4
      poly0->compute_bounding_box();
      int w = (int)vcl_floor(poly0->get_max_x()-poly0->get_min_x()+0.5);
      int h = (int)vcl_floor(poly0->get_max_y()-poly0->get_min_y()+0.5);
      vcl_cout << "w: " << w << " h: " << h << vcl_endl;
      double pruning_threshold = vcl_pow((2*(w+h))/100.0f, 2)*thres;
      vcl_sprintf(command, "shockcmd.exe -i tmp_con.con -o tmp_esf.esf -t %f", (float)pruning_threshold);
      vcl_cout << "command: " << command << vcl_endl;
      system(command);

      dbsk2d_xshock_graph_fileio loader;
      dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph("tmp_esf.esf");
     
      if (verbose)
        vcl_cout << "Number of vertices in shock graph: " << sg->number_of_vertices() << vcl_endl;
     
      if (sg->number_of_vertices() == 0) {
        vcl_cout << "tmp_esf.esf" << " .esf has 0 vertices!!\n";
        return 0;
      }

      tree0 = new dbskr_tree(1.0f);
      tree0->acquire(sg, elastic_splice_cost, true, false);  // construct_circular_ends is true cause matching simple closed curves in this application
                                                             // dpmatch_combined is false assuming original edit distance alg of Sebastian et al. PAMI06 is being used
      (*obj0_trees)[obj0_poly_id] = tree0;
  }

  dbskr_tree_sptr treei = (*obji_trees)[obji_poly_id];
  if (!treei) {  //create this tree, its not created before
      vcl_ofstream of("tmp_con.con");
      of << "CONTOUR\nCLOSE\n";
      of << polyi->size() << "\n";
      for (unsigned int i = 0; i<polyi->size(); i++) {
        vgl_point_2d<double> p = polyi->vertex(i)->get_p();
        of << p.x() << " " << p.y() << "\n";
      }
      of.close();

      char command[1000];
      polyi->compute_bounding_box();
      int w = (int)vcl_floor(polyi->get_max_x()-polyi->get_min_x()+0.5);
      int h = (int)vcl_floor(polyi->get_max_y()-polyi->get_min_y()+0.5);
      vcl_cout << "w: " << w << " h: " << h << vcl_endl;
      double pruning_threshold = vcl_pow((2*(w+h))/100.0f, 2)*thres;
      vcl_sprintf(command, "shockcmd.exe -i tmp_con.con -o tmp_esf.esf -t %f", pruning_threshold);
      vcl_cout << "command: " << command << vcl_endl;
      system(command);

      dbsk2d_xshock_graph_fileio loader;
      dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph("tmp_esf.esf");
     
      if (verbose)
        vcl_cout << "Number of vertices in shock graph: " << sg->number_of_vertices() << vcl_endl;
     
      if (sg->number_of_vertices() == 0) {
        vcl_cout << "tmp_esf.esf" << " .esf has 0 vertices!!\n";
        return 0;
      }

      treei = new dbskr_tree(1.0f);
      treei->acquire(sg, elastic_splice_cost, true, false);  // construct_circular_ends is true cause matching simple closed curves in this application
                                                             // dpmatch_combined is false assuming original edit distance alg of Sebastian et al. PAMI06 is being used
      (*obji_trees)[obji_poly_id] = treei;
  }

  // do the shock matching
  dbskr_sm_cor_sptr sm_cor1, sm_cor2; double cost1, cost2;
  sm_cor1 = dbru_object_matcher::compute_shock_alignment(tree0,treei,cost1,false);
  sm_cor2 = dbru_object_matcher::compute_shock_alignment(treei,tree0,cost2,false);      
  dbru_rcor_sptr rcor1 = dbru_object_matcher::generate_rcor_shock_matching(obs0, obsi, sm_cor1, false);
  dbru_rcor_sptr rcor2 = dbru_object_matcher::generate_rcor_shock_matching(obsi, obs0, sm_cor2, false);
  float info1 = dbinfo_observation_matcher::minfo(obs0, obsi, rcor1->get_correspondences(), false);
  float info2 = dbinfo_observation_matcher::minfo(obsi, obs0, rcor2->get_correspondences(), false);
  if (cost1 < cost2)
    shock_matching_cost = cost1;
  else
    shock_matching_cost = cost2;

  vcl_cout << " cost " << shock_matching_cost;
  if (info1 > info2) {
    info_cost = info1;
    vcl_cout << " info: " << info1 << " matching time: "<< t.real()/1000.0f << " secs\n";
    return rcor1->get_appearance2_on_pixels1();
  } else {
    info_cost = info2;
    vcl_cout << " info: " << info2 << " matching time: "<< t.real()/1000.0f << " secs\n";
    return rcor2->get_appearance2_on_pixels1();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------
// compute curve alignment from observations
//----------------------------------------------------------------------------------------------------

//: compute curve alignment from two observations
dbcvr_cv_cor_sptr 
dbru_object_matcher::compute_curve_alignment(dbinfo_observation_sptr obs0,
                                             dbinfo_observation_sptr obsi,
                                             double &curve_matching_cost,
                                             double R,
                                             double rms,   // for line fitting before matching
                                             double restricted_cvmatch_ratio,
                                             bool verbose)
{
  if (verbose)
    vcl_cout << "Computing curve alignment...\n" ;

  //extract the polygons from the observations (assuming only one poly per observation)
  vsol_polygon_2d_sptr poly0 = obs0->geometry()->poly(0);
  vsol_polygon_2d_sptr polyi = obsi->geometry()->poly(0);

  dbcvr_clsd_cvmatch_sptr d1 = new dbcvr_clsd_cvmatch(poly0, polyi, R, rms, 3);
  d1->setStretchCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|
  d1->Match();

  int minIndex;
  curve_matching_cost = d1->finalBestCostRestrictedStartingPoint(minIndex, restricted_cvmatch_ratio, true);

  if (verbose)
    vcl_printf("%9.6f\n",curve_matching_cost);

  dbcvr_cv_cor_sptr sil_cor = d1->get_cv_cor(minIndex);

  return sil_cor;
}

 //: compute curve alignment from two observations
dbcvr_cv_cor_sptr 
dbru_object_matcher::compute_curve_alignment (vsol_polygon_2d_sptr poly0,
                                                    vsol_polygon_2d_sptr polyi,
                                                    double &curve_matching_cost,
                                                    double R,
                                                    double rms,   // for line fitting before matching
                                                    double restricted_cvmatch_ratio,
                                                    bool verbose)
{
  if (verbose)
    vcl_cout << "Computing curve alignment...\n" ;

  dbcvr_clsd_cvmatch_sptr d1 = new dbcvr_clsd_cvmatch(poly0, polyi, R, rms, 3);
  d1->setStretchCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|
  d1->Match();

  int minIndex;
  curve_matching_cost = d1->finalBestCostRestrictedStartingPoint(minIndex, restricted_cvmatch_ratio, true);

  if (verbose)
    vcl_printf("%9.6f\n",curve_matching_cost);

  dbcvr_cv_cor_sptr sil_cor = d1->get_cv_cor(minIndex);
  return sil_cor;
}

//----------------------------------------------------------------------------------------------------
// Compute region correspondece using DISTANCE TRANSFORMS
//----------------------------------------------------------------------------------------------------

//: compute the region correspondence from curve matching using distance transform mapping
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_dt(dbinfo_observation_sptr obs0,
                                                     dbinfo_observation_sptr obsi,
                                                     dbcvr_cv_cor_sptr sil_cor,
                                                     bool verbose)
{
  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi);

  if (verbose)
    vcl_cout << "Using distance transform to find region correspondences!\n";

  //extract the polygons from the observations (assuming only one poly per observation)
  vsol_polygon_2d_sptr poly0 = obs0->geometry()->poly(0);
  vsol_polygon_2d_sptr polyi = obsi->geometry()->poly(0);

  // use interpolated curves for interpolation based on arclength
  dbsol_interp_curve_2d_sptr curve11 = new dbsol_interp_curve_2d();
  dbsol_interp_curve_2d_sptr curve22 = new dbsol_interp_curve_2d();
  dbsol_curve_algs::interpolate_linear(curve11.ptr(), poly0);
  dbsol_curve_algs::interpolate_linear(curve22.ptr(), polyi);
  
  float scale = float((double)rcor->get_upper2_x()/(double)rcor->get_upper1_x());
  if (!dbru_rcor_generator::find_correspondence_dt(rcor, sil_cor, curve11, curve22, scale) ) {
    vcl_cout << "Region correspondence based on distance transform could not be found!\n";
    return 0;
  }

  return rcor;
}

//: compute the region correspondence from curve matching using distance transform mapping
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_dt(dbinfo_observation_sptr obs0,
                                                     dbinfo_observation_sptr obsi,
                                                     double R,
                                                     double rms,   // for line fitting before matching
                                                     double restricted_cvmatch_ratio,
                                                     bool verbose)
{
  double curve_matching_cost;

  //compute curve alignment first
  dbcvr_cv_cor_sptr sil_cor = compute_curve_alignment(obs0, obsi, curve_matching_cost, 
                                                      R, rms, restricted_cvmatch_ratio, 
                                                      verbose);

  return generate_rcor_curve_matching_dt(obs0, obsi, sil_cor, verbose);
}

//: compute the mutual info between two observations using curve matching and DT based correspondence
vil_image_resource_sptr 
dbru_object_matcher::minfo_curve_matching_dt(dbinfo_observation_sptr obs0,
                                             dbinfo_observation_sptr obsi,
                                             dbcvr_cv_cor_sptr sil_cor,
                                             double &info_dt,
                                             bool verbose)
{
  dbru_rcor_sptr rcor = generate_rcor_curve_matching_dt(obs0, obsi, sil_cor, verbose);

  vcl_vector <vcl_pair< unsigned, unsigned > >& corrs = rcor->get_correspondences();
  info_dt = dbinfo_observation_matcher::minfo(obs0, obsi, corrs, false);

  vil_image_resource_sptr output_sptr = rcor->get_appearance2_on_pixels1();
  return output_sptr;
}

//: compute the mutual info between two observations using curve matching and DT based correspondence
vil_image_resource_sptr 
dbru_object_matcher::minfo_curve_matching_dt(dbinfo_observation_sptr obs0,
                                             dbinfo_observation_sptr obsi,
                                             double &curve_matching_cost,
                                             double &info_dt,
                                             double R,
                                             double rms,   // for line fitting before matching
                                             double restricted_cvmatch_ratio,
                                             bool verbose)
{
  vul_timer t;

  //compute curve alignment first
  dbcvr_cv_cor_sptr sil_cor = compute_curve_alignment(obs0, obsi, curve_matching_cost, 
                                                      R, rms, restricted_cvmatch_ratio, 
                                                      verbose);

  vil_image_resource_sptr output_sptr = minfo_curve_matching_dt(obs0, obsi, 
                                                                sil_cor, info_dt,
                                                                verbose);
  
  if (verbose)
    vcl_cout<< "time: "<< (t.real()/1000.0f) << " seconds " <<vcl_endl;
  
  return output_sptr;
}

//----------------------------------------------------------------------------------------------------
// Compute region correspondece using DISTANCE TRANSFORM algorithm's version 2
//----------------------------------------------------------------------------------------------------
  
//: compute the region correspondence from curve matching using distance transform mapping
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_dt2(dbinfo_observation_sptr obs0,
                                                      dbinfo_observation_sptr obsi,
                                                      dbcvr_cv_cor_sptr sil_cor,
                                                      float ratio,
                                                      bool verbose)
{
  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi);

  if (verbose)
    vcl_cout << "Using distance transform to find region correspondences!\n";

  //extract the polygons from the observations (assuming only one poly per observation)
  vsol_polygon_2d_sptr poly0 = obs0->geometry()->poly(0);
  vsol_polygon_2d_sptr polyi = obsi->geometry()->poly(0);

  // use interpolated curves for interpolation based on arclength
  dbsol_interp_curve_2d_sptr curve11 = new dbsol_interp_curve_2d();
  dbsol_interp_curve_2d_sptr curve22 = new dbsol_interp_curve_2d();
  dbsol_curve_algs::interpolate_linear(curve11.ptr(), poly0);
  dbsol_curve_algs::interpolate_linear(curve22.ptr(), polyi);
  
  float scale = float((double)rcor->get_upper2_x()/(double)rcor->get_upper1_x());
  if (!dbru_rcor_generator::find_correspondence_dt2(rcor, sil_cor, curve11, curve22, scale, ratio) ) {
    vcl_cout << "Region correspondence based on distance transform could not be found!\n";
    return 0;
  }
  return rcor;
}

//: compute the region correspondence from curve matching using distance transform mapping
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_dt2(dbinfo_observation_sptr obs0,
                                                      dbinfo_observation_sptr obsi,
                                                      double R,
                                                      double rms,   // for line fitting before matching
                                                      double restricted_cvmatch_ratio,
                                                      float ratio,
                                                      bool verbose)
{
  double curve_matching_cost;

  //compute curve alignment first
  dbcvr_cv_cor_sptr sil_cor = compute_curve_alignment(obs0, obsi, curve_matching_cost, 
                                                      R, rms, restricted_cvmatch_ratio, 
                                                      verbose);

  return generate_rcor_curve_matching_dt2(obs0, obsi, sil_cor, ratio, verbose);

}

//----------------------------------------------------------------------------------------------------
// Compute region correspondece using LINE INTERSECTIONS
//----------------------------------------------------------------------------------------------------

//: compute the region correspondence from curve matching using line-intersectionmapping
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_line(dbinfo_observation_sptr obs0,
                                                       dbinfo_observation_sptr obsi,
                                                       dbcvr_cv_cor_sptr sil_cor,
                                                       int increment, 
                                                       bool save_histograms,
                                                       bool verbose)
{
  if (verbose)
    vcl_cout << "Using line intersections to find region correspondences!\n";

  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi, save_histograms);

  if (!dbru_rcor_generator::find_correspondence_line(rcor, sil_cor, increment)) {
    vcl_cout << "Region correspondence based on line intersections could not be found!\n";
    return 0;
  }

  return rcor;
}

//: compute the region correspondence from curve matching using line-intersection mapping
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_line(dbinfo_observation_sptr obs0,
                                                       dbinfo_observation_sptr obsi,
                                                       double R,
                                                       double rms,   // for line fitting before matching
                                                       double restricted_cvmatch_ratio,
                                                       int increment,
                                                       bool save_histograms,
                                                       bool verbose)
{
  double curve_matching_cost;

  //compute curve alignment first
  dbcvr_cv_cor_sptr sil_cor = compute_curve_alignment(obs0, obsi, curve_matching_cost, 
                                                      R, rms, restricted_cvmatch_ratio, 
                                                      verbose);

  return generate_rcor_curve_matching_line(obs0, obsi, sil_cor, increment, save_histograms, verbose);
}

//: compute the mutual info between two observations using curve matching and 
//line-intersection based correspondence
vil_image_resource_sptr 
dbru_object_matcher::minfo_curve_matching_line(dbinfo_observation_sptr obs0,
                                                                    dbinfo_observation_sptr obsi,
                                                                    dbcvr_cv_cor_sptr sil_cor,
                                                                    double &info_line,
                                                                    int increment,
                                                                    bool verbose) 
{
  dbru_rcor_sptr rcor = generate_rcor_curve_matching_line(obs0, obsi, sil_cor, increment, verbose);

  vcl_vector <vcl_pair< unsigned, unsigned > >& corrs = rcor->get_correspondences();
  info_line = dbinfo_observation_matcher::minfo(obs0, obsi, corrs, verbose);
  
  vil_image_resource_sptr output_sptr = rcor->get_appearance2_on_pixels1();
  return output_sptr;
}

//: compute the mutual info between two observations using curve matching and 
//line-intersection based correspondence
vil_image_resource_sptr 
dbru_object_matcher::minfo_curve_matching_line(dbinfo_observation_sptr obs0,
                                               dbinfo_observation_sptr obsi,
                                               double &curve_matching_cost,
                                               double &info_line,
                                               double R,
                                               double rms,   // for line fitting before matching
                                               double restricted_cvmatch_ratio,
                                               int increment,
                                               bool verbose)
{
  vul_timer t;

  //compute curve alignment first
  dbcvr_cv_cor_sptr sil_cor = compute_curve_alignment(obs0, obsi, curve_matching_cost, 
                                                      R, rms, restricted_cvmatch_ratio, 
                                                      verbose);

  vil_image_resource_sptr output_sptr = minfo_curve_matching_line(obs0, obsi, 
                                                                  sil_cor, info_line, 
                                                                  increment, 
                                                                  verbose);

  if (verbose)
    vcl_cout<< "time: "<< (t.real()/1000.0f) << " seconds " <<vcl_endl;
  
  return output_sptr;
}

//----------------------------------------------------------------------------------------------------
// Compute region correspondece using LINE INTERSECTIONS version 2 using arclength
//----------------------------------------------------------------------------------------------------
  
//: compute the region correspondence from curve matching using line-intersection mapping
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_line2(dbinfo_observation_sptr obs0,
                                                        dbinfo_observation_sptr obsi,
                                                        dbcvr_cv_cor_sptr sil_cor,
                                                        float ratio,
                                                        bool save_histograms,
                                                        bool verbose) 
{
  if (verbose)
    vcl_cout << "Using line intersections version 2 to find region correspondences!\n";

  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi, save_histograms);

  if (!dbru_rcor_generator::find_correspondence_line2(rcor, sil_cor, ratio)) {
    vcl_cout << "Region correspondence based on line intersections version 2 could not be found!\n";
    return 0;
  }

  return rcor;
}

//: compute the region correspondence from curve matching using line-intersection mapping version 2
dbru_rcor_sptr dbru_object_matcher::generate_rcor_curve_matching_line2(dbinfo_observation_sptr obs0,
                                                                       dbinfo_observation_sptr obsi,
                                                                       double R,
                                                                       double rms,   // for line fitting before matching
                                                                       double restricted_cvmatch_ratio,
                                                                       float ratio,
                                                                       bool save_histograms,
                                                                       bool verbose)  
{
  double curve_matching_cost;

  //compute curve alignment first
  dbcvr_cv_cor_sptr sil_cor = compute_curve_alignment(obs0, obsi, curve_matching_cost, 
                                                      R, rms, restricted_cvmatch_ratio, 
                                                      verbose);

  return generate_rcor_curve_matching_line2(obs0, obsi, sil_cor, ratio, save_histograms, verbose);
}

//----------------------------------------------------------------------------------------------------
// Compute region correspondece using LINE INTERSECTIONS version 3
//----------------------------------------------------------------------------------------------------

//: compute the region correspondence from curve matching using line-intersection mapping version 3
dbru_rcor_sptr dbru_object_matcher::generate_rcor_curve_matching_line3(dbinfo_observation_sptr obs0,
                                                                       dbinfo_observation_sptr obsi,
                                                                       dbcvr_cv_cor_sptr sil_cor,
                                                                       int increment,
                                                                       bool save_histograms,
                                                                       bool verbose) 
{
  if (verbose)
    vcl_cout << "Using line intersections version 3 to find region correspondences!\n";

  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi, save_histograms);

  if (!dbru_rcor_generator::find_correspondence_line3(rcor, sil_cor, increment)) {
    vcl_cout << "Region correspondence based on line intersections version 3 could not be found!\n";
    return 0;
  }

  return rcor;
}

//: compute the region correspondence from curve matching using line-intersection mapping version 3
dbru_rcor_sptr dbru_object_matcher::generate_rcor_curve_matching_line3(dbinfo_observation_sptr obs0,
                                                                       dbinfo_observation_sptr obsi,
                                                                       double R,
                                                                       double rms,   // for line fitting before matching
                                                                       double restricted_cvmatch_ratio,
                                                                       int increment,
                                                                       bool save_histograms,
                                                                       bool verbose)
{
  double curve_matching_cost;

  //compute curve alignment first
  dbcvr_cv_cor_sptr sil_cor = compute_curve_alignment(obs0, obsi, curve_matching_cost, 
                                                      R, rms, restricted_cvmatch_ratio, 
                                                      verbose);

  return generate_rcor_curve_matching_line3(obs0, obsi, sil_cor, increment, save_histograms, verbose);
}

//----------------------------------------------------------------------------------------------------
// Compute region correspondece using LINE INTERSECTIONS version 4
//----------------------------------------------------------------------------------------------------
  
//: compute the region correspondence from curve matching using line-intersection mapping version 3
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_line4(dbinfo_observation_sptr obs0,
                                                        dbinfo_observation_sptr obsi,
                                                        dbcvr_cv_cor_sptr sil_cor,
                                                        int total_votes,
                                                        bool save_histograms,
                                                        bool verbose) 
{
  if (verbose)
    vcl_cout << "Using line intersections version 4 to find region correspondences!\n";
  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi, save_histograms);
  if (!dbru_rcor_generator::find_correspondence_line4(rcor, sil_cor, total_votes)) {
    vcl_cout << "Region correspondence based on line intersections version 3 could not be found!\n";
    return 0;
  }
  return rcor;
}

//: compute the region correspondence from curve matching using line-intersection mapping version 3
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_curve_matching_line4(dbinfo_observation_sptr obs0,
                                                        dbinfo_observation_sptr obsi,
                                                        double R,
                                                        double rms,   // for line fitting before matching
                                                        double restricted_cvmatch_ratio,
                                                        int total_votes,
                                                        bool save_histograms,
                                                        bool verbose) 
{
  double curve_matching_cost;

  //compute curve alignment first
  dbcvr_cv_cor_sptr sil_cor = compute_curve_alignment(obs0, obsi, curve_matching_cost, 
                                                      R, rms, restricted_cvmatch_ratio, 
                                                      verbose);

  return generate_rcor_curve_matching_line4(obs0, obsi, sil_cor, total_votes, save_histograms, verbose);

}

//----------------------------------------------------------------------------------------------------
// Compute shock alignment 
//----------------------------------------------------------------------------------------------------
dbskr_sm_cor_sptr 
dbru_object_matcher::compute_shock_alignment(dbskr_tree_sptr tree0,
                                             dbskr_tree_sptr treei,
                                             double &shock_matching_cost,
                                             bool verbose) 
{
  // do the shock matching
  if (verbose)
    vcl_cout << " matching shock graphs... \n";

  dbskr_tree_edit edit(tree0, treei);
  edit.save_path(true);
  
  if (!edit.edit()) {
    vcl_cout << "Problems in editing trees\n";
    return 0;
  }

  float val = edit.final_cost();
  //shock_costs[i][j] = val/(polygon_lengths[i]+polygon_lengths[j]);
  //shock_matching_cost = val/(tree0->total_splice_cost()+treei->total_splice_cost());
  shock_matching_cost = val;

  if (verbose)
    vcl_cout << "shock matching cost: " << shock_matching_cost << vcl_endl;

  //edit.write_shgm(shgm_file);
  dbskr_sm_cor_sptr sm_cor = edit.get_correspondence();

  return sm_cor;
}
//: use edit distance using independent parts' mutual info to align shock branches
dbskr_sm_cor_sptr 
dbru_object_matcher::compute_shock_alignment_pmi(dbinfo_observation_sptr obs0,
                                                 dbinfo_observation_sptr obsi,
                                                 dbskr_tree_sptr tree0,
                                                 dbskr_tree_sptr treei,
                                                 double &shock_matching_cost,
                                                 bool verbose) 
{
// do the shock matching
  if (verbose)
    vcl_cout << " matching shock graphs... \n";

  dbskr_tree_edit_pmi edit(tree0, treei, obs0, obsi);
  edit.save_path(true);
  
  if (!edit.edit()) {
    vcl_cout << "Problems in editing trees\n";
    return 0;
  }

  float val = edit.final_cost();
  //shock_matching_cost = val/(tree0->total_splice_cost()+treei->total_splice_cost());
  shock_matching_cost = val;

  if (verbose)
    vcl_cout << "shock matching cost: " << shock_matching_cost << vcl_endl;

  //edit.write_shgm(shgm_file);
  dbskr_sm_cor_sptr sm_cor = edit.get_correspondence();
  return sm_cor;
}


//----------------------------------------------------------------------------------------------------
// Compute region correspondence using SHOCK MATCHING
//----------------------------------------------------------------------------------------------------

//: compute the region correspondence from shock matching
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_shock_matching(dbinfo_observation_sptr obs0,
                                                  dbinfo_observation_sptr obsi,
                                                  dbskr_sm_cor_sptr sm_cor,
                                                  bool verbose)
{
  if (verbose)
    vcl_cout << "Using shock matching to find region correspondences!\n";

  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi);

  if (!dbru_rcor_generator::find_correspondence_shock(rcor, sm_cor) ) {
    vcl_cout << "Region correspondence based on shock matching could not be found!\n";
    return 0;
  }

  return rcor;
}

//: compute the region correspondence from shock matching
dbru_rcor_sptr 
dbru_object_matcher::generate_rcor_shock_matching(dbinfo_observation_sptr obs0,
                                                  dbinfo_observation_sptr obsi,
                                                  dbskr_tree_sptr tree0,
                                                  dbskr_tree_sptr treei,
                                                  bool verbose)
{
  double shock_matching_cost;

  //compute shock alignment first
  dbskr_sm_cor_sptr sm_cor = compute_shock_alignment(tree0, treei,
                                                     shock_matching_cost, 
                                                     verbose);

  return generate_rcor_shock_matching(obs0, obsi, sm_cor, verbose);
}

//: compute the mutual info between two observations shock-matching based correspondence
vil_image_resource_sptr 
dbru_object_matcher::minfo_shock_matching(dbinfo_observation_sptr obs0,
                                          dbinfo_observation_sptr obsi,
                                          dbskr_sm_cor_sptr sm_cor,
                                          double &info_shock,
                                          bool verbose)
{
  dbru_rcor_sptr rcor = generate_rcor_shock_matching(obs0, obsi, sm_cor, verbose);

  vcl_vector <vcl_pair< unsigned, unsigned > >& corrs = rcor->get_correspondences();
  info_shock = dbinfo_observation_matcher::minfo(obs0, obsi, corrs, verbose);

  vil_image_resource_sptr output_sptr = rcor->get_appearance2_on_pixels1();

  return output_sptr;
}

//: compute the mutual info between two observations using shock-matching based correspondence
vil_image_resource_sptr 
dbru_object_matcher::minfo_shock_matching(dbinfo_observation_sptr obs0,
                                          dbinfo_observation_sptr obsi,
                                          dbskr_tree_sptr tree0,
                                          dbskr_tree_sptr treei,
                                          double &shock_matching_cost,
                                          double &info_shock,
                                          bool verbose) 
{
  vul_timer t;

  //compute shock alignment first
  dbskr_sm_cor_sptr sm_cor = compute_shock_alignment(tree0, treei,
                                                     shock_matching_cost, 
                                                     verbose);

  vil_image_resource_sptr output_sptr = minfo_shock_matching(obs0, obsi, 
                                                             sm_cor, info_shock, 
                                                             verbose);
  if (verbose)
    vcl_cout<< "time: "<< (t.real()/1000.0f) << " seconds " <<vcl_endl;
  
  return output_sptr;
}

//: make an image from observation data
vil_image_resource_sptr make_image(dbinfo_observation_sptr obs, int w, int h, double translation_x, double translation_y, bool show_contour) {
  dbinfo_region_geometry_sptr geo = obs->geometry();
  vsol_polygon_2d_sptr poly = geo->poly(0);

  vil_image_view<float> image_out(w,h,1);  // number of planes = 1
  // fill with random noise
  for (int y = 0; y<h; y++ ) 
    for (int x = 0; x<w; x++)
      image_out(x,y) = 255*float(vcl_rand()/(RAND_MAX+1.0));     

  dbinfo_feature_data_base_sptr d = (obs->features())[0]->data();
  assert(d->format() == DBINFO_INTENSITY_FEATURE);
  dbinfo_feature_data<vbl_array_1d<float> >* cd = dbinfo_feature_data<vbl_array_1d<float> >::ptr(d);
  vbl_array_1d<float>& v = cd->single_data();       

  for(unsigned k = 0; k<v.size(); ++k)
  {
    float vv0 = v[k];
    vgl_point_2d<float> coord = geo->point(k);
    int yy = (int)vcl_floor(coord.y()+translation_y+0.5);
    int xx = (int)vcl_floor(coord.x()+translation_x+0.5);
    if (xx < 0 || yy < 0 || xx >= w || yy >= h) continue;
    image_out(xx,yy) = vv0;
  }
  if (show_contour) {
    for (unsigned k = 0; k<poly->size(); k++) {
      vsol_point_2d_sptr p = poly->vertex(k);
      int yy = (int)vcl_floor(p->y()+translation_y+0.5);
      int xx = (int)vcl_floor(p->x()+translation_x+0.5);
      if (xx < 0 || yy < 0 || xx >= w || yy >= h) continue;
      image_out(xx,yy) = 0;
    }
  }

  vil_image_view<vxl_byte> output_img;
  vil_convert_stretch_range(image_out, output_img);
  return vil_new_image_resource_of_view(output_img);
}

//: find the rigid transformation that aligns observation0 to 
// observationi such that mutual information is maximised and 
// return this max mutual info
// this function is to visualize the final optimized observations
// ratio is the ratio of pixels to be randomly searched in query observation
// prior to optimization to find a better initial placement 
float dbru_object_matcher::minfo_rigid_alignment_rand(dbinfo_observation_sptr obs0, 
                                                      dbinfo_observation_sptr obsi, 
                                                      float dx, float dr, float ds, float ratio, int Nob,
                                                      vil_image_resource_sptr& image_r1,
                                                      vil_image_resource_sptr& image_r2,
                                                      vil_image_resource_sptr& image_r3,
                                                      bool verbose) 
{
  // make an image at the size of database observations's image, blank initially,
  // but dump pixel values from query observation such that its center of gravity 
  // coincides with database observation's center of gravity on the image.
  // obs0: query, obsi: database

  vul_timer t;

  dbinfo_region_geometry_sptr geo_db = obsi->geometry();
  dbinfo_region_geometry_sptr geo_q = obs0->geometry();

  //double radius_db = geo_db->diameter()/2.0f;
  //double radius_q = geo_q->diameter()/2.0f;
  vsol_point_2d_sptr cent_db = geo_db->centroid();
  vsol_point_2d_sptr cent_q = geo_q->centroid();
  vcl_cout << "center of db obs: " << *cent_db << " center of query: " << *cent_q << "\n";

  int w = geo_db->cols();
  int h = geo_db->rows();
  
  image_r1 = make_image(obs0, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()), false);
  vcl_cout << "making the image of database observation also\n";
  image_r2 = make_image(obsi, w, h, 0, 0, true);

  vgl_h_matrix_2d<float> H;  H.set_identity();
  dbinfo_observation_sptr initial_obs = dbinfo_observation_generator::generate(obsi, H);
  initial_obs->scan(0, image_r1);
  //image_r2 = make_image(initial_obs, w, h, 0, 0, true);
  float initial_info = dbinfo_observation_matcher::minfo(obsi, initial_obs);
  vcl_cout << "before random iterations info is: " << initial_info << vcl_endl;
  
  vcl_cout << " size of query obs: " << geo_q->size() << vcl_endl;
  // before optimizing randomly search for a good starting point
  // we know database cog should align with some point "on" the query,
  // so pick points randomly inside the query Nob times
  float max_info = initial_info;
  vcl_srand(static_cast<unsigned int>(vcl_time(0)));
  float radius_ok = ratio*(float(geo_q->diameter())/2.0f);
  
  for (int i = 0; i<Nob; i++) {
    float x = 2*radius_ok*float(vcl_rand() / (RAND_MAX+1.0)) - radius_ok;
    float y = 2*radius_ok*float(vcl_rand() / (RAND_MAX+1.0)) - radius_ok;
    H.set_translation(x, y); 
    dbinfo_observation_sptr obs = dbinfo_observation_generator::generate(obsi, H);
    if (!obs) continue;
    obs->scan(0, image_r1);
    float info = dbinfo_observation_matcher::minfo(obsi, obs);
    if (info > max_info) {
      initial_obs = obs;
      max_info = info;
    }
  }

#if 0  // THE FOLLOWING COMMENTED PART IS SENT TO LOCKHEED BUT RANDOM SEARCH SHOULD BE ENOUGH
       // for time critique applications, especially the contours are ok, so COG alignment is relatively good
       // Nob parameter was not used in that code
  int count = 0;
  for (float x = -radius_ok; x < radius_ok; x++)
    for (float y = -radius_ok; y < radius_ok; y++) {
    vcl_cout << "x: " << x << "y: " << y << " ";
    vul_timer t2;
    H.set_translation(x, y); 
    dbinfo_observation_sptr obs = dbinfo_observation_generator::generate(obsi, H);
    if (!obs) continue;
    obs->scan(0, image_r1);
    float info = dbinfo_observation_matcher::minfo(obsi, obs);
    if (info > max_info) {
      initial_obs = obs;
      max_info = info;
    }
    vcl_cout << "info is: " << info << " time: " << t2.real()/1000.0f << " seconds.\n";
    count++;
  }
#endif
  
  vcl_cout << "after " << Nob << " random iterations the best info is: " << max_info << " time: " << t.real()/1000.0f << " seconds.\n";
  //vcl_cout << "after " << count << " iterations the best info is: " << max_info << " time: " << t.real()/1000.0f << " seconds.\n";
  //image_r2 = make_image(initial_obs, w, h, 0, 0, true);
  //image_r3 = make_image(initial_obs, w, h, 0, 0, true);
  //return max_info;
  initial_info = max_info;
  vul_timer t3;
  dbinfo_match_optimizer opt(10.0f,   dx,           dr,         ds);
  //opt.set_frame(frame_);
  opt.set_resource(image_r1);
  opt.set_debug_level(0);
  // database observation (obsi) is to stay put and query observation is to be moved around 
  // in query image to maximize mutual info
  opt.set_obs0(obsi);
  opt.set_best_seed(initial_obs);
  dbinfo_observation_sptr final_obs;
  if(opt.optimize())
    final_obs = opt.optimized_obs();

  image_r3 = make_image(final_obs, w, h, 0, 0, true);
  //float info = dbinfo_observation_matcher::minfo(obsi, final_obs);
  //vcl_cout << "final minfo: " << 10.0f-opt.current_cost() << " our info: " << info << vcl_endl;
  vcl_cout << "optimization time: " << t3.real()/1000.0f << vcl_endl;
  max_info = float(10.0f-opt.current_cost());
  return initial_info>max_info?initial_info:max_info;
  
}

//: find the rigid transformation that aligns observation0 to 
// observationi such that mutual information is maximised and 
// return this max mutual info
// this function is to visualize the final optimized observations
// ratio is the ratio of pixels to be randomly searched in query observation
// prior to optimization to find a better initial placement 
float dbru_object_matcher::minfo_rigid_alignment_rand(dbinfo_observation_sptr obs0, 
                                                      dbinfo_observation_sptr obsi, 
                                                      float dx, float dr, float ds, float ratio, int Nob) 
{
  // make an image at the size of database observations's image, blank initially,
  // but dump pixel values from query observation such that its center of gravity 
  // coincides with database observation's center of gravity on the image.
  // obs0: query, obsi: database

  vul_timer t;

  dbinfo_region_geometry_sptr geo_db = obsi->geometry();
  dbinfo_region_geometry_sptr geo_q = obs0->geometry();

  //double radius_db = geo_db->diameter()/2.0f;
  //double radius_q = geo_q->diameter()/2.0f;
  vsol_point_2d_sptr cent_db = geo_db->centroid();
  vsol_point_2d_sptr cent_q = geo_q->centroid();
  int w = geo_db->cols();
  int h = geo_db->rows();
  vil_image_resource_sptr image_r1 = make_image(obs0, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()), false);

  vgl_h_matrix_2d<float> H;  H.set_identity();
  dbinfo_observation_sptr initial_obs = dbinfo_observation_generator::generate(obsi, H);
  initial_obs->scan(0, image_r1);
  float initial_info = dbinfo_observation_matcher::minfo(obsi, initial_obs);

  // before optimizing randomly search for a good starting point
  // we know database cog should align with some point "on" the query,
  // so pick points randomly inside the query Nob times
  float max_info = initial_info;
  vcl_srand(static_cast<unsigned int>(vcl_time(0)));
  float radius_ok = ratio*(float(geo_q->diameter())/2.0f);
  
  for (int i = 0; i<Nob; i++) {
    float x = 2*radius_ok*float(vcl_rand() / (RAND_MAX+1.0)) - radius_ok;
    float y = 2*radius_ok*float(vcl_rand() / (RAND_MAX+1.0)) - radius_ok;
    H.set_translation(x, y); 
    dbinfo_observation_sptr obs = dbinfo_observation_generator::generate(obsi, H);
    if (!obs) continue;
    obs->scan(0, image_r1);
    float info = dbinfo_observation_matcher::minfo(obsi, obs);
    if (info > max_info) {
      initial_obs = obs;
      max_info = info;
    }
  }

  initial_info = max_info;
  dbinfo_match_optimizer opt(10.0f,   dx,           dr,         ds);
  opt.set_resource(image_r1);
  opt.set_debug_level(0);
  // database observation (obsi) is to stay put and query observation is to be moved around 
  // in query image to maximize mutual info
  opt.set_obs0(obsi);
  opt.set_best_seed(initial_obs);
  dbinfo_observation_sptr final_obs;
  if(opt.optimize())
    final_obs = opt.optimized_obs();
  max_info = float(10.0f-opt.current_cost());
  return initial_info>max_info?initial_info:max_info;
  
  //return max_info;
}

//: find the rigid transformation that aligns observation0 to 
// observationi such that mutual information is maximised and 
// return this max mutual info
// this function is to visualize the final optimized observations
// ratio is the ratio of pixels to be randomly searched in query observation
// prior to optimization to find a better initial placement 
// THE FOLLOWING METHOD IS SENT TO LOCKHEED in Aug 06 and Dec 06 Deliveries
// Nob parameter is not used in this method
float dbru_object_matcher::minfo_rigid_alignment_search(dbinfo_observation_sptr obs0, 
                                                        dbinfo_observation_sptr obsi, 
                                                        float dx, float dr, float ds, float ratio) 
{
  // make an image at the size of database observations's image, blank initially,
  // but dump pixel values from query observation such that its center of gravity 
  // coincides with database observation's center of gravity on the image.
  // obs0: query, obsi: database

  vul_timer t;

  dbinfo_region_geometry_sptr geo_db = obsi->geometry();
  dbinfo_region_geometry_sptr geo_q = obs0->geometry();

  //double radius_db = geo_db->diameter()/2.0f;
  //double radius_q = geo_q->diameter()/2.0f;
  vsol_point_2d_sptr cent_db = geo_db->centroid();
  vsol_point_2d_sptr cent_q = geo_q->centroid();
  int w = geo_db->cols();
  int h = geo_db->rows();
  vil_image_resource_sptr image_r1 = make_image(obs0, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()), false);

  vgl_h_matrix_2d<float> H;  H.set_identity();
  dbinfo_observation_sptr initial_obs = dbinfo_observation_generator::generate(obsi, H);
  initial_obs->scan(0, image_r1);
  float initial_info = dbinfo_observation_matcher::minfo(obsi, initial_obs);

  // before optimizing randomly search for a good starting point
  // we know database cog should align with some point "on" the query,
  // so pick points randomly inside the query Nob times
  float max_info = initial_info;
  float radius_ok = ratio*(float(geo_q->diameter())/2.0f);
  int count = 0;
  for (float x = -radius_ok; x < radius_ok; x++)
    for (float y = -radius_ok; y < radius_ok; y++) {
      H.set_translation(x, y); 
      dbinfo_observation_sptr obs = dbinfo_observation_generator::generate(obsi, H);
      if (!obs) continue;
      obs->scan(0, image_r1);
      float info = dbinfo_observation_matcher::minfo(obsi, obs);
      if (info > max_info) {
        initial_obs = obs;
        max_info = info;
      }
      count++;
    }
  //vcl_cout << "tried " << count << " translations\n";

  initial_info = max_info;
  dbinfo_match_optimizer opt(10.0f,   dx,           dr,         ds);
  opt.set_resource(image_r1);
  opt.set_debug_level(0);
  // database observation (obsi) is to stay put and query observation is to be moved around 
  // in query image to maximize mutual info
  opt.set_obs0(obsi);
  opt.set_best_seed(initial_obs);
  dbinfo_observation_sptr final_obs;
  if(opt.optimize())
    final_obs = opt.optimized_obs();
  max_info = float(10.0f-opt.current_cost());
  return initial_info>max_info?initial_info:max_info;
}


//: find the rigid transformation that aligns observation0 to 
// observationi such that mutual information is maximised and 
// return this max mutual info
// this function is to visualize the final optimized observations
float dbru_object_matcher::minfo_rigid_alignment(dbinfo_observation_sptr obs0, 
                                                 dbinfo_observation_sptr obsi, 
                                                 float dx,
                                                 float dr,
                                                 float ds,
                                                 vil_image_resource_sptr& image_r1,
                                                 vil_image_resource_sptr& image_r2,
                                                 vil_image_resource_sptr& image_r3,
                                                 bool verbose) 
{
  // make an image at the size of database observations's image, blank initially,
  // but dump pixel values from query observation such that its center of gravity 
  // coincides with database observation's center of gravity on the image.
  // obs0: query, obsi: database

  dbinfo_region_geometry_sptr geo_db = obsi->geometry();
  dbinfo_region_geometry_sptr geo_q = obs0->geometry();

  //double radius_db = geo_db->diameter()/2.0f;
  //double radius_q = geo_q->diameter()/2.0f;
  vsol_point_2d_sptr cent_db = geo_db->centroid();
  vsol_point_2d_sptr cent_q = geo_q->centroid();

  int w = geo_db->cols();
  int h = geo_db->rows();

  image_r1 = make_image(obs0, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()), false);

  vgl_h_matrix_2d<float> H;  H.set_identity();
  dbinfo_observation_sptr initial_obs = dbinfo_observation_generator::generate(obsi, H);
  initial_obs->scan(0, image_r1);
  image_r2 = make_image(initial_obs, w, h, 0, 0, true);

  dbinfo_match_optimizer opt(10.0f,   dx,           dr,         ds);
  //opt.set_frame(frame_);
  opt.set_resource(image_r1);
  opt.set_debug_level(0);
  // database observation (obsi) is to stay put and query observation is to be moved around 
  // in query image to maximize mutual info
  opt.set_obs0(obsi);
  opt.set_best_seed(initial_obs);
  dbinfo_observation_sptr final_obs;
  if(opt.optimize())
    final_obs = opt.optimized_obs();

  image_r3 = make_image(final_obs, w, h, 0, 0, true);
  float info = dbinfo_observation_matcher::minfo(obsi, final_obs);
  vcl_cout << "final minfo: " << 10.0f-opt.current_cost() << " our info: " << info << vcl_endl;
  return info;
}

//: find the rigid transformation that aligns observation0 to 
// observationi such that mutual information is maximised and 
// return this max mutual info
float dbru_object_matcher::minfo_rigid_alignment(dbinfo_observation_sptr obs0, 
                                                 dbinfo_observation_sptr obsi, 
                                                 float dx, float dr, float ds)
{
  // make an image at the size of database observations's image, blank initially,
  // but dump pixel values from query observation such that its center of gravity 
  // coincides with database observation's center of gravity on the image.
  // obs0: query, obsi: database

  dbinfo_region_geometry_sptr geo_db = obsi->geometry();
  dbinfo_region_geometry_sptr geo_q = obs0->geometry();

  //double radius_db = geo_db->diameter()/2.0f;
  //double radius_q = geo_q->diameter()/2.0f;
  vsol_point_2d_sptr cent_db = geo_db->centroid();
  vsol_point_2d_sptr cent_q = geo_q->centroid();

  int w = geo_db->cols();
  int h = geo_db->rows();

  vil_image_resource_sptr image_r1 = make_image(obs0, w, h, (-cent_q->x()+cent_db->x()), (-cent_q->y()+cent_db->y()), false);
  vgl_h_matrix_2d<float> H;  H.set_identity();
  dbinfo_observation_sptr initial_obs = dbinfo_observation_generator::generate(obsi, H);
  initial_obs->scan(0, image_r1);
  dbinfo_match_optimizer opt(10.0f,   dx,           dr,         ds);
  opt.set_resource(image_r1);
  // database observation (obsi) is to stay put and query observation is to be moved around 
  // in query image to maximize mutual info
  opt.set_obs0(obsi);
  opt.set_best_seed(initial_obs);
 
  if(opt.optimize()) {
    return float(10.0f-opt.current_cost());
  } else
    return 0.0f;
}



//: find the rigid transformation that aligns observation0 to 
// observationi such that mutual information is maximised and 
// return this max mutual info
/*float dbru_object_matcher::minfo_rigid_alignment(dbinfo_observation_sptr obs0, 
                                                 dbinfo_observation_sptr obsi, 
                                                 float dx,
                                                 float dr,
                                                 float ds,
                                                 vil_image_resource_sptr& image_r1,
                                                 vil_image_resource_sptr& image_r2,
                                                 vil_image_resource_sptr& image_r3,
                                                 bool verbose) 
{
  //translate database geometry to create the observation which will be moved around over query image
  dbinfo_region_geometry_sptr geo_i = obsi->geometry();
  double radius = (geo_i->diameter())/2.0;
  vsol_point_2d_sptr cent = geo_i->centroid();
  vgl_h_matrix_2d<float> H;
  H.set_identity();
  H.set_translation(-(cent->x()-radius), -(cent->y()-radius));
  // make a new observation from database geometry to run it over query image (translated into 
  // the valid image area, just to start with)
  vcl_vector<vsol_polygon_2d_sptr> new_polys;
  for (unsigned i = 0; i<geo_i->n_polys(); i++) {
    vsol_polygon_2d_sptr new_p = bsol_algs::transform_about_point(geo_i->poly(i), cent, H);
    new_polys.push_back(new_p);
  }
  
  // width and height of the image should be 2*radius, 
  image_r1 = make_image(obs0, radius, false);

  //dbinfo_observation_sptr initial_obs = new dbinfo_observation(0, image_r1, new_polys, true, true, false);
  obsi->geom_->trans
  dbinfo_observation_sptr initial_obs = dbinfo_observation_generator::generate(obsi, H);
  initial_obs->scan(0, image_r1);
  image_r2 = make_image(initial_obs, radius, true);

                          //max_info, search_radius_, angle_range_, scale_range_
  dbinfo_match_optimizer opt(10.0f,   dx,           dr,         ds);
  //opt.set_frame(frame_);
  opt.set_resource(image_r1);
  opt.set_debug_level(10);
  // database observation (obsi) is to stay put and query observation is to be moved around 
  // in query image to maximize mutual info
  opt.set_obs0(obsi);
  opt.set_best_seed(initial_obs);
  dbinfo_observation_sptr final_obs;
  if(opt.optimize())
    final_obs = opt.optimized_obs();

  image_r3 = make_image(final_obs, radius, true);
  return dbinfo_observation_matcher::minfo(obsi, final_obs);
}
*/

//----------------------------------------------------------------------------------------------------
// Compute region correspondence using TPS
//----------------------------------------------------------------------------------------------------
//: compute the region correspondence using TPS when initial correspondences are
//  coming from silhouette correspondence
// if there are 100 corresponding points in sil_cor then an increment of 4 causes 
// 25 points to be used by tps
dbru_rcor_sptr dbru_object_matcher::generate_rcor_tps(dbinfo_observation_sptr obs0,
                                                      dbinfo_observation_sptr obsi,
                                                      dbcvr_cv_cor_sptr sil_cor,
                                                      int increment, 
                                                      bool verbose) 
{
  dbru_rcor_sptr rcor = new dbru_rcor(obs0, obsi);
  rcor->set_sil_cor(sil_cor);

  // find the transformation
  mbl_thin_plate_spline_2d tps;
  // normalize the data points so that SVD finds a solution
  vgl_norm_trans_2d<double> trans1, trans2;
  
  if (!dbru_rcor_generator::find_correspondence_tps_curve(rcor, tps, trans1, trans2, increment) ) {
    vcl_cout << "Region correspondence based on TPS could not be found!\n";
    return 0;
  }
  return rcor;
}
