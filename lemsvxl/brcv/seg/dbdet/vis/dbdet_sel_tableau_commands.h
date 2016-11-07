// This is brcv/seg/dbdet/vis/dbdet_sel_tableau_commands.h
#ifndef dbdet_sel_tableau_commands_h
#define dbdet_sel_tableau_commands_h
//:
//\file
//\brief Various commands related to the sel_tableau
//\author Amir Tamrakar
//\date 09/25/07
//
//\verbatim
//  Modifications
//    Amir Tamrakar     Taken from dbdet_sel_tableau.cxx
//
//\endverbatim

#include <vcl_cstdio.h>
#include <dbdet/algo/dbdet_postprocess_contours.h>

//: command to toggle a boolean flag
class dbdet_sel_tableau_toggle_command : public vgui_command
{
 public:
  dbdet_sel_tableau_toggle_command(dbdet_sel_tableau* tab, const void* boolref) : 
       sel_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau *sel_tableau;
  bool* bref;
};

//: command to set some integer values 
class dbdet_sel_tableau_set_int_command : public vgui_command
{
 public:
  dbdet_sel_tableau_set_int_command(dbdet_sel_tableau* tab, 
    const vcl_string& name, const void* intref) : sel_tableau(tab),  iref_((int*)intref), name_(name) {}

  void execute() 
  { 
    int param_val = *iref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *iref_ = param_val;
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau *sel_tableau;
  int* iref_;
  vcl_string name_;
};

//: Command to set some double values
class dbdet_sel_tableau_set_double_command : public vgui_command
{
 public:
  dbdet_sel_tableau_set_double_command(dbdet_sel_tableau* tab, 
    const vcl_string& name, const void* dref) : sel_tableau(tab),  dref_((double*)dref), name_(name) {}

  void execute() 
  { 
    double param_val = *dref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *dref_ = param_val;
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau *sel_tableau;
  double* dref_;
  vcl_string name_;
};

//: Command to choose a color
class dbdet_sel_tableau_set_current_color_command : public vgui_command
{
 public:
  dbdet_sel_tableau_set_current_color_command(dbdet_sel_tableau* tab, vnl_vector_fixed<float,3> * color ) : 
    sel_tableau(tab), color_(color) {}

  void execute() 
  { 
    vgui_dialog param_dlg("Color");
    param_dlg.field("Red",   (*color_)[0]);
    param_dlg.field("Green", (*color_)[1]);
    param_dlg.field("Blue",  (*color_)[2]);

    

    if(!param_dlg.ask())
      return;
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau *sel_tableau;
  vnl_vector_fixed<float,3> * color_;
};

//: Command to set styles
class dbdet_sel_tableau_set_style_command : public vgui_command
{
 public:
  dbdet_sel_tableau_set_style_command(dbdet_sel_tableau* tab, vnl_vector_fixed<float,3> * color, float* line_width ) 
      : sel_tableau(tab), color_(color), line_width_(line_width) {}

  void execute() 
  { 
    char color[50];
    vcl_sprintf (color, "%.3f %.3f %.3f", (*color_)[0], (*color_)[1], (*color_)[2]);
    vcl_string col(color);

    vgui_dialog style_dlg("Change Style");
    style_dlg.inline_color("Color", col);
    style_dlg.field("Line Width", *line_width_);

    if(!style_dlg.ask())
      return;

    vcl_istringstream color_strm(col);
    color_strm >> color_[0] >> color_[1] >> color_[2];
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau *sel_tableau;
  vnl_vector_fixed<float,3> * color_;
  float* line_width_;
};

//: Commands to interact with the ELG
class dbdet_sel_tableau_ELG_command : public vgui_command
{
 public:
  dbdet_sel_tableau_ELG_command(dbdet_sel_tableau* tab, int cmd): 
     sel_tableau(tab), cmd_(cmd) {}

  void execute() 
  { 
    switch (cmd_)
    {
      case 0:
      {
      }
      default:
        break;
    }
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau *sel_tableau;
  int cmd_;
};

//: commands to call the Generic edge linker
class dbdet_sel_tableau_genel_command : public vgui_command
{
 public:
  dbdet_sel_tableau_genel_command(dbdet_sel_tableau* tab, int cmd): 
     sel_tableau(tab), cmd_(cmd) {}

  void execute() 
  { 
    switch (cmd_)
    {
      case 0:
      {
      }
      default:
        break;
    }
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau *sel_tableau;
  int cmd_;
};

//: Commands to interact with the sel algorithm
class dbdet_sel_tableau_sel_command : public vgui_command
{
 public:
  dbdet_sel_tableau *sel_tableau;
  int cmd_;

  dbdet_sel_tableau_sel_command(dbdet_sel_tableau* tab, int cmd): 
     sel_tableau(tab), cmd_(cmd) {}

  void execute() 
  { 
    switch (cmd_)
    {
      case 1:  //construct the link graph
      {
        //open dialog box to get params
        int minsize = 4;
        int method = 0; //default: all links
        bool use_all_curvelets = false;

        // Amir: just construct the full link graph
        ////Linking_algo_choices
        //vcl_vector<vcl_string> linking_algo_choices;
        //linking_algo_choices.push_back("all links");                              //0
        //linking_algo_choices.push_back("immediate links only");                   //1
        //linking_algo_choices.push_back("immediate reciprocal links");             //2
        //linking_algo_choices.push_back("immediate reciprocal links with support");//3
        //linking_algo_choices.push_back("triplet links with support");             //4

        //open a dialog box
        vgui_dialog param_dlg("Link graph options");
        param_dlg.field("Minimum group size", minsize);
        //param_dlg.choice( "Linking Condition" , linking_algo_choices, method );
        //param_dlg.checkbox("Use ALL curveles", use_all_curvelets);
        
        if(!param_dlg.ask())
          return;

        //now call the linking function
        if (use_all_curvelets)
          sel_tableau->edge_linker()->use_all_curvelets();
        else
          sel_tableau->edge_linker()->use_anchored_curvelets_only();

        sel_tableau->edge_linker()->construct_the_link_graph(minsize, method);
        break;
      }
      case 2:
      {
        //clear all contours
        sel_tableau->edge_linker()->clear_all_contours();
        break;
      }
      case 3: //extract contours by iterating on the link graph
      {
        //Set number of iterations
        unsigned num_iter = -1;

        //open a dialog box to set the min. deg to link
        vgui_dialog param_dlg("Set the # of iterations to run");
        param_dlg.field( "# of Linking iterations" , num_iter );
        if(!param_dlg.ask())
          return;

        if (num_iter == static_cast<unsigned>(-1))
          num_iter = sel_tableau->edge_linker()->min_deg_to_link();

        sel_tableau->edge_linker()->extract_image_contours_from_the_link_graph(num_iter);

        break;
      }
      case 4: //extract contours by greedy tracing
      { 
        //sel_tableau->edge_linker()->extract_image_contours_by_tracing();
        break;
      }
      case 5: //extract regular contours from the link graph
      {
        sel_tableau->edge_linker()->extract_regular_contours_from_the_link_graph();
        break;
      }
      case 6:
      {
        //Set min. deg overlap to Link
        unsigned min_deg = sel_tableau->edge_linker()->min_deg_to_link();

        //open a dialog box to set the min. deg to link
        vgui_dialog param_dlg("Set the min. deg. to link.");
        param_dlg.field( "Min. deg to Link" , min_deg );
        if(!param_dlg.ask())
          return;

        sel_tableau->edge_linker()->set_min_deg_to_link(min_deg);
        break;
      }
      case 7: //connect fragmented contours
      {
        sel_tableau->edge_linker()->post_process_to_link_contour_fragments();
        break;
      }
      case 8:
      {
        sel_tableau->edge_linker()->determine_accuracy_of_measurements();
        break;
      }
      case 9:
      {
        int method = 0;

        //quality_function_choices
        vcl_vector<vcl_string> quality_function_choices;
        quality_function_choices.push_back("average dist between edgels");     //0
        quality_function_choices.push_back("max distance between edgels");     //1
        quality_function_choices.push_back("max dist/L");                      //2

        //open a dialog box to select the quality function
        vgui_dialog param_dlg("Curvelet Quality Functions");
        param_dlg.choice( "Quality Function" , quality_function_choices, method );
        if(!param_dlg.ask())
          return;

        sel_tableau->edge_linker()->evaluate_curvelet_quality(method);
        break;
      }
      case 10: //prune short contours
      {
        double contrast_threshold = 3.0;
        double length_threshold = 5.0;

        //open a dialog box
        vgui_dialog param_dlg("Pruning options");
        param_dlg.field("Avg. Contrast Threshold", contrast_threshold);
        param_dlg.field("Length Threshold", length_threshold);
        
        if(!param_dlg.ask())
          return;

        prune_contours_by_length(sel_tableau->edge_linker()->get_curve_fragment_graph(), length_threshold);

        //sel_tableau->edge_linker()->prune_contours(contrast_threshold, length_threshold);
        break;
      }
      case 11: //form curvelets from contours
      {
        break;
      }
      case 12: //prune the link graph
      {
        sel_tableau->edge_linker()->prune_the_link_graph();
        break;
      }
      case 13: //prune the curvelets by quality
      {
        double quality_threshold = 0.8;

        //open a dialog box
        vgui_dialog param_dlg("Curvelet Pruning Options");
        param_dlg.field("Curvelet Quality Threshold", quality_threshold);
        
        if(!param_dlg.ask())
          return;

        sel_tableau->edge_linker()->prune_the_curvelets(quality_threshold);
        break;
      }
      case 14: //recompute the curvelet quality
      {
        sel_tableau->edge_linker()->recompute_curvelet_quality();
        break;
      }
      case 15: //make link graph bidirectionally consistent
      {
        sel_tableau->edge_linker()->make_link_graph_consistent();
        break;
      }
      case 16: // seperate the link graph into two separate ones based on linking direction
      {
        sel_tableau->edge_linker()->separate_link_graphs_and_cvlet_maps();
        break;
      }
      case 17: // setup for DHT mode
      {
        // 1) form the link graph
        sel_tableau->edge_linker()->construct_the_link_graph(0,0);//minsize, method
        
        // 2) make the linkgraph consistent
        sel_tableau->edge_linker()->make_link_graph_consistent();
        
        // 3) separate the link graph by direction
        sel_tableau->edge_linker()->separate_link_graphs_and_cvlet_maps();

        // 4) find regular 1-chains from the separated link graph
        sel_tableau->edge_linker()->extract_regular_contours_from_the_link_graph();
        
        break;
      }
      case 18: //construct naive link graph
      {
        double affinity_threshold = 1.0;
        double proximity_threshold = 3.0;

        //open a dialog box
        vgui_dialog param_dlg("Linking Options");
        param_dlg.field("Proximity Threshold", proximity_threshold);
        param_dlg.field("Pairwise Affinity Threshold", affinity_threshold);
        
        if(!param_dlg.ask())
          return;

        //form the naive link graph
        sel_tableau->edge_linker()->construct_naive_link_graph(proximity_threshold, affinity_threshold);

        break;
      }
      case 19: //prune the curvelets by max gap size
      {
        double gap_threshold = 3.0;

        //open a dialog box
        vgui_dialog param_dlg("Curvelet Pruning Options");
        param_dlg.field("Gap Threshold", gap_threshold);
        
        if(!param_dlg.ask())
          return;

        sel_tableau->edge_linker()->prune_curvelets_by_gaps(gap_threshold);
        break;
      }
      case 20: //fitpolyarc bundle to current edgel chains
      {
        sel_tableau->edge_linker()->fit_polyarcs_to_all_edgel_chains();
        break;
      }
      case 21: //prune the curvelets by radius (length)
      {
        double length_threshold = 5.5;

        //open a dialog box
        vgui_dialog param_dlg("Curvelet Pruning Options");
        param_dlg.field("Length Threshold", length_threshold);
        
        if(!param_dlg.ask())
          return;

        sel_tableau->edge_linker()->prune_curvelets_by_length(length_threshold);
        break;
      }
      case 22: //prune the curvelets based on c1 continuity test at the anchro edgel (only for GENO type cvlets)
      {
        sel_tableau->edge_linker()->prune_curvelets_by_c1_condition();
        break;
      }
      case 23: //construct all hyp trees from the end points
      {
        sel_tableau->edge_linker()->construct_all_path_from_EHTs();
        break;
      }
      case 24: //disambiguate the CFTG
      {
        sel_tableau->edge_linker()->disambiguate_the_CFTG();
        break;
      }
      case 25: //disambiguate the CFG
      {
        sel_tableau->edge_linker()->correct_CFG_topology();
        break;
      }
      // case 26 by Yuliang Guo 10/13/2010 
      case 26: //Get the Final Contours in one Step
      {
        sel_tableau->edge_linker()->extract_regular_contours_from_the_link_graph();
        sel_tableau->edge_linker()->Construct_Hypothesis_Tree();
        sel_tableau->edge_linker()->Disambiguation();
        sel_tableau->edge_linker()->correct_CFG_topology();
	sel_tableau->edge_linker()->Post_Process();

        break;
      }
     //By Naman Kumar
      case 27: //Construct Hypothesis Tree 
      {
        sel_tableau->edge_linker()->Construct_Hypothesis_Tree();
        break;
      }
     // By Naman Kumar
      case 28: //Disambiguate
      {
        sel_tableau->edge_linker()->Disambiguation();
        break;
      }
      // By Naman Kumar
      case 30: //Post Process
      {
        sel_tableau->edge_linker()->Post_Process();
        break;
      }
      default:
        break;
    }
    sel_tableau->post_redraw(); 
  }

};

#endif //dbdet_sel_tableau_commands_h
