#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_grid_tableau.h>

#include <vidl/vidl_dc1394_istream.h>
#include <vidl/vidl_iidc1394_params.h>
#include <vidl/vidl_frame.h>
#include <vidl/gui/vidl_gui_param_dialog.h>
#include <dbpro/dbpro_delay.h>
#include <dbpro/dbpro_executive.h>
#include <dbpro/dbpro_observer.h>
#include <dbpro/dbpro_basic_processes.h>
#include <dbvidl2/pro/dbvidl2_source.h>
#include <dbvidl2/pro/dbvidl2_frame_to_resource.h>
#include <dbil/pro/dbil_convert_filters.h>
#include <dbil/pro/dbil_math_filters.h>
#include <dbil/pro/dbil_morphology_filters.h>

#include <dbbgm/pro/dbbgm_dbpro_filters.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_viewer.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_basic_functors.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <dbbgm/vis/dbbgm_image_tableau.h>

#include <dbpro/vis/dbpro_run_tableau.h>


#include <bsta/bsta_detector_gaussian.h>
#include <bsta/bsta_detector_mixture.h>

#include "set_file_commands.h"
#include "data_observers.h"





class bg_observer: public dbpro_observer
{
  public:
    bg_observer(const dbbgm_image_tableau_sptr& tab)
  : bg_tab(tab) {}
    //: Called by the process when the data is ready
    virtual bool notify(const dbpro_storage_sptr& data, unsigned long)
    {
      assert(bg_tab);
      bg_tab->update();
      return true;
    }
    dbbgm_image_tableau_sptr bg_tab;
};





int main(int argc, char** argv)
{
#if 0
  vidl_iidc1394_params::valid_options options;
  vidl_dc1394_istream::valid_params(options);
  if(options.cameras.empty()){
    vcl_cerr << "No cameras found" <<vcl_endl;
    return -1;
  }
  vidl_iidc1394_params params;
  params.port_ = options.cameras[0].port;
  params.node_ = options.cameras[0].node;
  params.video_mode_ = vidl_iidc1394_params::MODE_640x480_YUV422;
  params.frame_rate_ = vidl_iidc1394_params::FRAMERATE_15;
  vcl_cout << "found camera: " << options.cameras[0].model << vcl_endl;
  vcl_cout << "speed = "<<params.speed_<<vcl_endl;

  const vcl_vector< vidl_iidc1394_params::valid_options::valid_mode >& modes = options.cameras[0].modes;
  bool find_mode = false;
  for(unsigned int i=0; i<modes.size(); ++i){
    if(modes[i].mode == params.video_mode_){
      for(unsigned int j=0; j<modes[i].frame_rates.size(); ++j){
        if(modes[i].frame_rates[j] == params.frame_rate_){
          find_mode = true;
          break;
        }
      }
    }
  }
  if(!find_mode){
    vcl_cerr << "Requested camera mode not supported" <<vcl_endl;
    return -1;
  }

  vidl_dc1394_istream* istream = new vidl_dc1394_istream();
  istream->open("",2,false,params);
#endif

  float init_var = 0.1f, g_thresh = 3.0f, min_stdev = 0.02f;
  int max_components=3, window_size=300;
  float dist = 2.5f, wt = 0.7f, mradius = 2.0f;

  typedef bsta_gauss_if3 bsta_gauss_t;
  typedef bsta_gauss_t::vector_type _vector;
  typedef bsta_num_obs<bsta_gauss_t> gauss_type;
  typedef bsta_mixture<gauss_type> bsta_mix_t;
  typedef bsta_num_obs<bsta_mix_t> mix_gauss_type;
  typedef bsta_mg_grimson_window_updater<bsta_mix_t> updater_type;
  typedef bsta_top_weight_detector<mix_gauss_type, bsta_g_mdist_detector<bsta_gauss_t> > detector_type;
  bsta_gauss_t init_gauss(_vector(0.0f), _vector(init_var) );
  updater_type updater(init_gauss, max_components, g_thresh,
                       min_stdev, window_size);
  mix_gauss_type mixture;
  bbgm_image_sptr bg_model = new bbgm_image_of<mix_gauss_type>(0,0,mixture);

  bsta_g_mdist_detector<bsta_gauss_t> gmd(dist);
  detector_type detector(gmd, wt);

  vil_structuring_element se;
  se.set_to_disk(mradius);

  dbpro_executive graph;
  graph["source"]      = new dbvidl2_source(NULL);
  graph["to_resource"] = new dbvidl2_frame_to_resource(dbvidl2_frame_to_resource::ALLOW_WRAP,
                                                       VIL_PIXEL_FORMAT_BYTE,VIDL_PIXEL_COLOR_RGB,
                                                       dbvidl2_frame_to_resource::PLANES,
                                                       dbvidl2_frame_to_resource::REUSE_MEMORY);
  graph["to_float"]    = new dbil_convert_cast_filter<float>();
  graph["scale"]       = new dbil_math_scale_filter<float>(1.0/255.0);
  graph["bg_update"]   = new dbbgm_update_filter<updater_type,float>(updater);
  graph["bg_detect"]   = new dbbgm_detect_filter<detector_type,float>(detector);
  graph["bin_erode1"]  = new dbil_binary_erode_filter(se);
  graph["bin_dilate1"] = new dbil_binary_dilate_filter(se);
  graph["bin_erode2"]  = new dbil_binary_erode_filter(se);
  graph["bin_dilate2"] = new dbil_binary_dilate_filter(se);
  graph["delay"]       = new dbpro_delay(1,bg_model);
  graph["null_sink"]   = new dbpro_null_sink();


  graph["null_sink"]   ->connect_input(0,graph["bg_detect"],0);
  //graph["null_sink"]   ->connect_input(0,graph["bin_dilate2"],0);
  graph["bin_dilate2"] ->connect_input(0,graph["bin_erode2"],0);
  graph["bin_erode2"]  ->connect_input(0,graph["bin_erode1"],0);
  graph["bin_erode1"]  ->connect_input(0,graph["bin_dilate1"],0);
  graph["bin_dilate1"] ->connect_input(0,graph["bg_detect"],0);
  graph["bg_detect"]   ->connect_input(0,graph["bg_update"],0);
  graph["bg_detect"]   ->connect_input(1,graph["scale"],0);
  graph["delay"]       ->connect_input(0,graph["bg_update"],0);
  graph["bg_update"]   ->connect_input(0,graph["delay"],0);
  graph["bg_update"]   ->connect_input(1,graph["scale"],0);
  graph["scale"]       ->connect_input(0,graph["to_float"],0);
  graph["to_float"]    ->connect_input(0,graph["to_resource"],0);
  graph["to_resource"] ->connect_input(0,graph["source"],0);


  // initialize vgui
  vgui::init(argc, argv);


  // add an image tableau
  vgui_image_tableau_new video_tab;
  graph["to_resource"]->add_output_observer(0,new image_observer(video_tab));

  vgui_image_tableau_new seg_tab;
  graph["bg_detect"]->add_output_observer(0,new image_observer(seg_tab));

  //vgui_image_tableau_new segm_tab;
  //graph["bin_dilate2"]->add_output_observer(0,new image_observer(segm_tab));
  
  bbgm_mean_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type, 
                                        bsta_mean_functor<mix_gauss_type> >);
  bbgm_weight_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type, 
                                          bsta_weight_functor<mix_gauss_type> >);
  bbgm_variance_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type, 
                                            bsta_diag_covar_functor<mix_gauss_type> >);

  dbbgm_image_tableau_new bg_tab(bg_model);
  graph["bg_update"]->add_output_observer(0,new bg_observer(bg_tab));

  vgui_grid_tableau_new grid_tab(2,2);
  grid_tab->add_at(vgui_viewer2D_tableau_new(video_tab), 0,0);
  grid_tab->add_at(vgui_viewer2D_tableau_new(seg_tab), 1,0);
  grid_tab->add_at(vgui_viewer2D_tableau_new(bg_tab), 0,1);
  //grid_tab->add_at(vgui_viewer2D_tableau_new(segm_tab), 1,1);


  dbpro_run_tableau_sptr dbpro_tab = dbpro_run_tableau_new(graph);

  vgui_composite_tableau_new comp_tab(grid_tab, dbpro_tab);


  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(comp_tab);

  vgui_menu menu_bar;
  menu_bar.add("Open istream", new set_istream_command(graph["source"]));

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 800, 600, menu_bar);
}
