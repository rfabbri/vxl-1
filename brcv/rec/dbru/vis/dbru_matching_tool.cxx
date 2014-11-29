// This is brcv/rec/dbru/vis/dbru_matching_tool.cxx
//:
// \file

#include <dbru/vis/dbru_matching_tool.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbru/pro/dbru_rcor_storage.h>
#include <dbru/algo/dbru_object_matcher.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbru/dbru_rcor.h>

#include <dbcvr/dbcvr_cv_cor_sptr.h>
#include <dbskr/dbskr_sm_cor_sptr.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vgui/vgui_projection_inspector.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // for
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

//CAUTION: ADJUST this command according to your local paths if you'll use command line shock extraction
#define   COMMAND   "D:\\vxllems\\build\\apps\\shockcmd\\Debug\\shockcmd.exe -i tmp_con.con -o tmp_esf.esf -t %f"

bool dbru_matching_tool::get_tree(dbskr_tree_sptr& tree1, vsol_polygon_2d_sptr poly1) {
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
    vcl_ofstream of("tmp_con.con");
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
    sg1 = loader.load_xshock_graph("tmp_esf.esf");
  }   
  vcl_cout << "Number of vertices in shock graph1: " << sg1->number_of_vertices() << vcl_endl;
  if (sg1->number_of_vertices() == 0) {
    vcl_cout << "shock graph has 0 vertices!! exiting\n";
    return false;
  }
  //: CAUTION: with the following flag, it is assumed that original edit distance of Sebastian et al. PAMI06 is being used for matching
  bool dpmatch_combined = false;
  tree1->acquire(sg1, elastic_splice_cost_, true, dpmatch_combined);  // construct_circular_ends is true since matching closed curves in this application
  return true;
}


//: Constructor - protected
dbru_matching_tool::dbru_matching_tool()
{
  activation_ok_ = true;

  //: add active polygon to query list
  gesture_set_query = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  //: add active polygon to database list
  gesture_set_database  = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
  
  gesture_run = vgui_event_condition(vgui_key('r'), vgui_MODIFIER_NULL, true);
  gesture_open_dialog = vgui_event_condition(vgui_key('o'), vgui_MODIFIER_NULL, true);
  
  //gesture_clear_query = vgui_event_condition(vgui_key('q'), vgui_MODIFIER_NULL, true);    
  //gesture_clear_database = vgui_event_condition(vgui_key('d'), vgui_MODIFIER_NULL, true);    
  gesture_right_arrow = vgui_event_condition(vgui_CURSOR_RIGHT, vgui_MODIFIER_NULL, true);
  gesture_left_arrow = vgui_event_condition(vgui_CURSOR_LEFT, vgui_MODIFIER_NULL, true);
}

vcl_string
dbru_matching_tool::name() const
{
  return "Vehicle matching Tool";
}

//: This is called when the tool is activated
void dbru_matching_tool::activate() {
  
  // normal edit distance algorithm which uses elastic curve matching algo on boundaries to get deformation score
  // for branch correspondences
  choices_.push_back("normal edit distance");
  // edit distance algorithm which uses part mutual infos (assuming they're independent) as deformation score
  choices_.push_back("pmi edit distance");
  choices_.push_back("dt");
  choices_.push_back("line intersections");
  choice_ = 0;
  rms_ = 0.0f;
  R_ = 10.0f;
  restricted_cvmatch_ratio_ = 0.25f;
  dynamic_pruning_ = false;
  pruning_threshold_ = 1.0f;
  shock_curve_ds_ = 1.0f;
 
  //: use the elastic splice cost while computing scurve splice costs of the tree leaves, if FALSE, it uses the theoretic value in Sebastian,Klein,Kimia,PAMI edit distance paper.
  elastic_splice_cost_ = true;
  vcl_cout << "CAUTION: Elastic_splice_cost flag is set TRUE during tool activation\n";
  
  //set up the views for optimal usage  
  vcl_vector<bvis1_view_tableau_sptr> views = bvis1_manager::instance()->get_views();
  if (views.size() < 2) {
    vcl_cout << "Need two views for this tool, please add the second view!\n";
    return;
  }
  //: assuming we have two views
  active_view_ = views[0]; other_view_ = views[1];
  other_view_->selector()->toggle("image0");  //make it invisible
  
  //: need rcor_storage and shock_match storage
  rcor_storage_ = dbru_rcor_storage_new();
  shock_match_storage_ = dbskr_shock_match_storage_new();
  image_storage1_ = vidpro1_image_storage_new();
  image_storage2_ = vidpro1_image_storage_new();
  image_storage3_ = vidpro1_image_storage_new();
  image_storage1_->set_name("image1");
  image_storage2_->set_name("image2");
  image_storage3_->set_name("image3");
  rcor_storage_->set_name("rcor0");
  shock_match_storage_->set_name("shock_match0");
  bvis1_manager::instance()->repository()->store_data_at(rcor_storage_, other_view_->frame());
  bvis1_manager::instance()->repository()->store_data_at(shock_match_storage_, other_view_->frame());
  bvis1_manager::instance()->add_to_display(rcor_storage_);
  bvis1_manager::instance()->add_to_display(shock_match_storage_);
  active_view_->selector()->toggle("rcor0");
  active_view_->selector()->toggle("shock_match0");
  
  bvis1_manager::instance()->display_current_frame();

  vgui_dialog open_dl("Open file");
  vcl_string poly_filename = "";
  poly_filename.append(".txt");
  static vcl_string regexp = "*.*";
  open_dl.file("Video Object Polynoms Filename: ", regexp, poly_filename);
  //start_frame_ = 0;
  //end_frame_ = 1;
  //open_dl.field("Start frame ", start_frame_);
  //open_dl.field("End frame ", end_frame_);
  open_dl.ask();
  vcl_ifstream fs(poly_filename.c_str());
  
  if (!fs) {
    vcl_cout << "Problems in opening file: " << poly_filename << "\n";
    activation_ok_ = false;
  } else {
    vcl_string dummy;

    fs >> dummy; // VIDEOID:
    if (dummy != "VIDEOID:" && dummy != "FILEID:" && dummy != "VIDEOFILEID:") {
      vcl_cout << "No video id specified in input file!\n";
      video_id_ = 0;
      return;
    } else {
      fs >> video_id_;
      fs >> dummy;   // NFRAMES:
    }
    
    fs >> frame_cnt_;

    //: initialize polygon vector
    for (int i = 0; i<frame_cnt_; i++) {
      vcl_vector<vgui_soview2D_polygon*> tmp;
      polygons_.push_back(tmp);
    }

    active_style_ = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
    normal_style_ = vgui_style::new_style(0.0f, 0.0f, 1.0f, 3.0f, 3.0f);

    for (int i = 0; i<frame_cnt_; i++)  {  // read each frame
      //: currently assuming that for each polygon I have one polygon,
      //  but if we decide to use shadow separately, there might be
      //  more than one in the future.
      fs >> dummy;   // NOBJECTS:   
      fs >> dummy;   // 

      int polygon_cnt;
      fs >> dummy;   // NPOLYS:
      fs >> polygon_cnt;
      for (int j = 0; j<polygon_cnt; j++) {
        fs >> dummy; // NVERTS: 
        int vertex_cnt;
        fs >> vertex_cnt;
        vcl_vector<float> x_corners(vertex_cnt), y_corners(vertex_cnt);
        
        fs >> dummy; // X: 
        for (int k = 0; k<vertex_cnt; k++) 
          fs >> x_corners[k];

        fs >> dummy; // Y: 
        for (int k = 0; k<vertex_cnt; k++) 
          fs >> y_corners[k];

        vgui_soview2D_polygon *poly = new vgui_soview2D_polygon(vertex_cnt, &x_corners[0], &y_corners[0],true);
        polygons_[i].push_back(poly);

        
      }
    }
  }
  
  fs.close();
  
  //: initialize normal_polygons_
  for (int i = 0; i<frame_cnt_; i++) {
    normal_polygons_.push_back(new vbl_array_1d<vsol_polygon_2d_sptr>());
    trees_.push_back(new vbl_array_1d<dbskr_tree_sptr>());
    observations_.push_back(new vbl_array_1d<dbinfo_observation_sptr>());
  }
  
  for (int i = 0; i<frame_cnt_; i++) {
    bpro1_storage_sptr input_storage_sptr = bvis1_manager::instance()->repository()->get_data("image",i,0);
    vidpro1_image_storage_sptr frame;
    frame.vertical_cast(input_storage_sptr);
    vil_image_resource_sptr image_sptr = frame->get_image();
    images_.push_back(image_sptr);

    for (unsigned j = 0; j<polygons_[i].size(); j++) {
      float *x = (*polygons_[i][j]).x;
      float *y = (*polygons_[i][j]).y;
      int n = (*polygons_[i][j]).n;

      vcl_vector<vsol_point_2d_sptr> vertices;
      for (int k = 0; k<n; k++) 
        vertices.push_back(new vsol_point_2d(x[k], y[k]));

      vsol_polygon_2d_sptr poly = new vsol_polygon_2d(vertices);
      normal_polygons_[i]->push_back(poly);
      trees_[i]->push_back(0);
      observations_[i]->push_back(0);
    }
  }

  vcl_cout << "The tool is activated!!!\n";
  frame_no_ = bvis1_manager::instance()->current_frame();
  active_polygon_no_ = 0;

  bvis1_manager::instance()->post_overlay_redraw();
  //bvis1_manager::instance()->post_redraw();
  return;
}

//: Set the tableau to work with, just checking if video is loaded in this tool
bool
dbru_matching_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if( tableau.ptr() != NULL && tableau->type_name() == "vgui_image_tableau" ){
    return true;
  }

  if (tableau.ptr() == NULL) {
    vcl_cout << " tableau pointer is null, just returning\n";
    return false;
  }

  vcl_cout << "NON vgui_image_tableau is set!! name is : " << tableau->type_name() << " \n";
  return false;
}

bool
dbru_matching_tool::handle( const vgui_event & e, 
                             const bvis1_view_tableau_sptr& view )
{
  if (active_view_ != view)
    return false;

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  //: as mouse moves update active polygon 
  if ((e.modifier == vgui_MODIFIER_NULL) && (e.type == vgui_MOTION)) {

    int temp = active_polygon_no_;
    //: find min distance polygon
    if (polygons_[frame_no_].size() == 0) return false;
    float min_dist = polygons_[frame_no_][0]->distance_squared(ix, iy);
    active_polygon_no_ = 0;
    for (unsigned int i = 1; i<polygons_[frame_no_].size(); i++) {
      float temp = polygons_[frame_no_][i]->distance_squared(ix, iy);
      if (temp < min_dist) {
        min_dist = temp;
        active_polygon_no_ = i;
      }
    }

    if (temp != active_polygon_no_) {
      vcl_cout << "\n------- " << frame_no_ << " ---- " << active_polygon_no_ << "----\n";
      vcl_cout << "-----------------------------------------\n";
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (gesture_right_arrow(e) || gesture_left_arrow(e)) {
    frame_no_ = bvis1_manager::instance()->current_frame();
    
    if (polygons_[frame_no_].size() == 0) return false;

    //: find min distance polygon
    float min_dist = polygons_[frame_no_][0]->distance_squared(ix, iy);
    active_polygon_no_ = 0;
    for (unsigned int i = 1; i<polygons_[frame_no_].size(); i++) {
      float temp = polygons_[frame_no_][i]->distance_squared(ix, iy);
      if (temp < min_dist) {
        min_dist = temp;
        active_polygon_no_ = i;
      }
    }
    
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (gesture_set_query(e) && 
      frame_no_ >= 0 && 
      unsigned(frame_no_) < polygons_.size() && 
      unsigned(active_polygon_no_) < polygons_[frame_no_].size()) 
  {
    query_polygon_.first = frame_no_;
    query_polygon_.second = active_polygon_no_;
    vcl_cout << "query polygon is set, frame_no: " << query_polygon_.first;
    vcl_cout << " active_polygon_no: " << query_polygon_.second << vcl_endl; 
    return false;
  }
  
  if (gesture_set_database(e) &&
      frame_no_ >= 0 && 
      unsigned(frame_no_) < polygons_.size() && 
      unsigned(active_polygon_no_) < polygons_[frame_no_].size()) 
  {
    database_polygon_.first = frame_no_;
    database_polygon_.second = active_polygon_no_;
    vcl_cout << "database polygon is set, frame_no: " << database_polygon_.first;
    vcl_cout << " active_polygon_no: " << database_polygon_.second << vcl_endl; 
    return false;
  }

  if (gesture_run(e)) {
    //----------------------------------
    // create the observations
    //----------------------------------
    vcl_cout << "matching query polygon frame no: " << query_polygon_.first << " polygon no: " << query_polygon_.second << vcl_endl;
    vcl_cout << " with database polygon frame no: " << database_polygon_.first << " polygon no: " << database_polygon_.second << vcl_endl;
    vsol_polygon_2d_sptr poly1 = (*normal_polygons_[query_polygon_.first])[query_polygon_.second];
    vsol_polygon_2d_sptr poly2 = (*normal_polygons_[database_polygon_.first])[database_polygon_.second];
    dbinfo_observation_sptr obs1 = (*observations_[query_polygon_.first])[query_polygon_.second];
    if (!obs1) {
      obs1 = new dbinfo_observation(0, images_[query_polygon_.first], poly1, true, true, false);
      (*observations_[query_polygon_.first])[query_polygon_.second] = obs1;
    }
    dbinfo_observation_sptr obs2 = (*observations_[database_polygon_.first])[database_polygon_.second];
    if (!obs2) {
      obs2 = new dbinfo_observation(0, images_[database_polygon_.first], poly2, true, true, false);
      (*observations_[database_polygon_.first])[database_polygon_.second] = obs2;
    }
    dbskr_tree_sptr tree1, tree2;
    if (choice_ == 0 || choice_ == 1) { // get the trees
      tree1 = (*trees_[query_polygon_.first])[query_polygon_.second];
      if (!tree1) {
        if (!get_tree(tree1, poly1))
          return false;
        (*trees_[query_polygon_.first])[query_polygon_.second] = tree1;
      }
      tree2 = (*trees_[database_polygon_.first])[database_polygon_.second];
      if (!tree2) {
        if (!get_tree(tree2, poly2)) 
          return false;
        (*trees_[database_polygon_.first])[database_polygon_.second] = tree2;
      }
    }    
    dbru_rcor_sptr output_rcor;
    dbskr_sm_cor_sptr sm_cor; dbcvr_cv_cor_sptr sil_cor;
    double cost;
    if (choice_ == 0)
      sm_cor = dbru_object_matcher::compute_shock_alignment(tree1, tree2, cost, false);
    else if (choice_ == 1)
      sm_cor = dbru_object_matcher::compute_shock_alignment_pmi(obs1, obs2, tree1, tree2, cost, false);
    else 
      sil_cor = dbru_object_matcher::compute_curve_alignment(obs1, obs2, cost, R_, rms_, restricted_cvmatch_ratio_, false);

    switch(choice_) {
      case 0 :   // normal edit distance 
      case 1 : { // pmi edit distance
        output_rcor = dbru_object_matcher::generate_rcor_shock_matching(obs1, obs2, sm_cor, true);  // verbose    
        break; }
      case 2 : { // dt
        output_rcor = dbru_object_matcher::generate_rcor_curve_matching_dt(obs1, obs2, sil_cor, true);  // verbose    
        break; }
      case 3 : { // line intersections
        output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line(obs1, obs2, sil_cor, true);  // verbose    
        break; }
    }
    rcor_storage_->set_rcor(output_rcor);
    shock_match_storage_->set_sm_cor(sm_cor);
    image_storage3_->set_image(output_rcor->get_appearance2_on_pixels1());
    image_storage1_->set_image(output_rcor->get_used_pixels1());
    image_storage2_->set_image(output_rcor->get_used_pixels2());
    bvis1_manager::instance()->repository()->store_data_at(image_storage1_, other_view_->frame());
    bvis1_manager::instance()->repository()->store_data_at(image_storage2_, other_view_->frame());
    bvis1_manager::instance()->repository()->store_data_at(image_storage3_, other_view_->frame());
  
    bvis1_manager::instance()->add_to_display(image_storage1_);
    bvis1_manager::instance()->add_to_display(image_storage2_);
    bvis1_manager::instance()->add_to_display(image_storage3_);

    active_view_->selector()->toggle("image1");
    active_view_->selector()->toggle("image2");
    active_view_->selector()->toggle("image3");

    bvis1_manager::instance()->display_current_frame();
    return false;
  }

  if (gesture_open_dialog(e)) {
    get_matching_info();
    vcl_cout << "algorithm: " << choices_[choice_] << " \n";
    return false;
  }
  
  if (e.type == vgui_DRAW_OVERLAY) {
    
    if (frame_no_ >= 0 && unsigned(frame_no_) < polygons_.size()) {
            
      //: draw each polygon in this frame
      for (unsigned int i = 0; i<polygons_[frame_no_].size(); i++) {
        normal_style_->apply_all();
        polygons_[frame_no_][i]->draw();
      }

      if (active_polygon_no_ >= 0 && unsigned(active_polygon_no_) < polygons_[frame_no_].size()) {
        active_style_->apply_all();
        polygons_[frame_no_][active_polygon_no_]->draw();
      }
      
    } else {
      vcl_cout << "Current frame number is not valid!\n";
    }
     
    return false;
  }

  return false;
}

//-----------------------------------------------------------------------------
//: Make and display a dialog box to get Intelligent Scissors parameters.
//-----------------------------------------------------------------------------
bool dbru_matching_tool::get_matching_info()
{
  vgui_dialog* dlg = new vgui_dialog("Get matching algo and parameters");
  
  dlg->choice("Category:", choices_, choice_);
  rms_ = 0.0f;
  R_ = 10.0f;
  restricted_cvmatch_ratio_ = 0.25f;
  dynamic_pruning_ = false;
  // when all the bugs are cleared in Amir's process this will be obsolete and should be removed
  use_Amir_shock_extraction_ = false;
  pruning_threshold_ = 1.0f;
  //: adjust pruning threshold dynamically
  //  if the outlining bounding box length is 100, use threshold 0.2 (base_thres_)
  //  if this length halves, use 0.2/4
  //  if this length doubles use 0.2*4
  base_thres_ = 0.2f;
  shock_curve_ds_ = 1.0f;
  dlg->field("rms_:", rms_);
  dlg->field("R_:", R_);
  dlg->field("restricted_cvmatch_ratio_: ", restricted_cvmatch_ratio_);
  dlg->checkbox("use Amir's shock extraction?: ", use_Amir_shock_extraction_);
  dlg->checkbox("dynamic_pruning after shock extraction?: ", dynamic_pruning_);
  dlg->field("pruning_threshold_: ", pruning_threshold_);
  dlg->field("base threshold if dynamic thresholding: ", base_thres_);
  dlg->field("shock_curve_ds_: ", shock_curve_ds_);
  
  bool return_value = dlg->ask();
  return return_value;
}

