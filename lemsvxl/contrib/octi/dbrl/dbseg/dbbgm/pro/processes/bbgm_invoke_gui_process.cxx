#include <vcl_string.h>
#include <vcl_sstream.h>
#include <dbbgm/bbgm_wavelet.h>
#include <dbbgm/bbgm_image_sptr.h>
#include <dbbgm/vis/dbbgm_image_tableau.h>
#include <vul/vul_arg.h>
#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <dbbgm/vis/dbbgm_inspector_tool.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h>
bool bbgm_invoke_gui_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> in_types(4);
  in_types[0]="bbgm_image_sptr";//original model
  in_types[1]="bbgm_image_sptr";//inverse transform model
  in_types[2]="bbgm_image_sptr";//forward transform model 
  in_types[3]="vil_image_view_base_sptr"; //probability map
  pro.set_input_types(in_types);
  return true;

}

bool bbgm_invoke_gui_process(bprb_func_process& pro)
{
	
	bbgm_image_sptr model=pro.get_input<bbgm_image_sptr>(0);
	bbgm_image_sptr inverse_transform_model=pro.get_input<bbgm_image_sptr>(1);
	bbgm_image_sptr forward_transform_model=pro.get_input<bbgm_image_sptr>(2);
    vil_image_view_base_sptr probability_map_ptr=pro.get_input<vil_image_view_base_sptr>(3);
	vil_image_view<float> prob_map;
	if (probability_map_ptr) 
		prob_map = *vil_convert_cast(float(), probability_map_ptr);

	dbbgm_image_tableau_new bg_tab(model); 
	dbbgm_image_tableau_new wv_tab(forward_transform_model);
	dbbgm_image_tableau_new subband_tab(inverse_transform_model);
	bgui_image_tableau_new prob_map_tab(prob_map);
	
	char** my_argv = new char*[1];
	my_argv[0] = new char[13];
	char* temp = my_argv[0];
	vcl_strcpy(my_argv[0], "--mfc-use-gl");
	 
	int argcc=1;
   // Initialize the toolkit.
    vgui::init(argcc, my_argv);

	
	 vgui_grid_tableau_new grid_tab(2,2);
	 wv_tab->set_mapping(new vgui_range_map_params(0,1,0,1,0,1,3.0f,3.0f,3.0f,false,false,false));
	 prob_map_tab->set_mapping(new vgui_range_map_params(0,1,1.0f));
	 if (model)
		grid_tab->add_at(vgui_viewer2D_tableau_new(bg_tab), 0,0);
	 if(inverse_transform_model)
		grid_tab->add_at(vgui_viewer2D_tableau_new(subband_tab), 1,0);
	 if(forward_transform_model)
		grid_tab->add_at(vgui_viewer2D_tableau_new(wv_tab), 0,1);
	 if(prob_map)
		grid_tab->add_at(vgui_viewer2D_tableau_new(prob_map_tab), 1,1);
	 vgui_shell_tableau_new shell(grid_tab);
	 vgui::run(shell, 1280, 720);
	 return true;
}