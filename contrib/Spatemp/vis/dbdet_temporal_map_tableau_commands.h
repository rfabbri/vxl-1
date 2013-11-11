// This is brcv/seg/dbdet/vis/dbdet_temporal_map_tableau_commands.h
#ifndef dbdet_temporal_map_tableau_commands_h
#define dbdet_temporal_map_tableau_commands_h
//:
//\file
//\brief Various commands related to the temporal_map_tableau
#include <Spatemp/vis/dbdet_temporal_map_tableau.h>

//: command to toggle a boolean flag
class dbdet_temporal_map_tableau_toggle_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_toggle_command(dbdet_temporal_map_tableau* tab, const void* boolref) : 
       temporal_map_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    temporal_map_tableau->post_redraw(); 
  }

  dbdet_temporal_map_tableau *temporal_map_tableau;
  bool* bref;
};

//: command to set some double values 
class dbdet_temporal_map_tableau_set_double_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_set_double_command(dbdet_temporal_map_tableau* tab,
      const vcl_string& name, const void* intref) : temporal_map_tableau(tab),  iref_((float*)intref), name_(name) {}

  void execute() 
  { 
    float param_val = *iref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;
    *iref_ = param_val;
    temporal_map_tableau->post_redraw(); 
  }

  dbdet_temporal_map_tableau *temporal_map_tableau;
  float* iref_;
  vcl_string name_;
};
//: command to set some integer values 
class dbdet_temporal_map_tableau_set_int_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_set_int_command(dbdet_temporal_map_tableau* tab, 
    const vcl_string& name, const void* intref) : temporal_map_tableau(tab),  iref_((int*)intref), name_(name) {}

  void execute() 
  { 
    int param_val = *iref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *iref_ = param_val;
    temporal_map_tableau->post_redraw(); 
  }

  dbdet_temporal_map_tableau *temporal_map_tableau;
  int* iref_;
  vcl_string name_;
};
class dbdet_temporal_map_tableau_set_thresh_grouping_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_set_thresh_grouping_command(dbdet_temporal_map_tableau* tab, 
    const vcl_string& name, const void* intref) : temporal_map_tableau(tab),  iref_((int*)intref), name_(name) {}

  void execute() 
  { 
    int param_val = *iref_;
    vgui_dialog param_dlg("Set Threshold for grouping  Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *iref_ = param_val;
    temporal_map_tableau->post_redraw(); 
  }

  dbdet_temporal_map_tableau *temporal_map_tableau;
  int* iref_;
  vcl_string name_;
};
//: Command to choose a color
class dbdet_temporal_map_tableau_set_current_color_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_set_current_color_command(dbdet_temporal_map_tableau* tab, vnl_vector_fixed<float,3> * color ) : 
    temporal_map_tableau(tab), color_(color) {}

  void execute() 
  { 
    vgui_dialog param_dlg("Color");
    param_dlg.field("Red",   (*color_)[0]);
    param_dlg.field("Green", (*color_)[1]);
    param_dlg.field("Blue",  (*color_)[2]);

    

    if(!param_dlg.ask())
      return;
    temporal_map_tableau->post_redraw(); 
  }

  dbdet_temporal_map_tableau *temporal_map_tableau;
  vnl_vector_fixed<float,3> * color_;
};

//: Command to set styles
class dbdet_temporal_map_tableau_set_style_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_set_style_command(dbdet_temporal_map_tableau* tab, vnl_vector_fixed<float,3> * color, float* line_width ) 
      : temporal_map_tableau(tab), color_(color), line_width_(line_width) {}

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
    temporal_map_tableau->post_redraw(); 
  }

  dbdet_temporal_map_tableau *temporal_map_tableau;
  vnl_vector_fixed<float,3> * color_;
  float* line_width_;
};

//: Commands to interact with the ELG
class dbdet_temporal_map_tableau_ELG_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_ELG_command(dbdet_temporal_map_tableau* tab, int cmd): 
     temporal_map_tableau(tab), cmd_(cmd) {}

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
    temporal_map_tableau->post_redraw(); 
  }

  dbdet_temporal_map_tableau *temporal_map_tableau;
  int cmd_;
};

//: commands to call the Generic edge linker
class dbdet_temporal_map_tableau_genel_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_genel_command(dbdet_temporal_map_tableau* tab, int cmd): 
     temporal_map_tableau(tab), cmd_(cmd) {}

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
    temporal_map_tableau->post_redraw(); 
  }

  dbdet_temporal_map_tableau *temporal_map_tableau;
  int cmd_;
};

//: Commands to interact with the temporal_map algorithm
class dbdet_temporal_map_tableau_temporal_map_command : public vgui_command
{
 public:
  dbdet_temporal_map_tableau_temporal_map_command(dbdet_temporal_map_tableau* tab, int cmd): 
     temporal_map_tableau(tab), cmd_(cmd) {}

  void execute() 
  { 
    switch (cmd_)
    {
      case 1:  //construct the link graph
      {
        //open dialog box to get params
        int minsize = 4;
        int method = 0; //default
        bool use_all_curvelets = false;

        /* Amir: just construct the full link graph

        //Linking_algo_choices
        vcl_vector<vcl_string> linking_algo_choices;
        linking_algo_choices.push_back("all links");                              //0
        linking_algo_choices.push_back("immediate links only");                   //1
        linking_algo_choices.push_back("immediate reciprocal links");             //2
        linking_algo_choices.push_back("immediate reciprocal links with support");//3
        linking_algo_choices.push_back("triplet links with support");             //4

        //open a dialog box
        vgui_dialog param_dlg("Link graph options");
        param_dlg.field("Minimum group size", minsize);
        param_dlg.choice( "Linking Condition" , linking_algo_choices, method );
        param_dlg.checkbox("Use ALL curveles", use_all_curvelets);
        
        if(!param_dlg.ask())
          return;
        */

        ////now call the linking function
        //if (use_all_curvelets)
        //  temporal_map_->use_all_curvelets();
        //else
        //  temporal_map_->use_anchored_curvelets_only();

        //temporal_map_->construct_the_link_graph(minsize, method);
        break;
      }
      case 2:
      {
        //clear all contours
        //temporal_map_->clear_all_contours();
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

        //if (num_iter == -1)
        //  num_iter = temporal_map_->min_deg_to_link();

        //temporal_map_->extract_image_contours_from_the_link_graph(num_iter);

        break;
      }
      case 4: //extract contours by greedy tracing
      { 
        //temporal_map_->extract_image_contours_by_tracing();
        break;
      }
      case 5: //extract regular contours from the link graph
      {
        //temporal_map_->extract_regular_contours_from_the_link_graph();
        break;
      }
      case 6:
      {
        //Set min. deg overlap to Link
        //unsigned min_deg = temporal_map_->min_deg_to_link();

        ////open a dialog box to set the min. deg to link
        //vgui_dialog param_dlg("Set the min. deg. to link.");
        //param_dlg.field( "Min. deg to Link" , min_deg );
        //if(!param_dlg.ask())
        //  return;

        //temporal_map_->set_min_deg_to_link(min_deg);
        break;
      }
      case 7: //connect fragmented contours
      {
        //post_process_to_link_fragments();
        break;
      }
      case 8:
      {
        //temporal_map_->determine_accuracy_of_measurements();
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

        //open a dialog box to temporal_mapect the quality function
        vgui_dialog param_dlg("Curvelet Quality Functions");
        param_dlg.choice( "Quality Function" , quality_function_choices, method );
        if(!param_dlg.ask())
          return;

        //temporal_map_->evaluate_curvelet_quality(method);
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

        //prune_contours(contrast_threshold, length_threshold);
        break;
      }
      case 11: //form curvelets from contours
      {

      }
      default:
        break;
    }
    temporal_map_tableau->post_redraw(); 
  }

  //dbdet_temporal_map_sptr temporal_map_;
  dbdet_temporal_map_tableau *temporal_map_tableau;
  int cmd_;
};

#endif //dbdet_temporal_map_tableau_commands_h
