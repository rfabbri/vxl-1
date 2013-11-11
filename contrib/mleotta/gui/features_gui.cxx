#include <vgui/vgui.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>

#include <bgui/bgui_selector_tableau.h>

#include <vil/vil_math.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_gauss_filter.h>

#include <vidl/vidl_dc1394_istream.h>
#include <vidl/vidl_iidc1394_params.h>
#include <vidl/vidl_frame.h>
#include <vidl/gui/vidl_gui_param_dialog.h>
#include <dbpro/dbpro_delay.h>
#include <dbpro/dbpro_try_option.h>
#include <dbpro/dbpro_executive.h>
#include <dbpro/dbpro_basic_processes.h>
#include <dbvidl2/pro/dbvidl2_source.h>
#include <dbvidl2/pro/dbvidl2_frame_to_resource.h>
#include <dbvidl2/pro/dbvidl2_sink.h>
#include <dbvidl2/pro/dbvidl2_resource_to_frame.h>
#include <dbil/pro/dbil_convert_filters.h>
#include <dbil/pro/dbil_math_filters.h>
#include <dbil/pro/dbil_gaussian_blur_process.h>

#include <dbpro/vis/dbpro_run_tableau.h>
#include <vidreg/vidreg_detector.h>
#include <vidreg/vidreg_matcher.h>
#include <vidreg/vidreg_reconstructor.h>
#include <vidreg/vidreg_tracker.h>
#include <vidreg/vidreg_salient_group.h>
#include <vidreg/vidreg_feature_edgel.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_view.h>

#include "xio_processes.h"
#include "set_file_commands.h"
#include "data_observers.h"

class dbgui_screengrab_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      vgui::run_till_idle();
      vil_image_view<vxl_byte> screen = vgui_utils::colour_buffer_to_view();

      output(0, vil_new_image_resource_of_view(screen));
      return DBPRO_VALID;
    }
};


class vidreg_features_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vil_image_resource_sptr));
      vil_image_resource_sptr in_img = input<vil_image_resource_sptr>(0);

      if(detector_.detect_features(in_img)){
        vidreg_feature_group_sptr f = detector_.features();
        output(0, f);
        return DBPRO_VALID;
      }
      vcl_cerr << "features failed!"<<vcl_endl;
      return DBPRO_INVALID;
    }
  private:
    vidreg_detector detector_;
};


class vidreg_match_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vcl_vector<vidreg_salient_group_sptr>));
      vcl_vector<vidreg_salient_group_sptr> fixed_groups = input<vcl_vector<vidreg_salient_group_sptr> >(0);

      assert(input_type_id(1) == typeid(vidreg_feature_group_sptr));
      vidreg_feature_group_sptr last_features = input<vidreg_feature_group_sptr>(1);

      assert(input_type_id(2) == typeid(vidreg_feature_group_sptr));
      vidreg_feature_group_sptr new_features = input<vidreg_feature_group_sptr>(2);

      assert(input_type_id(3) == typeid(vcl_vector<rgrl_transformation_sptr>));
      vcl_vector<rgrl_transformation_sptr> xforms =
      input<vcl_vector<rgrl_transformation_sptr> >(3);

      if(!new_features || !last_features){
        return DBPRO_WAIT;
      }


      if(matcher_.assisted_match(xforms, *last_features, *new_features)){
      //if(matcher_.match(fixed_groups, *last_features, *new_features)){
        vcl_vector<vidreg_salient_group_sptr> groups = matcher_.salient_groups();
        output(0, groups);
        return DBPRO_VALID;
      }

      return DBPRO_INVALID;
    }
  private:
    vidreg_matcher matcher_;
};


class vidreg_reconstruct_filter : public dbpro_sink
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vcl_vector<vidreg_salient_group_sptr>));
      vcl_vector<vidreg_salient_group_sptr> groups = input<vcl_vector<vidreg_salient_group_sptr> >(0);

      if(groups.size() < 2){
        return DBPRO_VALID;
      }

      vidreg_reconstructor recon(groups[1]);

      return DBPRO_VALID;
    }
  private:
};


class vidreg_tracker_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vcl_vector<vidreg_salient_group_sptr>));
      vcl_vector<vidreg_salient_group_sptr> groups
          = input<vcl_vector<vidreg_salient_group_sptr> >(0);

      tracker_.process_frame(groups[1]->matches()[0],groups[1]->weight_map());

      output(0, tracker_.current_tracks());

      return DBPRO_VALID;
    }
  private:
    vidreg_tracker tracker_;
};


class vidreg_track_f_filter : public dbpro_filter
{
  public:
    vidreg_track_f_filter(unsigned int n)
      : filter_(n), last_state_(vidreg_track_filter::NOT_READY) {};

    //: Runs the filter
    dbpro_signal run(unsigned long timestamp, dbpro_debug_observer* const debug = NULL)
    {
      // notify the debugger if available
      if (debug) debug->notify_enter(this, timestamp);
  
      if(timestamp > this->timestamp_){

        typedef vcl_vector<vcl_pair<unsigned long, rgrl_feature_sptr> > Tvec;

        this->timestamp_ = timestamp;
        if(this->last_signal_ != DBPRO_EOS){
          this->last_signal_ = this->request_inputs(timestamp,debug);
          if(this->last_signal_ == DBPRO_VALID){
            assert(input_type_id(0) == typeid(Tvec));
            Tvec tracks = input<Tvec >(0);
            filter_.push_frame(tracks);
          }
        }
        if(this->last_signal_ == DBPRO_VALID || this->last_signal_ == DBPRO_EOS){
          Tvec out_tracks;
          last_state_ = filter_.pop_frame(out_tracks);

          switch(last_state_){
            case vidreg_track_filter::VALID:
              output(0, out_tracks);
              this->last_signal_ = DBPRO_VALID;
              break;
            case vidreg_track_filter::NOT_READY:
              this->last_signal_ = DBPRO_WAIT;
              break;
            case vidreg_track_filter::DONE:
              this->last_signal_ = DBPRO_EOS;
              break;
          }
        }
        this->notify_observers(this->last_signal_);
        this->clear();
      }
      // notify the debugger if available
      if (debug) debug->notify_exit(this, timestamp);
      return this->last_signal_;
    }

    //: Execute this process
    dbpro_signal execute()
    {
      return DBPRO_INVALID;
    }

  private:
    vidreg_track_filter         filter_;
    vidreg_track_filter::state  last_state_;
};




int main(int argc, char** argv)
{

  dbpro_executive graph;
  graph.enable_debug();
  graph["source"]      = new dbvidl2_source(NULL);
  graph["to_resource"] = new dbvidl2_frame_to_resource(dbvidl2_frame_to_resource::ALLOW_WRAP,
                                                       VIL_PIXEL_FORMAT_BYTE,VIDL_PIXEL_COLOR_MONO,
                                                       dbvidl2_frame_to_resource::PLANES,
                                                       dbvidl2_frame_to_resource::REUSE_MEMORY);
  //graph["edge"]        = new dbil_sobel_filter<vxl_byte,float>();
  //graph["hessian"]     = new dbil_hessian_filter<float>();
  //graph["adjust"]      = new dbil_math_scale_and_offset_filter<float>(1/100.0,0.0);
  //graph["to_byte"]     = new dbil_convert_stretch_range_filter<float>(); //dbil_convert_cast_filter<vxl_byte>();
  graph["fdet"]        = new vidreg_features_filter();
  graph["fmatch"]      = new vidreg_match_filter();
  //graph["recon"]       = new vidreg_reconstruct_filter();
  graph["f_delay"]     = new dbpro_delay(1,vidreg_feature_group_sptr(NULL));
  graph["m_delay"]     = new dbpro_delay(1,vcl_vector<vidreg_salient_group_sptr>(0));
  graph["null_sink"]   = new dbpro_null_sink();

  graph["track"]       = new vidreg_tracker_filter();
  graph["filter_trk"]  = new vidreg_track_f_filter(5);

  // Screen capture processes
  graph["screen_grab"] = new dbgui_screengrab_filter();
  graph["to_frame"]    = new dbvidl2_resource_to_frame();
  graph["sink"]        = new dbvidl2_sink(NULL);

  // XML streaming processes
  graph["xml_xform_sink"]    = new dbpro_xio_sink();
  graph["xml_xform_w"]       = new vidreg_xml_write_xform_filter();
  graph["xml_xform_source"]  = new dbpro_xio_source();
  graph["xml_xform_r"]       = new vidreg_xml_read_xform_filter();
  graph["x_delay"]           = new dbpro_delay(1,vcl_vector<rgrl_transformation_sptr>(0));
  graph["track_read_try"]    = new dbpro_try_option();
  graph["xml_track_w"]       = new vidreg_xml_write_tracks_filter();
  graph["xml_track_sink"]    = new dbpro_xio_sink();
  graph["xml_track_r"]       = new vidreg_xml_read_tracks_filter();
  graph["xml_track_source"]  = new dbpro_xio_source();

  //graph["null_sink"]   ->connect_input(0,graph["to_byte"],0);
  //graph["to_byte"]     ->connect_input(0,graph["adjust"],0);
  //graph["to_byte"]     ->connect_input(0,graph["hessian"],0);
  //graph["hessian"]     ->connect_input(0,graph["edge"],0);
  //graph["edge"]        ->connect_input(0,graph["to_resource"],0);


  graph["null_sink"]   ->connect_input(1,graph["track_read_try"],0);
  //graph["null_sink"]   ->connect_input(1,graph["filter_trk"],0);
  graph["null_sink"]   ->connect_input(0,graph["to_resource"],0);

  graph["track"]       ->connect_input(0,graph["fmatch"],0);
  //graph["recon"]       ->connect_input(0,graph["fmatch"],0);
  graph["fmatch"]      ->connect_input(0,graph["m_delay"],0);
  graph["fmatch"]      ->connect_input(1,graph["f_delay"],0);
  graph["fmatch"]      ->connect_input(2,graph["fdet"],0);
  //graph["null_sink"]   ->connect_input(1,graph["delay"],0);
  //graph["null_sink"]   ->connect_input(0,graph["fdet"],1);

  graph["f_delay"]     ->connect_input(0,graph["fdet"],0);
  graph["m_delay"]     ->connect_input(0,graph["fmatch"],0);
  graph["fdet"]        ->connect_input(0,graph["to_resource"],0);
  graph["to_resource"] ->connect_input(0,graph["source"],0);

  // Screen capture links
  graph["sink"]        ->connect_input(0,graph["to_frame"],0);
  graph["to_frame"]    ->connect_input(0,graph["screen_grab"],0);
  graph["screen_grab"] ->connect_input(0,graph["track_read_try"],0);

  // XML streaming links
  graph["xml_xform_sink"]  ->connect_input(0,graph["xml_xform_w"],0);
  graph["xml_xform_w"]     ->connect_input(0,graph["fmatch"],0);
  graph["xml_xform_r"]     ->connect_input(0,graph["xml_xform_source"],0);
  //graph["null_sink"]       ->connect_input(0,graph["xml_extract"],0);
  graph["x_delay"]         ->connect_input(0,graph["xml_xform_r"],0);
  graph["fmatch"]          ->connect_input(3,graph["x_delay"],0);


  graph["filter_trk"]      ->connect_input(0,graph["track_read_try"],0);
  graph["xml_track_w"]     ->connect_input(0,graph["filter_trk"],0);
  graph["xml_track_sink"]  ->connect_input(0,graph["xml_track_w"],0);
  graph["track_read_try"]  ->connect_input(0,graph["xml_track_r"],0);
  graph["track_read_try"]  ->connect_input(1,graph["track"],0);
  graph["xml_track_r"]     ->connect_input(0,graph["xml_track_source"],0);
  // initialize vgui
  vgui::init(argc, argv);


  // add an image tableau
  vgui_image_tableau_new video_tab;
  graph["to_resource"]->add_output_observer(0,new image_observer(video_tab));


  //vgui_image_tableau_new output_tab;
  //graph["to_byte"]->add_output_observer(0,new image_observer(output_tab));

  bgui_selector_tableau_new selector_tab;
  vgui_easy2D_tableau_new feature_tab0, feature_tab1, feature_tab2, feature_tab3, feature_tab4;
  selector_tab->add(feature_tab0, "features 1");
  selector_tab->add(feature_tab1, "features 2");
  selector_tab->add(feature_tab2, "features 3");
  selector_tab->add(feature_tab3, "features 4");
  selector_tab->add(feature_tab4, "features 5");


  graph["fmatch"]->add_output_observer(0,new feature_observer(feature_tab4,4,0.0,1.0,1.0,true));
  graph["fmatch"]->add_output_observer(0,new feature_observer(feature_tab3,3,1.0,1.0,0.0,true));
  graph["fmatch"]->add_output_observer(0,new feature_observer(feature_tab2,2,0.0,0.0,1.0,true));
  graph["fmatch"]->add_output_observer(0,new feature_observer(feature_tab1,1,0.0,1.0,0.0,true));
  graph["fmatch"]->add_output_observer(0,new feature_observer(feature_tab0,0,1.0,0.0,0.0,true));

  vgui_easy2D_tableau_new track_tab;
  selector_tab->add(track_tab, "tracks");
  graph["track_read_try"]->add_output_observer(0,new track_observer(track_tab,1.0,0.0,0.0,true));
  graph["filter_trk"]->add_output_observer(0,new track_observer(track_tab,0.0,1.0,0.0,true));


  //vgui_easy2D_tableau_new edgel_tab;
  //graph["fdet"]->add_output_observer(0,new edgel_observer(edgel_tab,1.0,0.0,0.0,true));
  //selector_tab->add(edgel_tab, "edgels");

  vgui_viewer2D_tableau_new view_tab(vgui_composite_tableau_new(video_tab, selector_tab));

  //vgui_grid_tableau_new grid_tab(2,1);
  //grid_tab->add_at(vgui_viewer2D_tableau_new(video_tab), 0,0);
  //grid_tab->add_at(vgui_viewer2D_tableau_new(output_tab), 1,0);

  dbpro_run_tableau_sptr dbpro_tab = dbpro_run_tableau_new(graph);

  vgui_composite_tableau_new comp_tab(dbpro_tab, view_tab);


  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(comp_tab);

  vgui_menu menu_bar, file_menu, video_menu, xml_menu;
  video_menu.add("Input", new set_istream_command(graph["source"]));
  video_menu.add("Output", new set_ostream_command(graph["sink"]));
  file_menu.add("Open Video Stream",video_menu);

  xml_menu.add("Xfrom Input", new set_xml_infile_command(graph["xml_xform_source"]));
  xml_menu.add("Xform Output", new set_xml_outfile_command(graph["xml_xform_sink"]));
  xml_menu.add("Track Input", new set_xml_infile_command(graph["xml_track_source"]));
  xml_menu.add("Track Output", new set_xml_outfile_command(graph["xml_track_sink"]));
  file_menu.add("Open XML Stream",xml_menu);
  menu_bar.add("Streams",file_menu);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 1250, 800, menu_bar);
}
