#include "dbru_run_osl_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <dbru/dbru_rcor.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/pro/dbru_rcor_storage_sptr.h>
#include <dbru/pro/dbru_rcor_storage.h>
#include <dbru/algo/dbru_object_matcher.h>
#include <dbru/dbru_object.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation_matcher.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>

#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>

#define   TEMP_CON  "D:\\vxllems\\build\\apps\\shockcmd\\Debug\\tmp_con.con"
#define   TEMP_ESF  "D:\\vxllems\\build\\apps\\shockcmd\\Debug\\tmp_esf.esf"
//CAUTION: ADJUST this command according to your local paths if you'll use command line shock extraction
#define   COMMAND   "D:\\vxllems\\build\\apps\\shockcmd\\Debug\\shockcmd.exe -i D:\\vxllems\\build\\apps\\shockcmd\\Debug\\tmp_con.con -o D:\\vxllems\\build\\apps\\shockcmd\\Debug\\tmp_esf.esf -t %f"
//#define   COMMAND   "D:\\vxllems\\build\\apps\\shockcmd\\Debug\\shockcmd.exe -i tmp_con.con -o tmp_esf.esf -t %f"


bool dbru_run_osl_process::get_tree(dbskr_tree_sptr& tree1, vsol_polygon_2d_sptr poly1) {
  tree1 = new dbskr_tree(shock_curve_ds_);
  dbsk2d_shock_graph_sptr sg1;

  float pruning_threshold = pruning_threshold_;
  if (dynamic_pruning_) {
    //: adjust pruning threshold dynamically
    //  if the outlining bounding box length is 100, use threshold 0.2
    //  if this length halves, use 0.2/4
    //  if this length doubles use 0.2*4
    poly1->compute_bounding_box();
    int w = (int)vcl_floor(poly1->get_max_x()-poly1->get_min_x()+0.5);
    int h = (int)vcl_floor(poly1->get_max_y()-poly1->get_min_y()+0.5);
    vcl_cout << "w: " << w << " h: " << h << vcl_endl;
    pruning_threshold = float(vcl_pow((2*(w+h))/100.0f, 2)*base_thres_);
  }

  if (use_Amir_shock_extraction_)
    sg1 = dbsk2d_compute_shocks(poly1, pruning_threshold);
  else {  // do the ugly system call for now
    vcl_ofstream of(TEMP_CON);
    of << "CONTOUR\nCLOSE\n";  of << poly1->size() << "\n";
    for (unsigned int i = 0; i<poly1->size(); i++) {
      vgl_point_2d<double> p = poly1->vertex(i)->get_p();
      of << p.x() << " " << p.y() << "\n";
      }  
    of.close();

    char command[1000];       
    vcl_sprintf(command, COMMAND, pruning_threshold);
    vcl_cout << "command: " << command << vcl_endl;
    system(command);

    dbsk2d_xshock_graph_fileio loader;
    sg1 = loader.load_xshock_graph(TEMP_ESF);
  }   
  vcl_cout << "Number of vertices in shock graph1: " << sg1->number_of_vertices() << vcl_endl;
  if (sg1->number_of_vertices() == 0) {
    vcl_cout << "shock graph has 0 vertices!! exiting\n";
    return false;
  }
  tree1->acquire(sg1);
  return true;
}


dbru_run_osl_process::dbru_run_osl_process()
{  


  
  if (!parameters()->add( "Query object id from OSL: " , "-qid" , 0) ||
      !parameters()->add( "Query object polygon id: " , "-qpid" , 0) ||  
      !parameters()->add( "Database object id from OSL: " , "-did" , 0) ||
      !parameters()->add( "Database object polygon id: " , "-dpid" , 0) ||  
      !parameters()->add( "Shock matching, normal edit distance: ", "-shock", false) ||
      !parameters()->add( "Shock matching, edit distance pmi: ", "-shockpmi", false) ||
      !parameters()->add( "use Amir's shock extraction (otherwise system call)" , "-amir" , false )  ||
      !parameters()->add( "dynamic pruning" , "-dynamic" , true )  ||
      !parameters()->add( "if (dynamic) base threshold" , "-base" , 0.2f )  ||
      !parameters()->add( "if (!dynamic) threshold" , "-threshold" , 0.1f )  ||
      !parameters()->add( "shock curve ds" , "-ds" , 0.1f )  ||
      //!parameters()->add( "if not shock matching, then curve matching R: " , "-R" , 10.0f ) ||
      !parameters()->add( "rms for line fitting before curve matching: " , "-rms" , 0.05f ) ||
      !parameters()->add( "restricted curve match ratio: " , "-restricted" , 0.25f ) ||
      !parameters()->add( "Distance Transform: ", "-dt", false) ||
      !parameters()->add( "Line intersections: ", "-line", false) ||
      !parameters()->add( "increment: " , "-increment" , 20) ||
      !parameters()->add( "Rigid alignment: ", "-rigid", false) ||
      !parameters()->add( "if rigid alignment, dx: " , "-dxx" , 2.0f )  ||
      !parameters()->add( "if rigid alignment, dr: " , "-drr" , 0.0f )  ||
      !parameters()->add( "if rigid alignment, ds: " , "-dss" , 0.0f )  ||
      !parameters()->add( "if rigid alignment, ratio: " , "-lim", 0.1f ) ||
      !parameters()->add( "if rigid alignment, number of obs to try randomly: " , "-Nob", 10 ) 
      //!parameters()->add( "Camera image pixel range (in bits): " , "-imagebits" , 8) 
      ) {
    vcl_cerr << "ERROR: Adding parameters in dbru_run_osl_process::dbru_run_osl_process()" << vcl_endl;
  }
}


//: Clone the process
bpro1_process*
dbru_run_osl_process::clone() const
{
  return new dbru_run_osl_process(*this);
}

bool dbru_run_osl_process::execute()
{
  parameters()->get_value( "-shock" , shock_ );
  parameters()->get_value( "-shockpmi" , shock_pmi_ );
  parameters()->get_value( "-dt" , dt_ );
  parameters()->get_value( "-line" , line_ );
  
  parameters()->get_value( "-qid" , query_object_id_ );
  parameters()->get_value( "-did" , database_object_id_ );
  parameters()->get_value( "-qpid" , query_polygon_id_ );
  parameters()->get_value( "-dpid" , database_polygon_id_ );

  parameters()->get_value( "-increment" , increment_ );
  int image_bits = 8;
  parameters()->get_value("-imagebits", image_bits);
  max_value_ = float(vcl_pow(double(2.0), double(image_bits))-1);
  
  parameters()->get_value( "-rms" , rms_ );
  R_ = 10.0f;
  //parameters()->get_value( "-R" , R_ );
  parameters()->get_value( "-restricted" , restricted_cvmatch_ratio_ );

  parameters()->get_value( "-amir" , use_Amir_shock_extraction_ );
  //: CAUTION PRUNE THRESHOLD IS EXTREMELY IMPORTANT, it should be dynamically set, or input set
  //  should be examined to determine a good preset threshold
  //  If this threshold is high to much smoothing accours, the boundary loses detail
  //  otherwise too many branches stay and the shock matching takes very long time
  parameters()->get_value( "-dynamic" , dynamic_pruning_ );
  parameters()->get_value( "-threshold" , pruning_threshold_ );
  parameters()->get_value( "-base" , base_thres_ );
  parameters()->get_value( "-ds" , shock_curve_ds_ );
  parameters()->get_value( "-rigid" , rigid_alignment_ );
  parameters()->get_value( "-dxx" , dx_ );
  parameters()->get_value( "-drr" , dr_ );
  parameters()->get_value( "-dss" , ds_ );
  parameters()->get_value( "-lim" , ratio_);
  parameters()->get_value( "-Nob" , Nob_ );

  dbru_osl_storage_sptr osl_storage;
  osl_storage.vertical_cast(input_data_[0][0]);
 
  unsigned int size = osl_storage->get_osl_size();
  if (query_object_id_ < 0 || query_object_id_ >= size) {
    vcl_cout << "query object id " << query_object_id_ << " is not valid, OSL size is: " << size <<vcl_endl;
    return false;
  }
  if (database_object_id_ < 0 || database_object_id_ >= size) {
    vcl_cout << "database object id " << database_object_id_ << " is not valid, OSL size is: " << size <<vcl_endl;
    return false;
  }
  
  dbru_object_sptr q_obj = osl_storage->get_object(query_object_id_);
  dbru_object_sptr d_obj = osl_storage->get_object(database_object_id_);

  if (q_obj->n_observations() <= 0) {
    vcl_cout << "Observations of query object with id " << query_object_id_ << " are not created, exiting!\n";
    return false;
  }
  if (d_obj->n_observations() <= 0) {
    vcl_cout << "Observations of database object with id " << database_object_id_ << " are not created, exiting!\n";
    return false;
  }
  vsol_polygon_2d_sptr poly1 = q_obj->get_polygon(query_polygon_id_);
  vsol_polygon_2d_sptr poly2 = d_obj->get_polygon(database_polygon_id_);

  dbinfo_observation_sptr obs1 = q_obj->get_observation(query_polygon_id_);
  dbinfo_observation_sptr obs2 = d_obj->get_observation(database_polygon_id_);

  dbskr_tree_sptr tree1, tree2;
  if (shock_ || shock_pmi_) {
    if (!get_tree(tree1, poly1)) {
      vcl_cout << "Unable to generate tree for query object\n";
      return false;
    }
    if (!get_tree(tree2, poly2)) {
      vcl_cout << "Unable to generate tree for database object\n";
      return false;
    }
  }

  dbru_rcor_sptr output_rcor;
  dbskr_sm_cor_sptr sm_cor; dbcvr_cv_cor_sptr sil_cor;
  double cost;
  if (shock_)
    sm_cor = dbru_object_matcher::compute_shock_alignment(tree1, tree2, cost, false);
  else if (shock_pmi_)
    sm_cor = dbru_object_matcher::compute_shock_alignment_pmi(obs1, obs2, tree1, tree2, cost, false);
  else if (!rigid_alignment_) 
    sil_cor = dbru_object_matcher::compute_curve_alignment(obs1, obs2, cost, R_, rms_, restricted_cvmatch_ratio_, false);

  if (shock_ || shock_pmi_)  
    output_rcor = dbru_object_matcher::generate_rcor_shock_matching(obs1, obs2, sm_cor, true);  // verbose    
  else if (dt_)
    output_rcor = dbru_object_matcher::generate_rcor_curve_matching_dt(obs1, obs2, sil_cor, true);  // verbose    
  else if (!rigid_alignment_)
    output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line(obs1, obs2, sil_cor, true);  // verbose    
    
  float info;
  vil_image_resource_sptr output_sptr1, output_sptr2, output_sptr3;
  if (!rigid_alignment_)
    info = dbinfo_observation_matcher::minfo(obs1, obs2, output_rcor->get_correspondences(), false, max_value_);
  else                                             //query //database
    //info = dbru_object_matcher::minfo_rigid_alignment(obs1, obs2, dx_, dr_, ds_, output_sptr1, output_sptr2, output_sptr3, true);
    info = dbru_object_matcher::minfo_rigid_alignment_rand(obs1, obs2, dx_, dr_, ds_, ratio_, Nob_, output_sptr1, output_sptr2, output_sptr3, true);
  vcl_cout << "Mutual info: " << info << vcl_endl;

  clear_output();
  dbru_rcor_storage_sptr rcor_storage = dbru_rcor_storage_new();
  rcor_storage->set_rcor(output_rcor);
  output_data_[0].push_back(rcor_storage);

  dbskr_shock_match_storage_sptr sm_storage = dbskr_shock_match_storage_new();
  sm_storage->set_sm_cor(sm_cor);
  output_data_[0].push_back(sm_storage);

  if (!rigid_alignment_) {
    output_sptr3 = output_rcor->get_appearance2_on_pixels1();
    output_sptr1 = output_rcor->get_used_pixels1();
    output_sptr2 = output_rcor->get_used_pixels2();
  }

  vidpro1_image_storage_sptr output_storage1 = vidpro1_image_storage_new();
  output_storage1->set_image(output_sptr1);
  vidpro1_image_storage_sptr output_storage2 = vidpro1_image_storage_new();
  output_storage2->set_image(output_sptr2);
  vidpro1_image_storage_sptr output_storage3 = vidpro1_image_storage_new();
  output_storage3->set_image(output_sptr3);

  //output_data_[0].push_back(output_storage1);
  //output_data_[0].push_back(output_storage2);
  output_data_[0].push_back(output_storage3);
  return true;
}

